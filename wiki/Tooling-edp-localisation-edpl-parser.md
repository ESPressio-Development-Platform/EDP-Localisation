# tools/edp_localisation/edpl_parser.py

**Classification:** INTERNAL TOOLING

**Source baseline:** `9d6a6814097c3eb99792d84b134d7d2dcb059c6f`

[Open exact source](https://github.com/ESPressio-Development-Platform/EDP-Localisation/blob/9d6a6814097c3eb99792d84b134d7d2dcb059c6f/tools/edp_localisation/edpl_parser.py)

## `Section`

**Kind:** class · **Scope:** module

Purpose is defined by the implementation and call sites; no module docstring is attached to this declaration.

```python
class Section:
```

## `ParsedType`

**Kind:** class · **Scope:** module

Purpose is defined by the implementation and call sites; no module docstring is attached to this declaration.

```python
class ParsedType:
```

## `Pack`

**Kind:** class · **Scope:** module

Purpose is defined by the implementation and call sites; no module docstring is attached to this declaration.

```python
class Pack:
```

## `__init__`

**Kind:** def · **Scope:** class/internal scope

Purpose is defined by the implementation and call sites; no module docstring is attached to this declaration.

```python
def __init__(self, data: bytes, *, validate_crc: bool = True) -> None:
```

## `from_path`

**Kind:** def · **Scope:** class/internal scope

Purpose is defined by the implementation and call sites; no module docstring is attached to this declaration.

```python
def from_path(cls, path: Path, *, validate_crc: bool = True) -> "Pack":
```

## `_section_bytes`

**Kind:** def · **Scope:** class/internal scope

Purpose is defined by the implementation and call sites; no module docstring is attached to this declaration.

```python
def _section_bytes(self, kind: int) -> bytes:
```

## `_parse`

**Kind:** def · **Scope:** class/internal scope

Purpose is defined by the implementation and call sites; no module docstring is attached to this declaration.

```python
def _parse(self, *, validate_crc: bool) -> None:
```

## `_parse_language_metadata`

**Kind:** def · **Scope:** class/internal scope

Purpose is defined by the implementation and call sites; no module docstring is attached to this declaration.

```python
def _parse_language_metadata(self) -> None:
```

## `_parse_payload`

**Kind:** def · **Scope:** class/internal scope

Purpose is defined by the implementation and call sites; no module docstring is attached to this declaration.

```python
def _parse_payload(self) -> None:
```

## `_text`

**Kind:** def · **Scope:** class/internal scope

Purpose is defined by the implementation and call sites; no module docstring is attached to this declaration.

```python
def _text(self, offset: int, length: int) -> str:
```

## `_parse_display_names`

**Kind:** def · **Scope:** class/internal scope

Purpose is defined by the implementation and call sites; no module docstring is attached to this declaration.

```python
def _parse_display_names(self) -> None:
```

## `_parse_general_strings`

**Kind:** def · **Scope:** class/internal scope

Purpose is defined by the implementation and call sites; no module docstring is attached to this declaration.

```python
def _parse_general_strings(self) -> None:
```

## `_parse_type_schema`

**Kind:** def · **Scope:** class/internal scope

Purpose is defined by the implementation and call sites; no module docstring is attached to this declaration.

```python
def _parse_type_schema(self) -> None:
```

## `recover_json`

**Kind:** def · **Scope:** class/internal scope

Purpose is defined by the implementation and call sites; no module docstring is attached to this declaration.

```python
def recover_json(self) -> dict:
```

