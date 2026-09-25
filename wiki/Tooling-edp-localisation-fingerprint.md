# tools/edp_localisation/fingerprint.py

**Classification:** INTERNAL TOOLING

**Source baseline:** `83aa04323787a3b9ce48fa452dffdd960f1358af`

[Open exact source](https://github.com/ESPressio-Development-Platform/EDP-Localisation/blob/83aa04323787a3b9ce48fa452dffdd960f1358af/tools/edp_localisation/fingerprint.py)

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


## Type/Field identity contribution

The canonical semantic fingerprint records fixed widths `8/1` whenever a Type/Field schema universe exists and `0/0` only when it is absent. Neither width is an author-controlled semantic input; the bytes remain in the canonical stream so the platform contract is explicit and deterministic.
