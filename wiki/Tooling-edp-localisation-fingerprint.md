# tools/edp_localisation/fingerprint.py

**Classification:** INTERNAL TOOLING

**Source baseline:** `9a0ca6321eaaf9940bfb0d84556e438cca5b7e55`

[Open exact source](https://github.com/ESPressio-Development-Platform/EDP-Localisation/blob/9a0ca6321eaaf9940bfb0d84556e438cca5b7e55/tools/edp_localisation/fingerprint.py)

## `Fingerprint`

**Kind:** class · **Scope:** module

Purpose is defined by the implementation and call sites; no declaration docstring is attached.

```python
class Fingerprint:
```

## `runtime_hex`

**Kind:** def · **Scope:** nested/class

Purpose is defined by the implementation and call sites; no declaration docstring is attached.

```python
def runtime_hex(self) -> str:
```

## `digest_hex`

**Kind:** def · **Scope:** nested/class

Purpose is defined by the implementation and call sites; no declaration docstring is attached.

```python
def digest_hex(self) -> str:
```

## `_Stream`

**Kind:** class · **Scope:** module

Purpose is defined by the implementation and call sites; no declaration docstring is attached.

```python
class _Stream:
```

## `__init__`

**Kind:** def · **Scope:** nested/class

Purpose is defined by the implementation and call sites; no declaration docstring is attached.

```python
def __init__(self) -> None:
```

## `tag`

**Kind:** def · **Scope:** nested/class

Purpose is defined by the implementation and call sites; no declaration docstring is attached.

```python
def tag(self, value: int) -> None:
```

## `u8`

**Kind:** def · **Scope:** nested/class

Purpose is defined by the implementation and call sites; no declaration docstring is attached.

```python
def u8(self, value: int) -> None:
```

## `u32`

**Kind:** def · **Scope:** nested/class

Purpose is defined by the implementation and call sites; no declaration docstring is attached.

```python
def u32(self, value: int) -> None:
```

## `raw`

**Kind:** def · **Scope:** nested/class

Purpose is defined by the implementation and call sites; no declaration docstring is attached.

```python
def raw(self, value: bytes) -> None:
```

## `bytes`

**Kind:** def · **Scope:** nested/class

Purpose is defined by the implementation and call sites; no declaration docstring is attached.

```python
def bytes(self, value: bytes) -> None:
```

## `text`

**Kind:** def · **Scope:** nested/class

Purpose is defined by the implementation and call sites; no declaration docstring is attached.

```python
def text(self, value: str) -> None:
```

## `optional_text`

**Kind:** def · **Scope:** nested/class

Purpose is defined by the implementation and call sites; no declaration docstring is attached.

```python
def optional_text(self, value: str | None) -> None:
```

## `_status_byte`

**Kind:** def · **Scope:** module

Purpose is defined by the implementation and call sites; no declaration docstring is attached.

```python
def _status_byte(status: str) -> int:
```

## `_policy_byte`

**Kind:** def · **Scope:** module

Purpose is defined by the implementation and call sites; no declaration docstring is attached.

```python
def _policy_byte(policy: str | None) -> int:
```

## `build_fingerprint`

**Kind:** def · **Scope:** module

Purpose is defined by the implementation and call sites; no declaration docstring is attached.

```python
def build_fingerprint(model: SemanticModel) -> Fingerprint:
```


## Type identity contribution

The canonical semantic fingerprint records Type width as the fixed value `8` whenever a Type/Field schema universe exists and `0` only when it is absent. Type width is therefore no longer an author-controlled semantic input.
