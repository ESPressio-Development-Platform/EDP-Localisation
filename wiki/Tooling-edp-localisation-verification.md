# tools/edp_localisation/verification.py

**Classification:** INTERNAL TOOLING

**Source baseline:** `42425a3a33051fdf148408792a21a647ddeac753`

[Open exact source](https://github.com/ESPressio-Development-Platform/EDP-Localisation/blob/42425a3a33051fdf148408792a21a647ddeac753/tools/edp_localisation/verification.py)

## `_validate_manifest_shape`

**Kind:** def · **Scope:** module

Purpose is defined by the implementation and call sites; no declaration docstring is attached.

```python
def _validate_manifest_shape(path: Path, root: dict) -> None:
```

## `load_build_manifest`

**Kind:** def · **Scope:** module

Purpose is defined by the implementation and call sites; no declaration docstring is attached.

```python
def load_build_manifest(generated_root: Path) -> dict:
```

## `verify_internal_generated_set`

**Kind:** def · **Scope:** module

Purpose is defined by the implementation and call sites; no declaration docstring is attached.

```python
def verify_internal_generated_set(generated_root: Path) -> dict:
```

## `_tree_bytes`

**Kind:** def · **Scope:** module

Purpose is defined by the implementation and call sites; no declaration docstring is attached.

```python
def _tree_bytes(root: Path) -> dict[str, bytes]:
```

## `verify_generated_set`

**Kind:** def · **Scope:** module

Purpose is defined by the implementation and call sites; no declaration docstring is attached.

```python
def verify_generated_set(
source_root: Path,
platform_bundle: Path,
schema_inventories: Iterable[Path],
generated_root: Path,
cpp_namespace: str,
) -> None:
```


## Diagnostic generated-set validation

`_validate_manifest_shape` verifies 128-bit uppercase diagnostic identity and full SHA-256 digest agree. `load_build_manifest` requires the canonical `diagnostic-schema.json` entry with kind `diagnostic-schema`; `verify_internal_generated_set` checks its SHA-256 and embedded schema version/kind and digest against the manifest. Byte-for-byte complete regeneration remains in `verify_generated_set`.

This does not authenticate the source, prove C++ compiler behaviour, or validate historical semantic compatibility across multiple software builds; those require future tests and deployment provenance. All tests remain not executed on unavailable RPI400/AI-AGENT-02.
