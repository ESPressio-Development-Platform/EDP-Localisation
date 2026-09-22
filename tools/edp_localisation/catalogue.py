from __future__ import annotations

from pathlib import Path
from typing import Any

from .common import (
    AUTHORING_SCHEMA_VERSION, PLATFORM_BUNDLE_SCHEMA_VERSION, ToolError, canonical_bcp47,
    canonical_bundle_digest, load_json, parse_sha256_pin, reject_unknown, require_array,
    require_int, require_keys, require_object, require_text, width_max,
)
from .model_types import CatalogueDomain, CatalogueString, CatalogueSubDomain, Manifest, MetadataPolicy, PlatformManifest, PlatformPin


_METADATA_ALLOWED = {"name", "description", "copyright"}
_POLICY_VALUES = {"translatable", "canonical"}
_STATUS_VALUES = {"active", "retired"}


def _parse_metadata_policy(value: Any, source: Path, location: str) -> MetadataPolicy:
    if value is None:
        return MetadataPolicy()
    obj = require_object(value, source, location)
    reject_unknown(obj, _METADATA_ALLOWED, source, location)
    values = {
        "name": "translatable",
        "description": "translatable",
        "copyright": "canonical",
    }
    for key, raw in obj.items():
        policy = require_text(raw, source, f"{location}.{key}", nonempty=True)
        if policy not in _POLICY_VALUES:
            raise ToolError(f"{source}:{location}.{key}: expected 'translatable' or 'canonical'")
        values[key] = policy
    return MetadataPolicy(**values)


def parse_manifest(source_root: Path) -> Manifest:
    path = source_root / "manifest.json"
    root = require_object(load_json(path), path, "$")
    allowed = {
        "schemaVersion",
        "terminalLanguage",
        "supportedLanguages",
        "identifierWidths",
        "platformLocalisation",
    }
    require_keys(root, allowed, path, "$")
    reject_unknown(root, allowed, path, "$")

    schema_version = require_int(root["schemaVersion"], path, "$.schemaVersion")
    if schema_version != AUTHORING_SCHEMA_VERSION:
        raise ToolError(f"{path}:$.schemaVersion: unsupported schema version {schema_version}")

    terminal = canonical_bcp47(root["terminalLanguage"], path, "$.terminalLanguage")
    supported_raw = require_array(root["supportedLanguages"], path, "$.supportedLanguages")
    supported = tuple(
        canonical_bcp47(value, path, f"$.supportedLanguages[{index}]")
        for index, value in enumerate(supported_raw)
    )
    if not supported:
        raise ToolError(f"{path}:$.supportedLanguages: at least one language is required")
    if len(set(supported)) != len(supported):
        raise ToolError(f"{path}:$.supportedLanguages: duplicate language identity")
    if terminal not in supported:
        raise ToolError(f"{path}:$.terminalLanguage: terminal language is not supported")

    widths = require_object(root["identifierWidths"], path, "$.identifierWidths")
    width_keys = {"domain", "subDomain", "string"}
    require_keys(widths, width_keys, path, "$.identifierWidths")
    reject_unknown(widths, width_keys, path, "$.identifierWidths")
    domain_bytes = require_int(widths["domain"], path, "$.identifierWidths.domain")
    subdomain_bytes = require_int(widths["subDomain"], path, "$.identifierWidths.subDomain")
    string_bytes = require_int(widths["string"], path, "$.identifierWidths.string")
    for name, value in (
        ("domain", domain_bytes),
        ("subDomain", subdomain_bytes),
        ("string", string_bytes),
    ):
        if value not in (1, 2, 4):
            raise ToolError(f"{path}:$.identifierWidths.{name}: expected 1, 2, or 4")

    platform = require_object(root["platformLocalisation"], path, "$.platformLocalisation")
    platform_keys = {"identity", "version", "integrity"}
    require_keys(platform, platform_keys, path, "$.platformLocalisation")
    reject_unknown(platform, platform_keys, path, "$.platformLocalisation")
    pin = PlatformPin(
        identity=require_text(platform["identity"], path, "$.platformLocalisation.identity", nonempty=True),
        version=require_text(platform["version"], path, "$.platformLocalisation.version", nonempty=True),
        integrity_hex=parse_sha256_pin(platform["integrity"], path, "$.platformLocalisation.integrity"),
    )

    return Manifest(
        terminal_language=terminal,
        supported_languages=supported,
        domain_bytes=domain_bytes,
        subdomain_bytes=subdomain_bytes,
        string_bytes=string_bytes,
        platform_pin=pin,
    )


