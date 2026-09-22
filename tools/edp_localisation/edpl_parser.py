from __future__ import annotations

import struct
from dataclasses import dataclass
from pathlib import Path

from .common import ToolError, canonical_bcp47, format_type_identifier
from .edpl_format import *


@dataclass(frozen=True)
class Section:
    kind: int
    offset: int
    length: int


@dataclass
class ParsedType:
    name: str | None
    description: str | None
    fields: dict[int, tuple[str | None, str | None]]


class Pack:
    def __init__(self, data: bytes, *, validate_crc: bool = True) -> None:
        self.data = data
        self.sections: dict[int, Section] = {}
        self.domain_bytes = 0
        self.subdomain_bytes = 0
        self.string_bytes = 0
        self.file_crc32c = 0
        self.language = ""
        self.parent: str | None = None
        self.terminal = False
        self.fingerprint = b""
        self.type_identifier_bytes = 0
        self.field_identifier_bytes = 0
        self.payload = b""
        self.display_names: dict[str, str] = {}
        self.strings: dict[tuple[int, int, int], str] = {}
        self.types: dict[bytes, ParsedType] = {}
        self._parse(validate_crc=validate_crc)

    @classmethod
    def from_path(cls, path: Path, *, validate_crc: bool = True) -> "Pack":
        try:
            data = path.read_bytes()
        except OSError as exc:
            raise ToolError(f"{path}: unable to read pack: {exc}") from exc
        return cls(data, validate_crc=validate_crc)

    def _section_bytes(self, kind: int) -> bytes:
        section = self.sections[kind]
        return self.data[section.offset:section.offset + section.length]

    def _parse(self, *, validate_crc: bool) -> None:
        if len(self.data) < PREAMBLE_SIZE:
            raise ToolError("EDPL pack is shorter than fixed preamble")
        if self.data[:4] != b"EDPL":
            raise ToolError("not an EDPL pack")
        major, minor = self.data[4], self.data[5]
        if (major, minor) != (EDPL_FORMAT_MAJOR, EDPL_FORMAT_MINOR):
            raise ToolError(f"unsupported EDPL format {major}.{minor}")
        header_size = int.from_bytes(self.data[6:8], "little")
        section_count = self.data[8]
        if self.data[9] != 0 or self.data[13] != 0:
            raise ToolError("EDPL reserved preamble fields are non-zero")
        self.domain_bytes = self.data[10]
        self.subdomain_bytes = self.data[11]
        self.string_bytes = self.data[12]
        total_size = int.from_bytes(self.data[14:18], "little")
        self.file_crc32c = int.from_bytes(self.data[18:22], "little")
        if total_size != len(self.data):
            raise ToolError("EDPL TotalFileSize does not match resource size")
        minimum_header = PREAMBLE_SIZE + section_count * DIRECTORY_ENTRY_SIZE
        if header_size < minimum_header or header_size > len(self.data):
            raise ToolError("invalid EDPL HeaderSize")
        if validate_crc and crc32c(self.data, (CRC_OFFSET, CRC_SIZE)) != self.file_crc32c:
            raise ToolError("EDPL CRC32C mismatch")

        ranges: list[tuple[int, int]] = []
        for index in range(section_count):
            base = PREAMBLE_SIZE + index * DIRECTORY_ENTRY_SIZE
            kind, flags, reserved, offset, length = struct.unpack_from("<BBHII", self.data, base)
            if flags != 0 or reserved != 0:
                raise ToolError("EDPL section directory reserved fields are non-zero")
            if offset < header_size or offset + length > len(self.data):
                raise ToolError("EDPL section range is out of bounds")
            for existing_start, existing_end in ranges:
                if length and offset < existing_end and existing_start < offset + length:
                    raise ToolError("EDPL sections overlap")
            ranges.append((offset, offset + length))
            if kind in REQUIRED_SECTIONS:
                if kind in self.sections:
                    raise ToolError("duplicate required EDPL section")
                self.sections[kind] = Section(kind, offset, length)
        missing = set(REQUIRED_SECTIONS) - set(self.sections)
        if missing:
            raise ToolError(f"missing required EDPL sections: {sorted(missing)}")

        self._parse_language_metadata()
        self._parse_payload()
        self._parse_display_names()
        self._parse_general_strings()
        self._parse_type_schema()

    def _parse_language_metadata(self) -> None:
        data = self._section_bytes(SECTION_LANGUAGE_METADATA)
        if len(data) < 20 or data[0] != SECTION_VERSION:
            raise ToolError("invalid Language Metadata section")
        flags = data[1]
        if flags & ~TERMINAL_FLAG:
            raise ToolError("invalid Language Metadata flags")
        language_length, parent_length = data[2], data[3]
        if language_length == 0 or len(data) != 20 + language_length + parent_length:
            raise ToolError("invalid Language Metadata lengths")
        self.fingerprint = data[4:20]
        try:
            language = data[20:20 + language_length].decode("ascii")
            parent_bytes = data[20 + language_length:]
            parent = parent_bytes.decode("ascii") if parent_bytes else None
        except UnicodeDecodeError as exc:
            raise ToolError("Language Metadata identity is not ASCII") from exc
        synthetic = Path("<edpl>")
        self.language = canonical_bcp47(language, synthetic, "language")
        self.parent = canonical_bcp47(parent, synthetic, "parent") if parent is not None else None
        self.terminal = bool(flags & TERMINAL_FLAG)
        if self.terminal != (self.parent is None):
            raise ToolError("terminal/parent Language Metadata is inconsistent")
        if self.parent == self.language:
            raise ToolError("language pack self-parent is invalid")

    def _parse_payload(self) -> None:
        data = self._section_bytes(SECTION_UTF8_PAYLOAD)
        if len(data) < 8:
            raise ToolError("invalid Payload section")
        version, flags, reserved, length = struct.unpack_from("<BBHI", data, 0)
        if version != SECTION_VERSION or flags != 0 or reserved != 0 or len(data) != 8 + length:
            raise ToolError("invalid Payload section header")
        self.payload = data[8:]

    def _text(self, offset: int, length: int) -> str:
        if offset < 0 or length < 0 or offset + length > len(self.payload):
            raise ToolError("payload reference is out of bounds")
        try:
            value = self.payload[offset:offset + length].decode("utf-8", errors="strict")
        except UnicodeDecodeError as exc:
            raise ToolError("payload reference is not valid UTF-8") from exc
        if "\x00" in value:
            raise ToolError("payload representation contains embedded NUL")
        return value

    def _parse_display_names(self) -> None:
        data = self._section_bytes(SECTION_LANGUAGE_DISPLAY_NAMES)
        if len(data) < 4:
            raise ToolError("invalid Language Display Names section")
        version, reserved, count = struct.unpack_from("<BBH", data, 0)
        if version != SECTION_VERSION or reserved != 0 or len(data) != 4 + count * 16:
            raise ToolError("invalid Language Display Names header")
        previous: str | None = None
        synthetic = Path("<edpl>")
        for index in range(count):
            to, tl, reserved3, do, dl = struct.unpack_from("<IB3sII", data, 4 + index * 16)
            if reserved3 != b"\0\0\0":
                raise ToolError("invalid Language Display Names reserved bytes")
            target = self._text(to, tl)
            canonical_bcp47(target, synthetic, "display-target")
            if previous is not None and target <= previous:
                raise ToolError("Language Display Names are not strictly sorted")
            previous = target
            if target in self.display_names:
                raise ToolError("duplicate Language Display Name target")
            self.display_names[target] = self._text(do, dl)

    def _parse_general_strings(self) -> None:
        data = self._section_bytes(SECTION_GENERAL_STRINGS)
        if len(data) < 28:
            raise ToolError("invalid General Strings section")
        values = struct.unpack_from("<BBHIIIIII", data, 0)
        version, flags, reserved, dc, sc, tc, do, so, to = values
        if version != SECTION_VERSION or flags != 0 or reserved != 0:
            raise ToolError("invalid General Strings header")
        de = self.domain_bytes + 8
        se = self.subdomain_bytes + 8
        te = self.string_bytes + 8
        if do + dc * de > len(data) or so + sc * se > len(data) or to + tc * te > len(data):
            raise ToolError("General Strings tables are out of bounds")
        expected_sub = 0
        expected_string = 0
        previous_domain = -1
        for di in range(dc):
            base = do + di * de
            did = int.from_bytes(data[base:base + self.domain_bytes], "little")
            first_sub, sub_count = struct.unpack_from("<II", data, base + self.domain_bytes)
            if did <= previous_domain or first_sub != expected_sub or first_sub + sub_count > sc:
                raise ToolError("invalid Domain table ordering/range")
            previous_domain = did
            previous_sub = -1
            for sri in range(sub_count):
                si = first_sub + sri
                sbase = so + si * se
                sid = int.from_bytes(data[sbase:sbase + self.subdomain_bytes], "little")
                first_string, string_count = struct.unpack_from("<II", data, sbase + self.subdomain_bytes)
                if sid <= previous_sub or first_string != expected_string or first_string + string_count > tc:
                    raise ToolError("invalid SubDomain table ordering/range")
                previous_sub = sid
                previous_string = -1
                for tri in range(string_count):
                    ti = first_string + tri
                    tbase = to + ti * te
                    tid = int.from_bytes(data[tbase:tbase + self.string_bytes], "little")
                    po, pl = struct.unpack_from("<II", data, tbase + self.string_bytes)
                    if tid <= previous_string:
                        raise ToolError("invalid String table ordering")
                    previous_string = tid
                    self.strings[(did, sid, tid)] = self._text(po, pl)
                    expected_string += 1
            expected_sub += sub_count
        if expected_sub != sc or expected_string != tc:
            raise ToolError("General Strings ownership ranges are not complete")

    def _parse_type_schema(self) -> None:
        data = self._section_bytes(SECTION_TYPE_SCHEMA)
        if len(data) < 20:
            raise ToolError("invalid Type Schema section")
        version, flags, tb, fb, tc, fc, to, fo = struct.unpack_from("<BBBBIIII", data, 0)
        if version != SECTION_VERSION or flags != 0:
            raise ToolError("invalid Type Schema header")
        self.type_identifier_bytes = tb
        self.field_identifier_bytes = fb
        if tc == 0 and fc == 0 and tb == 0 and fb == 0:
            if to != 20 or fo != 20:
                raise ToolError("invalid empty Type Schema offsets")
            return
        if tb == 0 or fb not in (1, 2, 4):
            raise ToolError("invalid Type/Field identifier widths")
        te = tb + 28
        fe = fb + 20
        if to + tc * te > len(data) or fo + fc * fe > len(data):
            raise ToolError("Type Schema tables are out of bounds")
        previous_type: bytes | None = None
        expected_field = 0
        for ti in range(tc):
            base = to + ti * te
            type_id = bytes(data[base:base + tb])
            if previous_type is not None and type_id <= previous_type:
                raise ToolError("Type table is not strictly sorted")
            previous_type = type_id
            flags_value = data[base + tb]
            if flags_value & ~(NAME_PRESENT | DESCRIPTION_PRESENT) or data[base + tb + 1:base + tb + 4] != b"\0\0\0":
                raise ToolError("invalid Type representation flags/reserved bytes")
            no, nl, do, dl, first_field, field_count = struct.unpack_from("<IIIIII", data, base + tb + 4)
            if first_field != expected_field or first_field + field_count > fc:
                raise ToolError("invalid Type Field range")
            name = self._text(no, nl) if flags_value & NAME_PRESENT else None
            description = self._text(do, dl) if flags_value & DESCRIPTION_PRESENT else None
            if not (flags_value & NAME_PRESENT) and (no != 0 or nl != 0):
                raise ToolError("absent Type Name must encode zero offset/length")
            if not (flags_value & DESCRIPTION_PRESENT) and (do != 0 or dl != 0):
                raise ToolError("absent Type Description must encode zero offset/length")
            fields: dict[int, tuple[str | None, str | None]] = {}
            previous_field = -1
            for fri in range(field_count):
                fi = first_field + fri
                fbase = fo + fi * fe
                fid = int.from_bytes(data[fbase:fbase + fb], "little")
                if fid <= previous_field:
                    raise ToolError("Field table is not strictly sorted within Type")
                previous_field = fid
                fflags = data[fbase + fb]
                if fflags & ~(NAME_PRESENT | DESCRIPTION_PRESENT) or data[fbase + fb + 1:fbase + fb + 4] != b"\0\0\0":
                    raise ToolError("invalid Field representation flags/reserved bytes")
                fno, fnl, fdo, fdl = struct.unpack_from("<IIII", data, fbase + fb + 4)
                fname = self._text(fno, fnl) if fflags & NAME_PRESENT else None
                fdescription = self._text(fdo, fdl) if fflags & DESCRIPTION_PRESENT else None
                if not (fflags & NAME_PRESENT) and (fno != 0 or fnl != 0):
                    raise ToolError("absent Field Name must encode zero offset/length")
                if not (fflags & DESCRIPTION_PRESENT) and (fdo != 0 or fdl != 0):
                    raise ToolError("absent Field Description must encode zero offset/length")
                fields[fid] = (fname, fdescription)
                expected_field += 1
            self.types[type_id] = ParsedType(name, description, fields)
        if expected_field != fc:
            raise ToolError("Type Schema Field ownership ranges are incomplete")

    def recover_json(self) -> dict:
        types = {}
        for type_id, value in sorted(self.types.items()):
            entry: dict = {"fields": {}}
            if value.name is not None:
                entry["name"] = value.name
            if value.description is not None:
                entry["description"] = value.description
            for field_id, (name, description) in sorted(value.fields.items()):
                fentry: dict = {}
                if name is not None:
                    fentry["name"] = name
                if description is not None:
                    fentry["description"] = description
                entry["fields"][str(field_id)] = fentry
            types[format_type_identifier(type_id)] = entry
        domains: dict[str, dict] = {}
        for (did, sid, tid), value in sorted(self.strings.items()):
            domain = domains.setdefault(str(did), {"subDomains": {}})
            sub = domain["subDomains"].setdefault(str(sid), {"strings": {}})
            sub["strings"][str(tid)] = value
        return {
            "schemaVersion": 1,
            "language": {
                "language": self.language,
                "parent": self.parent,
                "terminal": self.terminal,
                "contractFamilyFingerprint": self.fingerprint.hex().upper(),
                "displayNames": dict(sorted(self.display_names.items())),
            },
            "strings": {"schemaVersion": 1, "domains": domains},
            "typeSchema": {"schemaVersion": 1, "types": types},
        }
