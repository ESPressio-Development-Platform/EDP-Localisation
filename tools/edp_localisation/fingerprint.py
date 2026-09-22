from __future__ import annotations

import hashlib
from dataclasses import dataclass

from .common import FINGERPRINT_CANONICALISATION_VERSION
from .model import SemanticModel


@dataclass(frozen=True)
class Fingerprint:
    runtime: bytes
    digest: bytes
    canonical_stream: bytes

    @property
    def runtime_hex(self) -> str:
        return self.runtime.hex().upper()

    @property
    def digest_hex(self) -> str:
        return self.digest.hex().upper()


class _Stream:
    def __init__(self) -> None:
        self.data = bytearray()
        self.data.extend(b"EDP-LOCALISATION-CONTRACT-FINGERPRINT-V1\0")
        self.u8(FINGERPRINT_CANONICALISATION_VERSION)

    def tag(self, value: int) -> None:
        self.u8(value)

    def u8(self, value: int) -> None:
        self.data.extend(int(value).to_bytes(1, "little", signed=False))

    def u32(self, value: int) -> None:
        self.data.extend(int(value).to_bytes(4, "little", signed=False))

    def raw(self, value: bytes) -> None:
        self.data.extend(value)

    def bytes(self, value: bytes) -> None:
        self.u32(len(value))
        self.raw(value)

    def text(self, value: str) -> None:
        self.bytes(value.encode("utf-8"))

    def optional_text(self, value: str | None) -> None:
        if value is None:
            self.u8(0)
        elif value == "":
            self.u8(1)
        else:
            self.u8(2)
            self.text(value)


def _status_byte(status: str) -> int:
    return 1 if status == "active" else 2


def _policy_byte(policy: str | None) -> int:
    if policy is None:
        return 0
    return 1 if policy == "translatable" else 2


def build_fingerprint(model: SemanticModel) -> Fingerprint:
    s = _Stream()
    manifest = model.manifest

    s.tag(0x01)
    s.u8(manifest.domain_bytes)
    s.u8(manifest.subdomain_bytes)
    s.u8(manifest.string_bytes)
    s.u8(model.schema.type_identifier_bytes if model.schema else 0)
    s.u8(model.schema.field_identifier_bytes if model.schema else 0)

    s.tag(0x02)
    languages = sorted(manifest.supported_languages)
    s.u32(len(languages))
    s.text(manifest.terminal_language)
    for language in languages:
        source = model.languages[language]
        s.text(language)
        if source.parent is None:
            s.u8(0)
        else:
            s.u8(1)
            s.text(source.parent)

    s.tag(0x03)
    s.u32(len(model.catalogue))
    for did, domain in sorted(model.catalogue.items()):
        s.u32(did)
        s.u8(_status_byte(domain.status))
        s.u8(_policy_byte(domain.metadata_policy.name))
        s.u8(_policy_byte(domain.metadata_policy.description))
        s.u8(_policy_byte(domain.metadata_policy.copyright))
        s.u32(len(domain.subdomains))
        for sid, sub in sorted(domain.subdomains.items()):
            s.u32(sid)
            s.u8(_status_byte(sub.status))
            s.u8(_policy_byte(sub.metadata_policy.name))
            s.u8(_policy_byte(sub.metadata_policy.description))
            s.u8(_policy_byte(sub.metadata_policy.copyright))
            s.u32(len(sub.strings))
            for tid, item in sorted(sub.strings.items()):
                s.u32(tid)
                s.u8(_status_byte(item.status))
                s.u8(_policy_byte(item.localisation))

    active_general_keys: list[tuple[int, int, int]] = []
    for did, domain in sorted(model.catalogue.items()):
        if domain.status != "active":
            continue
        for sid, sub in sorted(domain.subdomains.items()):
            if sub.status != "active":
                continue
            active_general_keys.extend((did, sid, metadata_id) for metadata_id in (0, 1, 2))
            active_general_keys.extend(
                (did, sid, tid)
                for tid, item in sorted(sub.strings.items())
                if item.status == "active"
            )

    s.tag(0x04)
    s.u32(len(languages))
    s.u32(len(active_general_keys))
    for language in languages:
        s.text(language)
        values = model.languages[language].strings
        for did, sid, tid in active_general_keys:
            s.u32(did)
            s.u32(sid)
            s.u32(tid)
            s.optional_text(values.get((did, sid, tid)))

    s.tag(0x05)
    s.u32(len(languages))
    for language in languages:
        s.text(language)
        display_names = model.languages[language].display_names
        for target in languages:
            s.text(target)
            s.optional_text(display_names.get(target))

    s.tag(0x06)
    if model.schema is None:
        s.u8(0)
    else:
        s.u8(1)
        schema = model.schema
        s.u8(schema.type_identifier_bytes)
        s.u8(schema.field_identifier_bytes)
        s.u32(len(schema.types))
        for type_id, type_value in sorted(schema.types.items()):
            s.bytes(type_id)
            s.u8(_status_byte(type_value.status))
            s.u8(1 if type_value.presentation_exposed else 0)
            s.u32(len(type_value.fields))
            for field_id, field_value in sorted(type_value.fields.items()):
                s.u32(field_id)
                s.u8(_status_byte(field_value.status))
                s.u8(1 if field_value.presentation_exposed else 0)

    s.tag(0x07)
    if model.schema is None:
        s.u8(0)
    else:
        s.u8(1)
        schema = model.schema
        active_types = [
            (type_id, type_value)
            for type_id, type_value in sorted(schema.types.items())
            if type_value.status == "active"
        ]
        s.u32(len(languages))
        for language in languages:
            s.text(language)
            presentations = model.languages[language].types
            s.u32(len(active_types))
            for type_id, type_value in active_types:
                s.bytes(type_id)
                presentation = presentations.get(type_id)
                if type_value.presentation_exposed:
                    s.optional_text(presentation.name if presentation else None)
                    s.optional_text(presentation.description if presentation else None)
                else:
                    s.optional_text(None)
                    s.optional_text(None)
                exposed_fields = [
                    (field_id, field_value)
                    for field_id, field_value in sorted(type_value.fields.items())
                    if field_value.status == "active" and field_value.presentation_exposed
                ]
                s.u32(len(exposed_fields))
                for field_id, _field_value in exposed_fields:
                    s.u32(field_id)
                    field_presentation = presentation.fields.get(field_id) if presentation else None
                    s.optional_text(field_presentation.name if field_presentation else None)
                    s.optional_text(field_presentation.description if field_presentation else None)

    digest = hashlib.sha256(bytes(s.data)).digest()
    return Fingerprint(runtime=digest[:16], digest=digest, canonical_stream=bytes(s.data))
