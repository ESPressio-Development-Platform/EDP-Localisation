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

