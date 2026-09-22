from __future__ import annotations

from dataclasses import dataclass
from pathlib import Path
from typing import Callable, TypeVar

from .common import ToolError, canonical_bcp47, parse_type_identifier
from .compiler import load_build_manifest, verify_internal_generated_set
from .edpl import Pack, ParsedType


@dataclass(frozen=True)
class Resolution:
    status: str
    requested_language: str
    supplying_language: str | None = None
    fallback_used: bool = False
    value: str | None = None

    def as_json(self) -> dict:
        result: dict[str, object] = {
            "status": self.status,
            "requestedLanguage": self.requested_language,
        }
        if self.supplying_language is not None:
            result["supplyingLanguage"] = self.supplying_language
            result["fallbackUsed"] = self.fallback_used
        if self.value is not None:
            result["value"] = self.value
        return result


T = TypeVar("T")
_MISSING = object()


class GeneratedContractFamily:
    def __init__(self, root: Path, *, verify: bool = True) -> None:
        self.root = root.resolve()
        self.manifest = verify_internal_generated_set(self.root) if verify else load_build_manifest(self.root)
        self.terminal = self.manifest["terminalLanguage"]
        self.supported = tuple(self.manifest["supportedLanguages"])
        self.fingerprint = bytes.fromhex(self.manifest["contractFamilyFingerprint"])
        self.pack_paths: dict[str, Path] = {}
        for item in self.manifest["outputs"]:
            if item["kind"] == "language-pack":
                self.pack_paths[item["language"]] = self.root / item["path"]
        if set(self.pack_paths) != set(self.supported):
            raise ToolError("generated build manifest language-pack set does not match supportedLanguages")
        self._packs: dict[str, Pack] = {}

    def pack(self, language: str) -> Pack:
        if language not in self.pack_paths:
            raise ToolError(f"LanguagePackUnavailable: no generated pack for {language}")
        pack = self._packs.get(language)
        if pack is None:
            pack = Pack.from_path(self.pack_paths[language])
            if pack.language != language:
                raise ToolError(f"IncompatibleLanguagePack: pack identity {pack.language!r} does not match {language!r}")
            if pack.fingerprint != self.fingerprint:
                raise ToolError(f"IncompatibleLanguagePack: fingerprint mismatch for {language}")
            self._packs[language] = pack
        return pack

    def _terminal_pack(self) -> Pack:
        terminal = self.pack(self.terminal)
        if not terminal.terminal or terminal.parent is not None:
            raise ToolError("IncompatibleLanguagePack: build-manifest terminal pack is not terminal/parentless")
        return terminal

    def resolve(self, requested: str, lookup: Callable[[Pack], object]) -> Resolution:
        requested = canonical_bcp47(requested, Path("<command-line>"), "language")
        self._terminal_pack()
        current = requested
        for _ in range(len(self.supported)):
            pack = self.pack(current)
            if current == self.terminal:
                if not pack.terminal or pack.parent is not None:
                    raise ToolError("IncompatibleLanguagePack: terminal chain endpoint is inconsistent")
            elif pack.terminal:
                raise ToolError("IncompatibleLanguagePack: non-terminal chain member is marked terminal")
            value = lookup(pack)
            if value is not _MISSING:
                return Resolution(
                    status="Success",
                    requested_language=requested,
                    supplying_language=current,
                    fallback_used=current != requested,
                    value=value if isinstance(value, str) else str(value),
                )
            if current == self.terminal:
                return Resolution("NoStringFoundForIdentifier", requested)
            if pack.parent is None:
                raise ToolError("InvalidDataset: fallback chain terminated before terminal language")
            current = pack.parent
        raise ToolError("InvalidDataset: fallback traversal exceeded supported-language bound")

    def resolve_string(self, requested: str, domain: int, subdomain: int, string: int) -> Resolution:
        return self.resolve(
            requested,
            lambda pack: pack.strings[(domain, subdomain, string)]
            if (domain, subdomain, string) in pack.strings else _MISSING,
        )

    def resolve_language_name(self, requested: str, target: str) -> Resolution:
        target = canonical_bcp47(target, Path("<command-line>"), "target")
        return self.resolve(
            requested,
            lambda pack: pack.display_names[target] if target in pack.display_names else _MISSING,
        )

    def _type_id(self, text: str) -> bytes:
        sample = self._terminal_pack()
        width = sample.type_identifier_bytes
        if width == 0:
            raise ToolError("NoStringFoundForIdentifier: generated ContractFamily has no Type presentation universe")
        return parse_type_identifier(text, width, Path("<command-line>"), "type")

    def resolve_type_property(self, requested: str, type_text: str, property_name: str) -> Resolution:
        type_id = self._type_id(type_text)
        if property_name not in ("name", "description"):
            raise ValueError(property_name)

        def lookup(pack: Pack) -> object:
            value = pack.types.get(type_id)
            if value is None:
                return _MISSING
            represented = getattr(value, property_name)
            if represented is None:
                if property_name == "name" and pack.language == self.terminal:
                    raise ToolError("InvalidDataset: terminal Type exists but Name is absent")
                return _MISSING
            return represented

        return self.resolve(requested, lookup)

    def resolve_field_property(
        self,
        requested: str,
        type_text: str,
        field: int,
        property_name: str,
    ) -> Resolution:
        type_id = self._type_id(type_text)
        if property_name not in ("name", "description"):
            raise ValueError(property_name)

        def lookup(pack: Pack) -> object:
            type_value: ParsedType | None = pack.types.get(type_id)
            if type_value is None:
                return _MISSING
            field_value = type_value.fields.get(field)
            if field_value is None:
                return _MISSING
            represented = field_value[0 if property_name == "name" else 1]
            if represented is None:
                if property_name == "name" and pack.language == self.terminal:
                    raise ToolError("InvalidDataset: terminal Field exists but Name is absent")
                return _MISSING
            return represented

        return self.resolve(requested, lookup)

    def decompile_generated(self) -> dict:
        packs = {language: self.pack(language) for language in self.supported}
        terminal = packs[self.terminal]
        return {
            "schemaVersion": 1,
            "kind": "generated-contract-family-recovery",
            "contract": {
                "edplocFormat": self.manifest["edplocFormat"],
                "contractFamilyFingerprint": self.manifest["contractFamilyFingerprint"],
                "terminalLanguage": self.terminal,
                "supportedLanguages": list(self.supported),
                "identifierWidths": {
                    "domain": terminal.domain_bytes,
                    "subDomain": terminal.subdomain_bytes,
                    "string": terminal.string_bytes,
                    "type": terminal.type_identifier_bytes,
                    "field": terminal.field_identifier_bytes,
                },
            },
            "languages": {
                language: packs[language].recover_json()
                for language in self.supported
            },
        }
