from __future__ import annotations

from pathlib import Path
from typing import Any, Iterable

from .common import (
    AUTHORING_SCHEMA_VERSION, SCHEMA_INVENTORY_VERSION, ToolError, canonical_bcp47, load_json,
    parse_canonical_uint_key, parse_type_identifier, reject_unknown, require_bool, require_int,
    require_keys, require_object, require_text, width_max,
)
from .model_types import (
    CatalogueDomain, FieldPresentation, LanguageSource, Manifest, SchemaField, SchemaType, SchemaUniverse, TypePresentation,
)


_STATUS_VALUES = {"active", "retired"}


def _parse_strings_json(
    path: Path,
    *,
    catalogue: dict[int, CatalogueDomain],
    manifest: Manifest,
    allowed_domain: callable,
) -> dict[tuple[int, int, int], str]:
    root = require_object(load_json(path), path, "$")
    allowed_root = {"schemaVersion", "domains"}
    require_keys(root, allowed_root, path, "$")
    reject_unknown(root, allowed_root, path, "$")
    if require_int(root["schemaVersion"], path, "$.schemaVersion") != AUTHORING_SCHEMA_VERSION:
        raise ToolError(f"{path}:$.schemaVersion: unsupported schema version")
    domains = require_object(root["domains"], path, "$.domains")
    result: dict[tuple[int, int, int], str] = {}
    max_domain = width_max(manifest.domain_bytes)
    max_sub = width_max(manifest.subdomain_bytes)
    max_string = width_max(manifest.string_bytes)

    for domain_key, raw_domain in domains.items():
        did = parse_canonical_uint_key(domain_key, max_domain, path, f"$.domains[{domain_key!r}]")
        if not allowed_domain(did):
            raise ToolError(f"{path}:$.domains[{domain_key!r}]: DomainId {did} is not permitted in this source")
        domain = catalogue.get(did)
        if domain is None or domain.status != "active":
            raise ToolError(f"{path}:$.domains[{domain_key!r}]: unknown or inactive DomainId {did}")
        dobj = require_object(raw_domain, path, f"$.domains[{domain_key!r}]")
        require_keys(dobj, {"subDomains"}, path, f"$.domains[{domain_key!r}]")
        reject_unknown(dobj, {"subDomains"}, path, f"$.domains[{domain_key!r}]")
        subdomains = require_object(dobj["subDomains"], path, f"$.domains[{domain_key!r}].subDomains")

        for sub_key, raw_sub in subdomains.items():
            sid = parse_canonical_uint_key(sub_key, max_sub, path, f"$.domains[{domain_key!r}].subDomains[{sub_key!r}]")
            sub = domain.subdomains.get(sid)
            if sub is None or sub.status != "active":
                raise ToolError(f"{path}: Domain {did} references unknown or inactive SubDomainId {sid}")
            sobj = require_object(raw_sub, path, f"$.domains[{domain_key!r}].subDomains[{sub_key!r}]")
            require_keys(sobj, {"strings"}, path, f"$.domains[{domain_key!r}].subDomains[{sub_key!r}]")
            reject_unknown(sobj, {"strings"}, path, f"$.domains[{domain_key!r}].subDomains[{sub_key!r}]")
            strings = require_object(sobj["strings"], path, f"$.domains[{domain_key!r}].subDomains[{sub_key!r}].strings")

            for string_key, raw_value in strings.items():
                tid = parse_canonical_uint_key(string_key, max_string, path, f"...strings[{string_key!r}]")
                if tid >= 3:
                    item = sub.strings.get(tid)
                    if item is None or item.status != "active":
                        raise ToolError(f"{path}: Domain {did}/SubDomain {sid} references unknown or inactive StringId {tid}")
                value = require_text(raw_value, path, f"Domain {did}/SubDomain {sid}/String {tid}")
                key = (did, sid, tid)
                if key in result:
                    raise ToolError(f"{path}: duplicate representation for {key}")
                result[key] = value
    return result


