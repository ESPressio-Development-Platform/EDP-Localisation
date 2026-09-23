# tools/edp_localisation/generated.py

**Classification:** INTERNAL TOOLING

**Source baseline:** `42425a3a33051fdf148408792a21a647ddeac753`

[Open exact source](https://github.com/ESPressio-Development-Platform/EDP-Localisation/blob/42425a3a33051fdf148408792a21a647ddeac753/tools/edp_localisation/generated.py)

## `Resolution`

**Kind:** class · **Scope:** module

Purpose is defined by the implementation and call sites; no declaration docstring is attached.

```python
class Resolution:
```

## `as_json`

**Kind:** def · **Scope:** nested/class

Purpose is defined by the implementation and call sites; no declaration docstring is attached.

```python
def as_json(self) -> dict:
```

## `GeneratedContractFamily`

**Kind:** class · **Scope:** module

Purpose is defined by the implementation and call sites; no declaration docstring is attached.

```python
class GeneratedContractFamily:
```

## `__init__`

**Kind:** def · **Scope:** nested/class

Purpose is defined by the implementation and call sites; no declaration docstring is attached.

```python
def __init__(self, root: Path, *, verify: bool = True) -> None:
```

## `pack`

**Kind:** def · **Scope:** nested/class

Purpose is defined by the implementation and call sites; no declaration docstring is attached.

```python
def pack(self, language: str) -> Pack:
```

## `_terminal_pack`

**Kind:** def · **Scope:** nested/class

Purpose is defined by the implementation and call sites; no declaration docstring is attached.

```python
def _terminal_pack(self) -> Pack:
```

## `resolve`

**Kind:** def · **Scope:** nested/class

Purpose is defined by the implementation and call sites; no declaration docstring is attached.

```python
def resolve(self, requested: str, lookup: Callable[[Pack], object]) -> Resolution:
```

## `resolve_string`

**Kind:** def · **Scope:** nested/class

Purpose is defined by the implementation and call sites; no declaration docstring is attached.

```python
def resolve_string(self, requested: str, domain: int, subdomain: int, string: int) -> Resolution:
```

## `resolve_language_name`

**Kind:** def · **Scope:** nested/class

Purpose is defined by the implementation and call sites; no declaration docstring is attached.

```python
def resolve_language_name(self, requested: str, target: str) -> Resolution:
```

## `_type_id`

**Kind:** def · **Scope:** nested/class

Purpose is defined by the implementation and call sites; no declaration docstring is attached.

```python
def _type_id(self, text: str) -> bytes:
```

## `resolve_type_property`

**Kind:** def · **Scope:** nested/class

Purpose is defined by the implementation and call sites; no declaration docstring is attached.

```python
def resolve_type_property(self, requested: str, type_text: str, property_name: str) -> Resolution:
```

## `lookup`

**Kind:** def · **Scope:** nested/class

Purpose is defined by the implementation and call sites; no declaration docstring is attached.

```python
def lookup(pack: Pack) -> object:
```

## `resolve_field_property`

**Kind:** def · **Scope:** nested/class

Purpose is defined by the implementation and call sites; no declaration docstring is attached.

```python
def resolve_field_property(
self,
requested: str,
type_text: str,
field: int,
property_name: str,
) -> Resolution:
```

## `lookup`

**Kind:** def · **Scope:** nested/class

Purpose is defined by the implementation and call sites; no declaration docstring is attached.

```python
def lookup(pack: Pack) -> object:
```

## `decompile_generated`

**Kind:** def · **Scope:** nested/class

Purpose is defined by the implementation and call sites; no declaration docstring is attached.

```python
def decompile_generated(self) -> dict:
```

