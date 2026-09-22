from __future__ import annotations

import struct

from .common import ToolError
from .fingerprint import Fingerprint
from .model import SemanticModel
from .edpl_format import *

class PayloadPool:
    def __init__(self) -> None:
        self._data = bytearray()
        self._offsets: dict[bytes, tuple[int, int]] = {}

    def add(self, text: str | bytes) -> tuple[int, int]:
        value = text.encode("utf-8") if isinstance(text, str) else bytes(text)
        if not value:
            return 0, 0
        existing = self._offsets.get(value)
        if existing is not None:
            return existing
        offset = len(self._data)
        self._data.extend(value)
        result = (offset, len(value))
        self._offsets[value] = result
        return result

    @property
    def data(self) -> bytes:
        return bytes(self._data)


def _u(value: int, width: int) -> bytes:
    return int(value).to_bytes(width, "little", signed=False)


def _language_metadata(model: SemanticModel, language: str, fingerprint: Fingerprint) -> bytes:
    source = model.languages[language]
    language_bytes = language.encode("ascii")
    parent_bytes = source.parent.encode("ascii") if source.parent is not None else b""
    flags = TERMINAL_FLAG if language == model.manifest.terminal_language else 0
    return (
        bytes((SECTION_VERSION, flags, len(language_bytes), len(parent_bytes)))
        + fingerprint.runtime
        + language_bytes
        + parent_bytes
    )


def _language_display_names(model: SemanticModel, language: str, payload: PayloadPool) -> bytes:
    source = model.languages[language]
    entries = bytearray()
    targets = sorted(source.display_names)
    if len(targets) > 0xFFFF:
        raise ToolError(f"language {language}: too many language display-name entries")
    for target in targets:
        target_bytes = target.encode("ascii")
        target_offset, target_length = payload.add(target_bytes)
        display_offset, display_length = payload.add(source.display_names[target])
        entries.extend(struct.pack("<IB3xII", target_offset, target_length, display_offset, display_length))
    return struct.pack("<BBH", SECTION_VERSION, 0, len(targets)) + bytes(entries)


def _general_strings(model: SemanticModel, language: str, payload: PayloadPool) -> bytes:
    values = model.languages[language].strings
    grouped: dict[int, dict[int, list[tuple[int, str]]]] = {}
    for (did, sid, tid), value in sorted(values.items()):
        grouped.setdefault(did, {}).setdefault(sid, []).append((tid, value))

    domains: list[tuple[int, int, int]] = []
    subdomains: list[tuple[int, int, int]] = []
    strings: list[tuple[int, int, int]] = []
    for did in sorted(grouped):
        first_sub = len(subdomains)
        for sid in sorted(grouped[did]):
            first_string = len(strings)
            for tid, value in sorted(grouped[did][sid]):
                offset, length = payload.add(value)
                strings.append((tid, offset, length))
            subdomains.append((sid, first_string, len(strings) - first_string))
        domains.append((did, first_sub, len(subdomains) - first_sub))

    db = model.manifest.domain_bytes
    sb = model.manifest.subdomain_bytes
    tb = model.manifest.string_bytes
    domain_table = b"".join(_u(did, db) + struct.pack("<II", first, count) for did, first, count in domains)
    sub_table = b"".join(_u(sid, sb) + struct.pack("<II", first, count) for sid, first, count in subdomains)
    string_table = b"".join(_u(tid, tb) + struct.pack("<II", offset, length) for tid, offset, length in strings)
    domain_offset = 28
    sub_offset = domain_offset + len(domain_table)
    string_offset = sub_offset + len(sub_table)
    header = struct.pack(
        "<BBHIIIIII",
        SECTION_VERSION, 0, 0, len(domains), len(subdomains), len(strings),
        domain_offset, sub_offset, string_offset,
    )
    return header + domain_table + sub_table + string_table


def _presentation_pair(value: str | None, pool: PayloadPool) -> tuple[int, int, bool]:
    if value is None:
        return 0, 0, False
    offset, length = pool.add(value)
    return offset, length, True