def _parse_type_schema_json(
    path: Path,
    schema: SchemaUniverse | None,
) -> dict[bytes, TypePresentation]:
    root = require_object(load_json(path), path, "$")
    allowed_root = {"schemaVersion", "types"}
    require_keys(root, allowed_root, path, "$")
    reject_unknown(root, allowed_root, path, "$")
    if require_int(root["schemaVersion"], path, "$.schemaVersion") != AUTHORING_SCHEMA_VERSION:
        raise ToolError(f"{path}:$.schemaVersion: unsupported schema version")
    types = require_object(root["types"], path, "$.types")
    if types and schema is None:
        raise ToolError(f"{path}: Type/Field presentation source requires at least one schema inventory")
    result: dict[bytes, TypePresentation] = {}
    if schema is None:
        return result
    max_field = width_max(schema.field_identifier_bytes)

    for type_key, raw_type in types.items():
        type_id = parse_type_identifier(type_key, path, f"$.types[{type_key!r}]")
        authoritative = schema.types.get(type_id)
        if authoritative is None or authoritative.status != "active":
            raise ToolError(f"{path}:$.types[{type_key!r}]: unknown or inactive TypeIdentifier")
        tobj = require_object(raw_type, path, f"$.types[{type_key!r}]")
        allowed = {"name", "description", "fields"}
        require_keys(tobj, {"fields"}, path, f"$.types[{type_key!r}]")
        reject_unknown(tobj, allowed, path, f"$.types[{type_key!r}]")
        name = tobj.get("name")
        description = tobj.get("description")
        if name is not None:
            if not authoritative.presentation_exposed:
                raise ToolError(f"{path}:$.types[{type_key!r}].name: Type is not presentation-exposed")
            name = require_text(name, path, f"$.types[{type_key!r}].name")
        if description is not None:
            if not authoritative.presentation_exposed:
                raise ToolError(f"{path}:$.types[{type_key!r}].description: Type is not presentation-exposed")
            description = require_text(description, path, f"$.types[{type_key!r}].description")
        fields_raw = require_object(tobj["fields"], path, f"$.types[{type_key!r}].fields")
        fields: dict[int, FieldPresentation] = {}
        for field_key, raw_field in fields_raw.items():
            fid = parse_canonical_uint_key(field_key, max_field, path, f"$.types[{type_key!r}].fields[{field_key!r}]")
            auth_field = authoritative.fields.get(fid)
            if auth_field is None or auth_field.status != "active" or not auth_field.presentation_exposed:
                raise ToolError(f"{path}: Type {type_key} references unknown/inactive/non-exposed FieldId {fid}")
            fobj = require_object(raw_field, path, f"$.types[{type_key!r}].fields[{field_key!r}]")
            f_allowed = {"name", "description"}
            reject_unknown(fobj, f_allowed, path, f"$.types[{type_key!r}].fields[{field_key!r}]")
            if not fobj:
                raise ToolError(f"{path}: Type {type_key}/Field {fid}: empty Field presentation object is invalid")
            fname = fobj.get("name")
            fdescription = fobj.get("description")
            if fname is not None:
                fname = require_text(fname, path, f"Type {type_key}/Field {fid}/name")
            if fdescription is not None:
                fdescription = require_text(fdescription, path, f"Type {type_key}/Field {fid}/description")
            fields[fid] = FieldPresentation(fname, fdescription)
        if name is None and description is None and not fields:
            raise ToolError(f"{path}:$.types[{type_key!r}]: semantically empty Type object is invalid")
        result[type_id] = TypePresentation(name, description, fields)
    return result


def _inventory_files(inputs: Iterable[Path]) -> list[Path]:
    files: list[Path] = []
    for value in inputs:
        if value.is_file():
            files.append(value)
        elif value.is_dir():
            files.extend(sorted(p for p in value.rglob("*.json") if p.is_file()))
        else:
            raise ToolError(f"schema inventory input does not exist: {value}")
    return sorted(dict.fromkeys(path.resolve() for path in files))


