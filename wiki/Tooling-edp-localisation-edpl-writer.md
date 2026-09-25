# tools/edp_localisation/edpl_writer.py

**Classification:** INTERNAL TOOLING

**Source baseline:** `83aa04323787a3b9ce48fa452dffdd960f1358af`

[Open exact source](https://github.com/ESPressio-Development-Platform/EDP-Localisation/blob/83aa04323787a3b9ce48fa452dffdd960f1358af/tools/edp_localisation/edpl_writer.py)

## `PayloadPool`

**Kind:** class · **Scope:** module

Purpose is defined by the implementation and call sites; no module docstring is attached to this declaration.

```python
class PayloadPool:
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
def add(self, text: str | bytes) -> tuple[int, int]:
```

## `data`

**Kind:** def · **Scope:** class/internal scope

Purpose is defined by the implementation and call sites; no module docstring is attached to this declaration.

```python
def data(self) -> bytes:
```

## `_u`

**Kind:** def · **Scope:** module

Purpose is defined by the implementation and call sites; no module docstring is attached to this declaration.

```python
def _u(value: int, width: int) -> bytes:
```

## `_language_metadata`

**Kind:** def · **Scope:** module

Purpose is defined by the implementation and call sites; no module docstring is attached to this declaration.

```python
def _language_metadata(model: SemanticModel, language: str, fingerprint: Fingerprint) -> bytes:
```

## `_language_display_names`

**Kind:** def · **Scope:** module

Purpose is defined by the implementation and call sites; no module docstring is attached to this declaration.

```python
def _language_display_names(model: SemanticModel, language: str, payload: PayloadPool) -> bytes:
```

## `_general_strings`

**Kind:** def · **Scope:** module

Purpose is defined by the implementation and call sites; no module docstring is attached to this declaration.

```python
def _general_strings(model: SemanticModel, language: str, payload: PayloadPool) -> bytes:
```

## `_presentation_pair`

**Kind:** def · **Scope:** module

Purpose is defined by the implementation and call sites; no module docstring is attached to this declaration.

```python
def _presentation_pair(value: str | None, pool: PayloadPool) -> tuple[int, int, bool]:
```

## `_type_schema`

**Kind:** def · **Scope:** module

Purpose is defined by the implementation and call sites; no module docstring is attached to this declaration.

```python
def _type_schema(model: SemanticModel, language: str, payload: PayloadPool) -> bytes:
```

## `_payload_section`

**Kind:** def · **Scope:** module

Purpose is defined by the implementation and call sites; no module docstring is attached to this declaration.

```python
def _payload_section(payload: PayloadPool) -> bytes:
```

## `build_language_pack`

**Kind:** def · **Scope:** module

Purpose is defined by the implementation and call sites; no module docstring is attached to this declaration.

```python
def build_language_pack(model: SemanticModel, language: str, fingerprint: Fingerprint) -> bytes:
```


## Type Schema width emission

The writer always emits `TypeIdentifierBytes = 8` and `FieldIdentifierBytes = 1` when a schema universe exists. Field records therefore use exactly one byte for the Type-local identity. The `0/0` pair is emitted only when no Type/Field universe exists; neither width is sourced from schema authoring.