def _type_schema(model: SemanticModel, language: str, payload: PayloadPool) -> bytes:
    schema = model.schema
    if schema is None:
        return struct.pack("<BBBBIIII", SECTION_VERSION, 0, 0, 0, 0, 0, 20, 20)

    source = model.languages[language]
    type_records: list[tuple[bytes, int, int, int, int, int, int, int]] = []
    field_records: list[tuple[int, int, int, int, int, int]] = []

    for type_id, presentation in sorted(source.types.items()):
        first_field = len(field_records)
        for field_id, fp in sorted(presentation.fields.items()):
            name_offset, name_length, name_present = _presentation_pair(fp.name, payload)
            description_offset, description_length, description_present = _presentation_pair(fp.description, payload)
            flags = (NAME_PRESENT if name_present else 0) | (DESCRIPTION_PRESENT if description_present else 0)
            field_records.append((field_id, flags, name_offset, name_length, description_offset, description_length))
        name_offset, name_length, name_present = _presentation_pair(presentation.name, payload)
        description_offset, description_length, description_present = _presentation_pair(presentation.description, payload)
        flags = (NAME_PRESENT if name_present else 0) | (DESCRIPTION_PRESENT if description_present else 0)
        type_records.append((
            type_id, flags, name_offset, name_length, description_offset, description_length,
            first_field, len(field_records) - first_field,
        ))

    type_table = bytearray()
    for type_id, flags, no, nl, do, dl, first_field, field_count in type_records:
        type_table.extend(type_id)
        type_table.extend(bytes((flags, 0, 0, 0)))
        type_table.extend(struct.pack("<IIIIII", no, nl, do, dl, first_field, field_count))

    field_table = bytearray()
    for field_id, flags, no, nl, do, dl in field_records:
        field_table.extend(_u(field_id, schema.field_identifier_bytes))
        field_table.extend(bytes((flags, 0, 0, 0)))
        field_table.extend(struct.pack("<IIII", no, nl, do, dl))

    type_offset = 20
    field_offset = type_offset + len(type_table)
    header = struct.pack(
        "<BBBBIIII", SECTION_VERSION, 0, schema.type_identifier_bytes, schema.field_identifier_bytes,
        len(type_records), len(field_records), type_offset, field_offset,
    )
    return header + bytes(type_table) + bytes(field_table)


def _payload_section(payload: PayloadPool) -> bytes:
    data = payload.data
    return struct.pack("<BBHI", SECTION_VERSION, 0, 0, len(data)) + data


def build_language_pack(model: SemanticModel, language: str, fingerprint: Fingerprint) -> bytes:
    if language not in model.languages:
        raise ToolError(f"unsupported language {language!r}")
    pool = PayloadPool()
    sections: list[tuple[int, bytes]] = [
        (SECTION_LANGUAGE_METADATA, _language_metadata(model, language, fingerprint)),
        (SECTION_LANGUAGE_DISPLAY_NAMES, _language_display_names(model, language, pool)),
        (SECTION_GENERAL_STRINGS, _general_strings(model, language, pool)),
        (SECTION_TYPE_SCHEMA, _type_schema(model, language, pool)),
    ]
    sections.append((SECTION_UTF8_PAYLOAD, _payload_section(pool)))

    directory = bytearray()
    offset = HEADER_SIZE
    for section_type, data in sections:
        directory.extend(struct.pack("<BBHII", section_type, 0, 0, offset, len(data)))
        offset += len(data)
    total_size = offset
    preamble = bytearray(PREAMBLE_SIZE)
    preamble[0:4] = b"EDPL"
    preamble[4] = EDPL_FORMAT_MAJOR
    preamble[5] = EDPL_FORMAT_MINOR
    preamble[6:8] = HEADER_SIZE.to_bytes(2, "little")
    preamble[8] = len(sections)
    preamble[9] = 0
    preamble[10] = model.manifest.domain_bytes
    preamble[11] = model.manifest.subdomain_bytes
    preamble[12] = model.manifest.string_bytes
    preamble[13] = 0
    preamble[14:18] = total_size.to_bytes(4, "little")
    preamble[18:22] = b"\0\0\0\0"

    result = bytearray(preamble + directory + b"".join(data for _kind, data in sections))
    checksum = crc32c(bytes(result), (CRC_OFFSET, CRC_SIZE))
    result[CRC_OFFSET:CRC_OFFSET + CRC_SIZE] = checksum.to_bytes(4, "little")
    return bytes(result)