def parse_schema_inventories(inputs: Iterable[Path]) -> SchemaUniverse | None:
    files = _inventory_files(inputs)
    if not files:
        return None
    field_bytes: int | None = None
    merged: dict[bytes, SchemaType] = {}

    for path in files:
        root = require_object(load_json(path), path, "$")
        allowed = {"schemaVersion", "fieldIdentifierBytes", "provenance", "types"}
        require_keys(root, {"schemaVersion", "fieldIdentifierBytes", "types"}, path, "$")
        reject_unknown(root, allowed, path, "$")
        if require_int(root["schemaVersion"], path, "$.schemaVersion") != SCHEMA_INVENTORY_VERSION:
            raise ToolError(f"{path}:$.schemaVersion: unsupported schema inventory version")
        fbytes = require_int(root["fieldIdentifierBytes"], path, "$.fieldIdentifierBytes")
        if fbytes not in (1, 2, 4):
            raise ToolError(f"{path}:$.fieldIdentifierBytes: expected 1, 2, or 4")
        if field_bytes is None:
            field_bytes = fbytes
        elif field_bytes != fbytes:
            raise ToolError(f"{path}: schema inventory Field identifier width disagrees with previous input")

        if "provenance" in root:
            provenance = require_object(root["provenance"], path, "$.provenance")
            reject_unknown(provenance, {"identity", "version"}, path, "$.provenance")
            require_keys(provenance, {"identity"}, path, "$.provenance")
            require_text(provenance["identity"], path, "$.provenance.identity", nonempty=True)
            if "version" in provenance:
                require_text(provenance["version"], path, "$.provenance.version", nonempty=True)

        types = require_object(root["types"], path, "$.types")
        max_field = width_max(fbytes)
        for type_key, raw_type in types.items():
            type_id = parse_type_identifier(type_key, path, f"$.types[{type_key!r}]")
            if type_id in merged:
                raise ToolError(f"{path}:$.types[{type_key!r}]: duplicate TypeIdentifier across inventories")
            tobj = require_object(raw_type, path, f"$.types[{type_key!r}]")
            t_allowed = {"status", "presentationExposed", "symbol", "fields"}
            require_keys(tobj, {"status", "presentationExposed", "fields"}, path, f"$.types[{type_key!r}]")
            reject_unknown(tobj, t_allowed, path, f"$.types[{type_key!r}]")
            status = require_text(tobj["status"], path, f"$.types[{type_key!r}].status", nonempty=True)
            if status not in _STATUS_VALUES:
                raise ToolError(f"{path}: Type {type_key}: status must be active or retired")
            exposed = require_bool(tobj["presentationExposed"], path, f"$.types[{type_key!r}].presentationExposed")
            if status == "retired" and exposed:
                raise ToolError(f"{path}: retired Type {type_key} must not be presentation-exposed")
            symbol = tobj.get("symbol")
            if symbol is not None:
                symbol = require_text(symbol, path, f"$.types[{type_key!r}].symbol", nonempty=True)
            fields_obj = require_object(tobj["fields"], path, f"$.types[{type_key!r}].fields")
            fields: dict[int, SchemaField] = {}
            for field_key, raw_field in fields_obj.items():
                fid = parse_canonical_uint_key(field_key, max_field, path, f"Type {type_key}/Field {field_key}")
                if fid in fields:
                    raise ToolError(f"{path}: Type {type_key}: duplicate FieldIdentifier {fid}")
                fobj = require_object(raw_field, path, f"Type {type_key}/Field {fid}")
                f_allowed = {"status", "presentationExposed", "symbol"}
                require_keys(fobj, {"status", "presentationExposed"}, path, f"Type {type_key}/Field {fid}")
                reject_unknown(fobj, f_allowed, path, f"Type {type_key}/Field {fid}")
                fstatus = require_text(fobj["status"], path, f"Type {type_key}/Field {fid}/status", nonempty=True)
                if fstatus not in _STATUS_VALUES:
                    raise ToolError(f"{path}: Type {type_key}/Field {fid}: status must be active or retired")
                fexposed = require_bool(fobj["presentationExposed"], path, f"Type {type_key}/Field {fid}/presentationExposed")
                if fstatus == "retired" and fexposed:
                    raise ToolError(f"{path}: retired Type {type_key}/Field {fid} must not be presentation-exposed")
                if status == "retired" and fstatus == "active":
                    raise ToolError(f"{path}: retired Type {type_key} may not contain active Field {fid}")
                fsymbol = fobj.get("symbol")
                if fsymbol is not None:
                    fsymbol = require_text(fsymbol, path, f"Type {type_key}/Field {fid}/symbol", nonempty=True)
                fields[fid] = SchemaField(fid, fstatus, fexposed, fsymbol)
            merged[type_id] = SchemaType(type_id, status, exposed, symbol, fields)

    assert field_bytes is not None
    return SchemaUniverse(field_bytes, merged)


