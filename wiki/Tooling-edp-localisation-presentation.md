# tools/edp_localisation/presentation.py

**Classification:** INTERNAL TOOLING

**Source baseline:** `83aa04323787a3b9ce48fa452dffdd960f1358af`

[Open exact source](https://github.com/ESPressio-Development-Platform/EDP-Localisation/blob/83aa04323787a3b9ce48fa452dffdd960f1358af/tools/edp_localisation/presentation.py)

## `_parse_strings_json`

**Kind:** def · **Scope:** module

Purpose is defined by the implementation and call sites; no declaration docstring is attached.

```python
def _parse_strings_json(
path: Path,
*,
catalogue: dict[int, CatalogueDomain],
manifest: Manifest,
allowed_domain: callable,
) -> dict[tuple[int, int, int], str]:
```

## `_parse_type_schema_json`

**Kind:** def · **Scope:** module

Purpose is defined by the implementation and call sites; no declaration docstring is attached.

```python
def _parse_type_schema_json(
path: Path,
schema: SchemaUniverse | None,
) -> dict[bytes, TypePresentation]:
```

## `_inventory_files`

**Kind:** def · **Scope:** module

Purpose is defined by the implementation and call sites; no declaration docstring is attached.

```python
def _inventory_files(inputs: Iterable[Path]) -> list[Path]:
```

## `parse_schema_inventories`

**Kind:** def · **Scope:** module

Purpose is defined by the implementation and call sites; no declaration docstring is attached.

```python
def parse_schema_inventories(inputs: Iterable[Path]) -> SchemaUniverse | None:
```

## `_merge_type_presentations`

**Kind:** def · **Scope:** module

Purpose is defined by the implementation and call sites; no declaration docstring is attached.

```python
def _merge_type_presentations(
base: dict[bytes, TypePresentation],
incoming: dict[bytes, TypePresentation],
source_label: str,
) -> dict[bytes, TypePresentation]:
```

## `_load_application_language`

**Kind:** def · **Scope:** module

Purpose is defined by the implementation and call sites; no declaration docstring is attached.

```python
def _load_application_language(
source_root: Path,
language: str,
manifest: Manifest,
catalogue: dict[int, CatalogueDomain],
schema: SchemaUniverse | None,
) -> LanguageSource:
```

## `_load_platform_language`

**Kind:** def · **Scope:** module

Purpose is defined by the implementation and call sites; no declaration docstring is attached.

```python
def _load_platform_language(
bundle_root: Path,
language: str,
manifest: Manifest,
catalogue: dict[int, CatalogueDomain],
schema: SchemaUniverse | None,
) -> tuple[dict[tuple[int, int, int], str], dict[bytes, TypePresentation]]:
```


## Schema inventory Type identity rules

`parse_schema_inventories` accepts neither `typeIdentifierBytes` nor `fieldIdentifierBytes`. Type keys are canonical fixed-width 64-bit identities (`0x` plus exactly 16 uppercase hexadecimal digits); Field keys are canonical decimal integers in `0..255`. Multiple inventories therefore have no width-negotiation state to reconcile, and duplicate Type identities remain errors.
