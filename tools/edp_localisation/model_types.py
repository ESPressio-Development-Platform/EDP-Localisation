from __future__ import annotations

from dataclasses import dataclass, field


@dataclass(frozen=True)
class PlatformPin:
    identity: str
    version: str
    integrity_hex: str


@dataclass(frozen=True)
class Manifest:
    terminal_language: str
    supported_languages: tuple[str, ...]
    domain_bytes: int
    subdomain_bytes: int
    string_bytes: int
    platform_pin: PlatformPin


@dataclass(frozen=True)
class MetadataPolicy:
    name: str = "translatable"
    description: str = "translatable"
    copyright: str = "canonical"

    def for_string_id(self, string_id: int) -> str:
        if string_id == 0:
            return self.name
        if string_id == 1:
            return self.description
        if string_id == 2:
            return self.copyright
        raise ValueError(string_id)


@dataclass(frozen=True)
class CatalogueString:
    identifier: int
    symbol: str | None
    status: str
    localisation: str | None
    context: str | None = None
    notes: str | None = None


@dataclass(frozen=True)
class CatalogueSubDomain:
    identifier: int
    symbol: str | None
    status: str
    metadata_policy: MetadataPolicy
    strings: dict[int, CatalogueString]


@dataclass(frozen=True)
class CatalogueDomain:
    identifier: int
    symbol: str | None
    status: str
    metadata_policy: MetadataPolicy
    subdomains: dict[int, CatalogueSubDomain]


@dataclass(frozen=True)
class SchemaField:
    identifier: int
    status: str
    presentation_exposed: bool
    symbol: str | None


@dataclass(frozen=True)
class SchemaType:
    identifier: bytes
    status: str
    presentation_exposed: bool
    symbol: str | None
    fields: dict[int, SchemaField]


@dataclass(frozen=True)
class SchemaUniverse:
    types: dict[bytes, SchemaType]


@dataclass
class FieldPresentation:
    name: str | None = None
    description: str | None = None


@dataclass
class TypePresentation:
    name: str | None = None
    description: str | None = None
    fields: dict[int, FieldPresentation] = field(default_factory=dict)


@dataclass
class LanguageSource:
    language: str
    parent: str | None
    self_name: str
    display_names: dict[str, str]
    strings: dict[tuple[int, int, int], str]
    types: dict[bytes, TypePresentation]


@dataclass(frozen=True)
class PlatformManifest:
    identity: str
    version: str


@dataclass
class SemanticModel:
    manifest: Manifest
    catalogue: dict[int, CatalogueDomain]
    schema: SchemaUniverse | None
    languages: dict[str, LanguageSource]
    platform_manifest: PlatformManifest