def _merge_type_presentations(
    base: dict[bytes, TypePresentation],
    incoming: dict[bytes, TypePresentation],
    source_label: str,
) -> dict[bytes, TypePresentation]:
    result: dict[bytes, TypePresentation] = {
        key: TypePresentation(value.name, value.description, dict(value.fields))
        for key, value in base.items()
    }
    for type_id, incoming_type in incoming.items():
        current = result.setdefault(type_id, TypePresentation())
        if incoming_type.name is not None:
            if current.name is not None:
                raise ToolError(f"duplicate Type name representation while merging {source_label}")
            current.name = incoming_type.name
        if incoming_type.description is not None:
            if current.description is not None:
                raise ToolError(f"duplicate Type description representation while merging {source_label}")
            current.description = incoming_type.description
        for field_id, incoming_field in incoming_type.fields.items():
            field_value = current.fields.setdefault(field_id, FieldPresentation())
            if incoming_field.name is not None:
                if field_value.name is not None:
                    raise ToolError(f"duplicate Field name representation while merging {source_label}")
                field_value.name = incoming_field.name
            if incoming_field.description is not None:
                if field_value.description is not None:
                    raise ToolError(f"duplicate Field description representation while merging {source_label}")
                field_value.description = incoming_field.description
    return result


def _load_application_language(
    source_root: Path,
    language: str,
    manifest: Manifest,
    catalogue: dict[int, CatalogueDomain],
    schema: SchemaUniverse | None,
) -> LanguageSource:
    directory = source_root / language
    if not directory.is_dir():
        raise ToolError(f"{source_root}: missing supported-language directory {language}")
    language_path = directory / "language.json"
    strings_path = directory / "strings.json"
    types_path = directory / "type_schema.json"
    for path in (language_path, strings_path, types_path):
        if not path.is_file():
            raise ToolError(f"missing mandatory per-language source file: {path}")

    root = require_object(load_json(language_path), language_path, "$")
    allowed = {"schemaVersion", "language", "parent", "selfName", "displayNames"}
    require_keys(root, allowed, language_path, "$")
    reject_unknown(root, allowed, language_path, "$")
    if require_int(root["schemaVersion"], language_path, "$.schemaVersion") != AUTHORING_SCHEMA_VERSION:
        raise ToolError(f"{language_path}:$.schemaVersion: unsupported schema version")
    parsed_language = canonical_bcp47(root["language"], language_path, "$.language")
    if parsed_language != language:
        raise ToolError(f"{language_path}:$.language: must exactly match containing directory {language!r}")
    raw_parent = root["parent"]
    parent: str | None
    if raw_parent is None:
        parent = None
    else:
        parent = canonical_bcp47(raw_parent, language_path, "$.parent")
        if parent == language:
            raise ToolError(f"{language_path}:$.parent: self-parent is invalid")
        if parent not in manifest.supported_languages:
            raise ToolError(f"{language_path}:$.parent: parent {parent!r} is not supported")
    self_name = require_text(root["selfName"], language_path, "$.selfName", nonempty=True)
    display_obj = require_object(root["displayNames"], language_path, "$.displayNames")
    display_names: dict[str, str] = {}
    for target_raw, value_raw in display_obj.items():
        target = canonical_bcp47(target_raw, language_path, f"$.displayNames[{target_raw!r}]")
        if target == language:
            raise ToolError(f"{language_path}:$.displayNames: own-language key must be represented only by selfName")
        if target not in manifest.supported_languages:
            raise ToolError(f"{language_path}:$.displayNames: target {target!r} is not supported")
        display_names[target] = require_text(value_raw, language_path, f"$.displayNames[{target!r}]")
    display_names[language] = self_name

    strings = _parse_strings_json(
        strings_path,
        catalogue=catalogue,
        manifest=manifest,
        allowed_domain=lambda value: value >= 1,
    )
    types = _parse_type_schema_json(types_path, schema)
    return LanguageSource(language, parent, self_name, display_names, strings, types)


def _load_platform_language(
    bundle_root: Path,
    language: str,
    manifest: Manifest,
    catalogue: dict[int, CatalogueDomain],
    schema: SchemaUniverse | None,
) -> tuple[dict[tuple[int, int, int], str], dict[bytes, TypePresentation]]:
    directory = bundle_root / language
    strings_path = directory / "strings.json"
    type_path = directory / "type_schema.json"
    strings: dict[tuple[int, int, int], str] = {}
    types: dict[bytes, TypePresentation] = {}
    if strings_path.is_file():
        strings = _parse_strings_json(
            strings_path,
            catalogue=catalogue,
            manifest=manifest,
            allowed_domain=lambda value: value == 0,
        )
    if type_path.is_file():
        types = _parse_type_schema_json(type_path, schema)
    return strings, types
