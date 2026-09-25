from __future__ import annotations

from pathlib import Path
from typing import Iterable

from .catalogue import parse_platform_manifest
from .common import ToolError, canonical_bcp47, canonical_bundle_digest, write_json


def _normalise_languages(
    terminal_language: str,
    languages: Iterable[str],
    source: Path,
) -> tuple[str, ...]:
    terminal = canonical_bcp47(
        terminal_language,
        source,
        "--terminal-language",
    )

    ordered: list[str] = [terminal]
    seen = {terminal}

    for index, raw in enumerate(languages):
        language = canonical_bcp47(
            raw,
            source,
            f"--language[{index}]",
        )

        if language in seen:
            continue

        seen.add(language)
        ordered.append(language)

    return tuple(ordered)


def _parse_parent_overrides(
    values: Iterable[str],
    supported_languages: tuple[str, ...],
    terminal_language: str,
    source: Path,
) -> dict[str, str]:
    supported = set(supported_languages)
    result: dict[str, str] = {}

    for index, raw in enumerate(values):
        if "=" not in raw:
            raise ToolError(
                f"{source}:--parent[{index}]: expected CHILD=PARENT"
            )

        child_raw, parent_raw = raw.split("=", 1)
        child = canonical_bcp47(
            child_raw,
            source,
            f"--parent[{index}].child",
        )
        parent = canonical_bcp47(
            parent_raw,
            source,
            f"--parent[{index}].parent",
        )

        if child == terminal_language:
            raise ToolError(
                f"{source}:--parent[{index}]: terminal language may not have a parent"
            )

        if child not in supported or parent not in supported:
            raise ToolError(
                f"{source}:--parent[{index}]: both CHILD and PARENT must be supported languages"
            )

        if child == parent:
            raise ToolError(
                f"{source}:--parent[{index}]: a language may not parent itself"
            )

        if child in result:
            raise ToolError(
                f"{source}:--parent[{index}]: duplicate parent override for {child}"
            )

        result[child] = parent

    return result


def _build_parent_graph(
    supported_languages: tuple[str, ...],
    terminal_language: str,
    overrides: dict[str, str],
    source: Path,
) -> dict[str, str | None]:
    graph: dict[str, str | None] = {
        language: (
            None
            if language == terminal_language
            else overrides.get(language, terminal_language)
        )
        for language in supported_languages
    }

    for language in supported_languages:
        visited: set[str] = set()
        current: str | None = language

        while current is not None:
            if current in visited:
                raise ToolError(
                    f"{source}: parent graph contains a cycle involving {current}"
                )

            visited.add(current)
            current = graph[current]

        if terminal_language not in visited:
            raise ToolError(
                f"{source}: parent chain for {language} does not reach terminal language {terminal_language}"
            )

    return graph


def initialise_source_tree(
    source_root: Path,
    platform_bundle: Path,
    terminal_language: str,
    languages: Iterable[str],
    parent_overrides: Iterable[str],
    *,
    domain_width: int = 1,
    subdomain_width: int = 1,
    string_width: int = 2,
    schema_inventory: Path | None = None,
    schema_identity: str = "Application.Types",
    schema_version: str = "1",
) -> dict[str, object]:
    source_root = source_root.resolve()
    platform_bundle = platform_bundle.resolve()
    command_source = source_root / "manifest.json"

    if source_root.exists():
        if not source_root.is_dir():
            raise ToolError(
                f"{source_root}: source destination exists and is not a directory"
            )

        if any(source_root.iterdir()):
            raise ToolError(
                f"{source_root}: refusing to overwrite a non-empty source directory"
            )

    if domain_width not in (1, 2, 4):
        raise ToolError("domain identifier width must be 1, 2, or 4 bytes")

    if subdomain_width not in (1, 2, 4):
        raise ToolError("subdomain identifier width must be 1, 2, or 4 bytes")

    if string_width not in (1, 2, 4):
        raise ToolError("string identifier width must be 1, 2, or 4 bytes")

    if not schema_identity:
        raise ToolError("schema identity must be non-empty")

    if not schema_version:
        raise ToolError("schema version must be non-empty")

    platform_manifest = parse_platform_manifest(platform_bundle)
    platform_integrity = canonical_bundle_digest(platform_bundle)

    supported_languages = _normalise_languages(
        terminal_language,
        languages,
        command_source,
    )
    terminal = supported_languages[0]

    overrides = _parse_parent_overrides(
        parent_overrides,
        supported_languages,
        terminal,
        command_source,
    )
    graph = _build_parent_graph(
        supported_languages,
        terminal,
        overrides,
        command_source,
    )

    if schema_inventory is not None:
        schema_inventory = schema_inventory.resolve()

        if schema_inventory.exists():
            raise ToolError(
                f"{schema_inventory}: refusing to overwrite existing schema inventory"
            )

    source_root.mkdir(parents=True, exist_ok=True)

    write_json(
        source_root / "manifest.json",
        {
            "schemaVersion": 1,
            "terminalLanguage": terminal,
            "supportedLanguages": list(supported_languages),
            "identifierWidths": {
                "domain": domain_width,
                "subDomain": subdomain_width,
                "string": string_width,
            },
            "platformLocalisation": {
                "identity": platform_manifest.identity,
                "version": platform_manifest.version,
                "integrity": f"sha256-{platform_integrity}",
            },
        },
    )

    write_json(
        source_root / "strings_catalogue.json",
        {
            "schemaVersion": 1,
            "domains": [
                {
                    "id": 1,
                    "symbol": "Application",
                    "status": "active",
                    "subDomains": [
                        {
                            "id": 0,
                            "symbol": "Root",
                            "status": "active",
                            "strings": [],
                        }
                    ],
                }
            ],
        },
    )

    for language in supported_languages:
        language_directory = source_root / language
        display_names = (
            {
                other: other
                for other in supported_languages
                if other != language
            }
            if language == terminal
            else {}
        )

        write_json(
            language_directory / "language.json",
            {
                "schemaVersion": 1,
                "language": language,
                "parent": graph[language],
                "selfName": language,
                "displayNames": display_names,
            },
        )

        write_json(
            language_directory / "strings.json",
            {
                "schemaVersion": 1,
                "domains": (
                    {
                        "1": {
                            "subDomains": {
                                "0": {
                                    "strings": {
                                        "0": "Application",
                                    }
                                }
                            }
                        }
                    }
                    if language == terminal
                    else {}
                ),
            },
        )

        write_json(
            language_directory / "type_schema.json",
            {
                "schemaVersion": 1,
                "types": {},
            },
        )

    if schema_inventory is not None:
        write_json(
            schema_inventory,
            {
                "schemaVersion": 1,
                "provenance": {
                    "identity": schema_identity,
                    "version": schema_version,
                },
                "types": {},
            },
        )

    return {
        "source": str(source_root),
        "terminalLanguage": terminal,
        "supportedLanguages": list(supported_languages),
        "schemaInventory": (
            str(schema_inventory)
            if schema_inventory is not None
            else None
        ),
        "platformLocalisation": {
            "identity": platform_manifest.identity,
            "version": platform_manifest.version,
            "integrity": f"sha256-{platform_integrity}",
        },
    }
