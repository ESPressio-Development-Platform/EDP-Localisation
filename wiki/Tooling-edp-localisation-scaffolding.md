# tools/edp_localisation/scaffolding.py

**Classification:** INTERNAL TOOLING

**Source baseline:** `83aa04323787a3b9ce48fa452dffdd960f1358af`

[Open exact source](https://github.com/ESPressio-Development-Platform/EDP-Localisation/blob/83aa04323787a3b9ce48fa452dffdd960f1358af/tools/edp_localisation/scaffolding.py)

## `_normalise_languages`

**Kind:** def · **Scope:** module

Purpose is defined by the implementation and call sites; no declaration docstring is attached.

```python
def _normalise_languages(
terminal_language: str,
languages: Iterable[str],
source: Path,
) -> tuple[str, ...]:
```

## `_parse_parent_overrides`

**Kind:** def · **Scope:** module

Purpose is defined by the implementation and call sites; no declaration docstring is attached.

```python
def _parse_parent_overrides(
values: Iterable[str],
supported_languages: tuple[str, ...],
terminal_language: str,
source: Path,
) -> dict[str, str]:
```

## `_build_parent_graph`

**Kind:** def · **Scope:** module

Purpose is defined by the implementation and call sites; no declaration docstring is attached.

```python
def _build_parent_graph(
supported_languages: tuple[str, ...],
terminal_language: str,
overrides: dict[str, str],
source: Path,
) -> dict[str, str | None]:
```

## `initialise_source_tree`

**Kind:** def · **Scope:** module

Purpose is defined by the implementation and call sites; no declaration docstring is attached.

```python
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
```


## Fixed Type identity width

`initialise_source_tree` accepts no Type-width or Field-width parameter. Newly scaffolded schema inventories omit both `typeIdentifierBytes` and `fieldIdentifierBytes`; Type identity is the fixed 64-bit EDP contract and Field identity is the fixed one-byte Type-local EDP contract.