def _parse_catalogue(
    path: Path,
    *,
    domain_bytes: int,
    subdomain_bytes: int,
    string_bytes: int,
    allowed_domain: callable,
    authority_name: str,
) -> dict[int, CatalogueDomain]:
    root = require_object(load_json(path), path, "$")
    allowed_root = {"schemaVersion", "domains"}
    require_keys(root, allowed_root, path, "$")
    reject_unknown(root, allowed_root, path, "$")
    if require_int(root["schemaVersion"], path, "$.schemaVersion") != AUTHORING_SCHEMA_VERSION:
        raise ToolError(f"{path}:$.schemaVersion: unsupported schema version")

    domains_raw = require_array(root["domains"], path, "$.domains")
    domains: dict[int, CatalogueDomain] = {}
    domain_symbols: set[str] = set()
    max_domain = width_max(domain_bytes)
    max_subdomain = width_max(subdomain_bytes)
    max_string = width_max(string_bytes)

    for d_index, raw_domain in enumerate(domains_raw):
        location = f"$.domains[{d_index}]"
        obj = require_object(raw_domain, path, location)
        allowed = {"id", "symbol", "status", "metadata", "subDomains"}
        require_keys(obj, {"id", "status", "subDomains"}, path, location)
        reject_unknown(obj, allowed, path, location)
        identifier = require_int(obj["id"], path, f"{location}.id")
        if identifier < 0 or identifier > max_domain:
            raise ToolError(f"{path}:{location}.id: DomainId is out of range")
        if not allowed_domain(identifier):
            raise ToolError(f"{path}:{location}.id: DomainId {identifier} is not owned by {authority_name}")
        if identifier in domains:
            raise ToolError(f"{path}:{location}.id: duplicate DomainId {identifier}")
        status = require_text(obj["status"], path, f"{location}.status", nonempty=True)
        if status not in _STATUS_VALUES:
            raise ToolError(f"{path}:{location}.status: expected active or retired")
        symbol = obj.get("symbol")
        if symbol is not None:
            symbol = require_text(symbol, path, f"{location}.symbol", nonempty=True)
        if status == "active" and not symbol:
            raise ToolError(f"{path}:{location}.symbol: active Domain requires a symbol")
        if symbol:
            if symbol in domain_symbols:
                raise ToolError(f"{path}:{location}.symbol: duplicate Domain symbol {symbol!r}")
            domain_symbols.add(symbol)
        metadata_policy = _parse_metadata_policy(obj.get("metadata"), path, f"{location}.metadata")

        subdomains_raw = require_array(obj["subDomains"], path, f"{location}.subDomains")
        subdomains: dict[int, CatalogueSubDomain] = {}
        sub_symbols: set[str] = set()
        for s_index, raw_sub in enumerate(subdomains_raw):
            s_loc = f"{location}.subDomains[{s_index}]"
            sobj = require_object(raw_sub, path, s_loc)
            s_allowed = {"id", "symbol", "status", "metadata", "strings"}
            require_keys(sobj, {"id", "status", "strings"}, path, s_loc)
            reject_unknown(sobj, s_allowed, path, s_loc)
            sid = require_int(sobj["id"], path, f"{s_loc}.id")
            if sid < 0 or sid > max_subdomain:
                raise ToolError(f"{path}:{s_loc}.id: SubDomainId is out of range")
            if sid in subdomains:
                raise ToolError(f"{path}:{s_loc}.id: duplicate SubDomainId {sid}")
            sstatus = require_text(sobj["status"], path, f"{s_loc}.status", nonempty=True)
            if sstatus not in _STATUS_VALUES:
                raise ToolError(f"{path}:{s_loc}.status: expected active or retired")
            ssymbol = sobj.get("symbol")
            if ssymbol is not None:
                ssymbol = require_text(ssymbol, path, f"{s_loc}.symbol", nonempty=True)
            if sstatus == "active" and not ssymbol:
                raise ToolError(f"{path}:{s_loc}.symbol: active SubDomain requires a symbol")
            if ssymbol:
                if ssymbol in sub_symbols:
                    raise ToolError(f"{path}:{s_loc}.symbol: duplicate SubDomain symbol {ssymbol!r}")
                sub_symbols.add(ssymbol)
            if sid == 0:
                if "metadata" in sobj:
                    raise ToolError(
                        f"{path}:{s_loc}.metadata: SubDomainId 0 is the Domain-root metadata namespace; "
                        "its reserved metadata policy is owned by the enclosing Domain"
                    )
                smetadata = metadata_policy
            else:
                smetadata = _parse_metadata_policy(
                    sobj.get("metadata"),
                    path,
                    f"{s_loc}.metadata"
                )

            strings_raw = require_array(sobj["strings"], path, f"{s_loc}.strings")
            strings: dict[int, CatalogueString] = {}
            string_symbols: set[str] = set()
            for t_index, raw_string in enumerate(strings_raw):
                t_loc = f"{s_loc}.strings[{t_index}]"
                tobj = require_object(raw_string, path, t_loc)
                t_allowed = {"id", "symbol", "status", "localisation", "context", "notes"}
                require_keys(tobj, {"id", "status"}, path, t_loc)
                reject_unknown(tobj, t_allowed, path, t_loc)
                tid = require_int(tobj["id"], path, f"{t_loc}.id")
                if tid < 3 or tid > max_string:
                    raise ToolError(f"{path}:{t_loc}.id: ordinary StringId must be >=3 and fit configured width")
                if tid in strings:
                    raise ToolError(f"{path}:{t_loc}.id: duplicate StringId {tid}")
                tstatus = require_text(tobj["status"], path, f"{t_loc}.status", nonempty=True)
                if tstatus not in _STATUS_VALUES:
                    raise ToolError(f"{path}:{t_loc}.status: expected active or retired")
                tsymbol = tobj.get("symbol")
                if tsymbol is not None:
                    tsymbol = require_text(tsymbol, path, f"{t_loc}.symbol", nonempty=True)
                if tstatus == "active" and not tsymbol:
                    raise ToolError(f"{path}:{t_loc}.symbol: active String requires a symbol")
                if tsymbol:
                    if tsymbol in string_symbols:
                        raise ToolError(f"{path}:{t_loc}.symbol: duplicate String symbol {tsymbol!r}")
                    string_symbols.add(tsymbol)
                localisation = tobj.get("localisation")
                if tstatus == "active":
                    if localisation is None:
                        raise ToolError(f"{path}:{t_loc}.localisation: active String requires localisation policy")
                    localisation = require_text(localisation, path, f"{t_loc}.localisation", nonempty=True)
                    if localisation not in _POLICY_VALUES:
                        raise ToolError(f"{path}:{t_loc}.localisation: expected translatable or canonical")
                elif localisation is not None:
                    localisation = require_text(localisation, path, f"{t_loc}.localisation", nonempty=True)
                    if localisation not in _POLICY_VALUES:
                        raise ToolError(f"{path}:{t_loc}.localisation: expected translatable or canonical")
                context = tobj.get("context")
                if context is not None:
                    context = require_text(context, path, f"{t_loc}.context")
                notes = tobj.get("notes")
                if notes is not None:
                    notes = require_text(notes, path, f"{t_loc}.notes")
                strings[tid] = CatalogueString(tid, tsymbol, tstatus, localisation, context, notes)

            subdomains[sid] = CatalogueSubDomain(sid, ssymbol, sstatus, smetadata, strings)
        domains[identifier] = CatalogueDomain(identifier, symbol, status, metadata_policy, subdomains)
    return domains


