# tools/edp_localisation/fingerprint.py

**Classification:** INTERNAL TOOLING

**Original reference baseline:** retained for historical symbols; new diagnostic additions reviewed at feature source commit `bd58bd419c69439a376251468473dd79babf2278`.

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

## Diagnostic-schema fingerprint (Tranche I, pending integration)

`build_diagnostic_fingerprint(model: SemanticModel) -> Fingerprint` uses the same immutable `Fingerprint` result class (16-byte runtime value, full SHA-256 digest and exact canonical stream) but a **distinct domain prefix**, version byte and entirely translation-independent content. The serialized schema transcript traverses Domain, SubDomain and ordinary String numeric identities in sorted order, lifecycle statuses, then each declared parameter index/status/canonical type. It excludes language/translation text and symbolic names. Sequence-width is not an input. This does **not** presently emit a runtime compiler descriptor or populated EDPL section.

`build_fingerprint` now appends a separate tagged optional parameter section only when one or more Strings declare parameters. As a result, old catalogues' ContractFamily canonical bytes remain identical; parameterized catalogues have different exact-family fingerprints when their semantic argument declarations change. That fact is independent of the diagnostic-schema digest.

**Validation:** focused unit tests are committed but NOT executed on RPI400/AI-AGENT-02 (both unavailable).
