from __future__ import annotations

from pathlib import Path
from typing import Iterable

from .common import ToolError
from .catalogue import (
    parse_application_catalogue, parse_manifest, parse_platform_catalogue, validate_platform_pin,
)
from .model_types import *
from .presentation import (
    _load_application_language, _load_platform_language, _merge_type_presentations, parse_schema_inventories,
)


def _validate_catalogue_parent_state(catalogue: dict[int, CatalogueDomain]) -> None:
    for domain in catalogue.values():
        for sub in domain.subdomains.values():
            if domain.status == "retired" and sub.status == "active":
                raise ToolError(f"retired Domain {domain.identifier} may not contain active SubDomain {sub.identifier}")
            for item in sub.strings.values():
                if sub.status == "retired" and item.status == "active":
                    raise ToolError(
                        f"retired Domain {domain.identifier}/SubDomain {sub.identifier} may not contain active String {item.identifier}"
                    )


def _general_policy(catalogue: dict[int, CatalogueDomain], key: tuple[int, int, int]) -> str:
    did, sid, tid = key
    sub = catalogue[did].subdomains[sid]
    if tid < 3:
        return sub.metadata_policy.for_string_id(tid)
    item = sub.strings[tid]
    assert item.localisation is not None
    return item.localisation


def _validate_semantics(model: SemanticModel) -> None:
    manifest = model.manifest
    languages = model.languages

    if set(languages) != set(manifest.supported_languages):
        raise ToolError("loaded language set does not match manifest supportedLanguages")

    terminal = languages[manifest.terminal_language]
    if terminal.parent is not None:
        raise ToolError(f"terminal language {terminal.language} must declare parent:null")
    for lang, source in languages.items():
        if lang != manifest.terminal_language and source.parent is None:
            raise ToolError(f"non-terminal language {lang} must declare one supported parent")

    for language in manifest.supported_languages:
        current = language
        visited: set[str] = set()
        for _ in range(len(manifest.supported_languages) + 1):
            if current in visited:
                raise ToolError(f"fallback cycle detected from {language}: {current}")
            visited.add(current)
            source = languages[current]
            if source.parent is None:
                if current != manifest.terminal_language:
                    raise ToolError(f"fallback chain for {language} terminates before terminal language")
                break
            current = source.parent
        else:
            raise ToolError(f"fallback chain for {language} exceeds supported-language bound")

    for target in manifest.supported_languages:
        value = terminal.display_names.get(target)
        if value is None or value == "":
            raise ToolError(f"terminal language {terminal.language} must provide a non-empty display name for {target}")

    terminal_strings = terminal.strings
    for did, domain in sorted(model.catalogue.items()):
        if domain.status != "active":
            continue
        for sid, sub in sorted(domain.subdomains.items()):
            if sub.status != "active":
                continue
            if (did, sid, 0) not in terminal_strings:
                raise ToolError(f"terminal language is missing mandatory Name for Domain {did}/SubDomain {sid}")
            for tid, item in sorted(sub.strings.items()):
                if item.status == "active" and (did, sid, tid) not in terminal_strings:
                    raise ToolError(f"terminal language is missing String {did}/{sid}/{tid}")

    for language, source in languages.items():
        if language == manifest.terminal_language:
            continue
        for key in source.strings:
            if _general_policy(model.catalogue, key) == "canonical":
                raise ToolError(
                    f"non-terminal language {language} supplies canonical general-string representation {key}"
                )

    if model.schema is not None:
        terminal_types = terminal.types
        for type_id, schema_type in sorted(model.schema.types.items()):
            if schema_type.status != "active":
                continue
            presentation = terminal_types.get(type_id)
            if schema_type.presentation_exposed:
                if presentation is None or presentation.name is None:
                    raise ToolError(
                        f"terminal language is missing Name for presentation-exposed Type 0x{type_id.hex().upper()}"
                    )
            for field_id, schema_field in sorted(schema_type.fields.items()):
                if schema_field.status != "active" or not schema_field.presentation_exposed:
                    continue
                fp = presentation.fields.get(field_id) if presentation is not None else None
                if fp is None or fp.name is None:
                    raise ToolError(
                        f"terminal language is missing Name for Type 0x{type_id.hex().upper()}/Field {field_id}"
                    )


def load_semantic_model(
    source_root: Path,
    platform_bundle: Path,
    schema_inventory_inputs: Iterable[Path],
) -> SemanticModel:
    source_root = source_root.resolve()
    platform_bundle = platform_bundle.resolve()
    manifest = parse_manifest(source_root)
    schema = parse_schema_inventories(schema_inventory_inputs)
    platform_manifest = validate_platform_pin(platform_bundle, manifest)
    app_catalogue = parse_application_catalogue(source_root, manifest)
    platform_catalogue = parse_platform_catalogue(platform_bundle, manifest)
    if set(app_catalogue) & set(platform_catalogue):
        raise ToolError("Platform and Application catalogues overlap Domain ownership")
    catalogue = {**platform_catalogue, **app_catalogue}
    _validate_catalogue_parent_state(catalogue)

    for child in source_root.iterdir():
        if child.is_dir() and (child / "language.json").exists() and child.name not in manifest.supported_languages:
            raise ToolError(f"unexpected language source directory not declared in manifest: {child.name}")

    languages: dict[str, LanguageSource] = {}
    for language in manifest.supported_languages:
        app = _load_application_language(source_root, language, manifest, app_catalogue, schema)
        platform_strings, platform_types = _load_platform_language(
            platform_bundle,
            language,
            manifest,
            platform_catalogue,
            schema,
        )
        overlap = set(platform_strings) & set(app.strings)
        if overlap:
            raise ToolError(f"language {language}: Platform/Application string representation overlap: {sorted(overlap)!r}")
        merged_strings = {**platform_strings, **app.strings}
        merged_types = _merge_type_presentations(platform_types, app.types, f"language {language}")
        languages[language] = LanguageSource(
            language=app.language,
            parent=app.parent,
            self_name=app.self_name,
            display_names=app.display_names,
            strings=merged_strings,
            types=merged_types,
        )

    model = SemanticModel(manifest, catalogue, schema, languages, platform_manifest)
    _validate_semantics(model)
    return model
