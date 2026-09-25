# tools/edp_localisation/edpl_parser.py

**Classification:** INTERNAL TOOLING

**Source baseline:** `83aa04323787a3b9ce48fa452dffdd960f1358af`

[Open exact source](https://github.com/ESPressio-Development-Platform/EDP-Localisation/blob/83aa04323787a3b9ce48fa452dffdd960f1358af/tools/edp_localisation/edpl_parser.py)

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


## Type Schema width validation

For a non-empty Type/Field universe the parser requires the persisted Type Schema header to advertise exactly `TypeIdentifierBytes == 8` and `FieldIdentifierBytes == 1`. The `0/0` width pair is accepted only for the explicit empty-schema sentinel. Arbitrary Type- or Field-width negotiation is not supported.