def parse_application_catalogue(source_root: Path, manifest: Manifest) -> dict[int, CatalogueDomain]:
    return _parse_catalogue(
        source_root / "strings_catalogue.json",
        domain_bytes=manifest.domain_bytes,
        subdomain_bytes=manifest.subdomain_bytes,
        string_bytes=manifest.string_bytes,
        allowed_domain=lambda value: value >= 1,
        authority_name="the Application catalogue",
    )


def parse_platform_manifest(bundle_root: Path) -> PlatformManifest:
    path = bundle_root / "platform-localisation-manifest.json"
    root = require_object(load_json(path), path, "$")
    allowed = {"schemaVersion", "identity", "version"}
    require_keys(root, allowed, path, "$")
    reject_unknown(root, allowed, path, "$")
    if require_int(root["schemaVersion"], path, "$.schemaVersion") != PLATFORM_BUNDLE_SCHEMA_VERSION:
        raise ToolError(f"{path}:$.schemaVersion: unsupported platform bundle schema")
    return PlatformManifest(
        identity=require_text(root["identity"], path, "$.identity", nonempty=True),
        version=require_text(root["version"], path, "$.version", nonempty=True),
    )


def validate_platform_pin(bundle_root: Path, manifest: Manifest) -> PlatformManifest:
    bundle_manifest = parse_platform_manifest(bundle_root)
    pin = manifest.platform_pin
    if bundle_manifest.identity != pin.identity:
        raise ToolError(
            f"{bundle_root}: platform identity mismatch: expected {pin.identity!r}, got {bundle_manifest.identity!r}"
        )
    if bundle_manifest.version != pin.version:
        raise ToolError(
            f"{bundle_root}: platform version mismatch: expected {pin.version!r}, got {bundle_manifest.version!r}"
        )
    actual_integrity = canonical_bundle_digest(bundle_root)
    if actual_integrity != pin.integrity_hex:
        raise ToolError(
            f"{bundle_root}: platform integrity mismatch: expected SHA-256 {pin.integrity_hex}, got {actual_integrity}"
        )
    return bundle_manifest


def parse_platform_catalogue(bundle_root: Path, manifest: Manifest) -> dict[int, CatalogueDomain]:
    return _parse_catalogue(
        bundle_root / "strings_catalogue.json",
        domain_bytes=manifest.domain_bytes,
        subdomain_bytes=manifest.subdomain_bytes,
        string_bytes=manifest.string_bytes,
        allowed_domain=lambda value: value == 0,
        authority_name="the Platform bundle",
    )
