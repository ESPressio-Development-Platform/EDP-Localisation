# tools/edp_localisation/common.py

**Classification:** INTERNAL TOOLING

**Source baseline:** `9d6a6814097c3eb99792d84b134d7d2dcb059c6f`

[Open exact source](https://github.com/ESPressio-Development-Platform/EDP-Localisation/blob/9d6a6814097c3eb99792d84b134d7d2dcb059c6f/tools/edp_localisation/common.py)

## `TOOLCHAIN_IDENTITY`

**Kind:** constant · **Scope:** module

Module-level constant or predefinition used by the localisation toolchain.

```python
TOOLCHAIN_IDENTITY = "EDP-Localisation"
```

## `TOOLCHAIN_VERSION`

**Kind:** constant · **Scope:** module

Module-level constant or predefinition used by the localisation toolchain.

```python
TOOLCHAIN_VERSION = "0.1.0"
```

## `AUTHORING_SCHEMA_VERSION`

**Kind:** constant · **Scope:** module

Module-level constant or predefinition used by the localisation toolchain.

```python
AUTHORING_SCHEMA_VERSION = 1
```

## `BUILD_MANIFEST_SCHEMA_VERSION`

**Kind:** constant · **Scope:** module

Module-level constant or predefinition used by the localisation toolchain.

```python
BUILD_MANIFEST_SCHEMA_VERSION = 1
```

## `SCHEMA_INVENTORY_VERSION`

**Kind:** constant · **Scope:** module

Module-level constant or predefinition used by the localisation toolchain.

```python
SCHEMA_INVENTORY_VERSION = 1
```

## `PLATFORM_BUNDLE_SCHEMA_VERSION`

**Kind:** constant · **Scope:** module

Module-level constant or predefinition used by the localisation toolchain.

```python
PLATFORM_BUNDLE_SCHEMA_VERSION = 1
```

## `FINGERPRINT_CANONICALISATION_VERSION`

**Kind:** constant · **Scope:** module

Module-level constant or predefinition used by the localisation toolchain.

```python
FINGERPRINT_CANONICALISATION_VERSION = 1
```

## `EDPL_FORMAT_MAJOR`

**Kind:** constant · **Scope:** module

Module-level constant or predefinition used by the localisation toolchain.

```python
EDPL_FORMAT_MAJOR = 1
```

## `EDPL_FORMAT_MINOR`

**Kind:** constant · **Scope:** module

Module-level constant or predefinition used by the localisation toolchain.

```python
EDPL_FORMAT_MINOR = 0
```

## `ToolError`

**Kind:** class · **Scope:** module

One deterministic user-facing tooling failure.

```python
class ToolError(Exception):
```

## `DuplicateKeyError`

**Kind:** class · **Scope:** module

Purpose is defined by the implementation and call sites; no module docstring is attached to this declaration.

```python
class DuplicateKeyError(ToolError):
```

## `Diagnostic`

**Kind:** class · **Scope:** module

Purpose is defined by the implementation and call sites; no module docstring is attached to this declaration.

```python
class Diagnostic:
```

## `render`

**Kind:** def · **Scope:** class/internal scope

Purpose is defined by the implementation and call sites; no module docstring is attached to this declaration.

```python
def render(self) -> str:
```

## `Diagnostics`

**Kind:** class · **Scope:** module

Purpose is defined by the implementation and call sites; no module docstring is attached to this declaration.

```python
class Diagnostics:
```

## `__init__`

**Kind:** def · **Scope:** class/internal scope

Purpose is defined by the implementation and call sites; no module docstring is attached to this declaration.

```python
def __init__(self) -> None:
```

## `add`

**Kind:** def · **Scope:** class/internal scope

Purpose is defined by the implementation and call sites; no module docstring is attached to this declaration.

```python
def add(self, source: Path | str, location: str, reason: str) -> None:
```

## `extend`

**Kind:** def · **Scope:** class/internal scope

Purpose is defined by the implementation and call sites; no module docstring is attached to this declaration.

```python
def extend(self, items: Iterable[Diagnostic]) -> None:
```

## `items`

**Kind:** def · **Scope:** class/internal scope

Purpose is defined by the implementation and call sites; no module docstring is attached to this declaration.

```python
def items(self) -> list[Diagnostic]:
```

## `raise_if_any`

**Kind:** def · **Scope:** class/internal scope

Purpose is defined by the implementation and call sites; no module docstring is attached to this declaration.

```python
def raise_if_any(self) -> None:
```

## `_object_pairs_no_duplicates`

**Kind:** def · **Scope:** module

Purpose is defined by the implementation and call sites; no module docstring is attached to this declaration.

```python
def _object_pairs_no_duplicates(pairs: list[tuple[str, Any]]) -> dict[str, Any]:
```

## `load_json`

**Kind:** def · **Scope:** module

Purpose is defined by the implementation and call sites; no module docstring is attached to this declaration.

```python
def load_json(path: Path) -> Any:
```

## `require_object`

**Kind:** def · **Scope:** module

Purpose is defined by the implementation and call sites; no module docstring is attached to this declaration.

```python
def require_object(value: Any, source: Path, location: str) -> dict[str, Any]:
```

## `require_array`

**Kind:** def · **Scope:** module

Purpose is defined by the implementation and call sites; no module docstring is attached to this declaration.

```python
def require_array(value: Any, source: Path, location: str) -> list[Any]:
```

## `reject_unknown`

**Kind:** def · **Scope:** module

Purpose is defined by the implementation and call sites; no module docstring is attached to this declaration.

```python
def reject_unknown(obj: dict[str, Any], allowed: set[str], source: Path, location: str) -> None:
```

## `require_keys`

**Kind:** def · **Scope:** module

Purpose is defined by the implementation and call sites; no module docstring is attached to this declaration.

```python
def require_keys(obj: dict[str, Any], required: set[str], source: Path, location: str) -> None:
```

## `require_int`

**Kind:** def · **Scope:** module

Purpose is defined by the implementation and call sites; no module docstring is attached to this declaration.

```python
def require_int(value: Any, source: Path, location: str) -> int:
```

## `require_bool`

**Kind:** def · **Scope:** module

Purpose is defined by the implementation and call sites; no module docstring is attached to this declaration.

```python
def require_bool(value: Any, source: Path, location: str) -> bool:
```

## `require_text`

**Kind:** def · **Scope:** module

Purpose is defined by the implementation and call sites; no module docstring is attached to this declaration.

```python
def require_text(value: Any, source: Path, location: str, *, nonempty: bool = False) -> str:
```

## `parse_canonical_uint_key`

**Kind:** def · **Scope:** module

Purpose is defined by the implementation and call sites; no module docstring is attached to this declaration.

```python
def parse_canonical_uint_key(text: str, max_value: int, source: Path, location: str) -> int:
```

## `width_max`

**Kind:** def · **Scope:** module

Purpose is defined by the implementation and call sites; no module docstring is attached to this declaration.

```python
def width_max(byte_width: int) -> int:
```

## `parse_type_identifier`

**Kind:** def · **Scope:** module

Purpose is defined by the implementation and call sites; no module docstring is attached to this declaration.

```python
def parse_type_identifier(text: str, byte_width: int, source: Path, location: str) -> bytes:
```

## `format_type_identifier`

**Kind:** def · **Scope:** module

Purpose is defined by the implementation and call sites; no module docstring is attached to this declaration.

```python
def format_type_identifier(value: bytes) -> str:
```

## `validate_cpp_identifier`

**Kind:** def · **Scope:** module

Purpose is defined by the implementation and call sites; no module docstring is attached to this declaration.

```python
def validate_cpp_identifier(identifier: str, description: str) -> str:
```

## `validate_cpp_namespace`

**Kind:** def · **Scope:** module

Purpose is defined by the implementation and call sites; no module docstring is attached to this declaration.

```python
def validate_cpp_namespace(namespace: str) -> tuple[str, ...]:
```

## `parse_sha256_pin`

**Kind:** def · **Scope:** module

Purpose is defined by the implementation and call sites; no module docstring is attached to this declaration.

```python
def parse_sha256_pin(value: Any, source: Path, location: str) -> str:
```

## `sha256_hex`

**Kind:** def · **Scope:** module

Purpose is defined by the implementation and call sites; no module docstring is attached to this declaration.

```python
def sha256_hex(data: bytes) -> str:
```

## `canonical_json_bytes`

**Kind:** def · **Scope:** module

Purpose is defined by the implementation and call sites; no module docstring is attached to this declaration.

```python
def canonical_json_bytes(value: Any) -> bytes:
```

## `write_json`

**Kind:** def · **Scope:** module

Purpose is defined by the implementation and call sites; no module docstring is attached to this declaration.

```python
def write_json(path: Path, value: Any) -> None:
```

## `canonical_bundle_digest`

**Kind:** def · **Scope:** module

Hash exact bundle file bytes in canonical relative-path order.

```python
def canonical_bundle_digest(root: Path) -> str:
```

## `canonical_bcp47`

**Kind:** def · **Scope:** module

Purpose is defined by the implementation and call sites; no module docstring is attached to this declaration.

```python
def canonical_bcp47(tag: Any, source: Path, location: str) -> str:
```

