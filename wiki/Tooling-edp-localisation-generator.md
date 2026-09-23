# tools/edp_localisation/generator.py

**Classification:** INTERNAL TOOLING

**Source baseline:** `42425a3a33051fdf148408792a21a647ddeac753`

[Open exact source](https://github.com/ESPressio-Development-Platform/EDP-Localisation/blob/42425a3a33051fdf148408792a21a647ddeac753/tools/edp_localisation/generator.py)

## `GeneratedSet`

**Kind:** class · **Scope:** module

Purpose is defined by the implementation and call sites; no declaration docstring is attached.

```python
class GeneratedSet:
```

## `VerifiedOutput`

**Kind:** class · **Scope:** module

Purpose is defined by the implementation and call sites; no declaration docstring is attached.

```python
class VerifiedOutput:
```

## `_cpp_identifier`

**Kind:** def · **Scope:** module

Purpose is defined by the implementation and call sites; no declaration docstring is attached.

```python
def _cpp_identifier(value: str | None, description: str) -> str:
```

## `_cpp_namespace_open`

**Kind:** def · **Scope:** module

Purpose is defined by the implementation and call sites; no declaration docstring is attached.

```python
def _cpp_namespace_open(parts: tuple[str, ...]) -> str:
```

## `_cpp_namespace_close`

**Kind:** def · **Scope:** module

Purpose is defined by the implementation and call sites; no declaration docstring is attached.

```python
def _cpp_namespace_close(parts: tuple[str, ...]) -> str:
```

## `_cpp_indent`

**Kind:** def · **Scope:** module

Purpose is defined by the implementation and call sites; no declaration docstring is attached.

```python
def _cpp_indent(level: int, text: str) -> str:
```

## `_generate_contract_header`

**Kind:** def · **Scope:** module

Purpose is defined by the implementation and call sites; no declaration docstring is attached.

```python
def _generate_contract_header(model: SemanticModel, fingerprint: Fingerprint, cpp_namespace: str) -> bytes:
```

## `_metadata_identifier_name`

**Kind:** def · **Scope:** module

Purpose is defined by the implementation and call sites; no declaration docstring is attached.

```python
def _metadata_identifier_name(string_id: int) -> str:
```

## `_active_application_domains`

**Kind:** def · **Scope:** module

Purpose is defined by the implementation and call sites; no declaration docstring is attached.

```python
def _active_application_domains(model: SemanticModel) -> list[CatalogueDomain]:
```

## `_generate_identifiers_header`

**Kind:** def · **Scope:** module

Purpose is defined by the implementation and call sites; no declaration docstring is attached.

```python
def _generate_identifiers_header(model: SemanticModel, cpp_namespace: str) -> bytes:
```

## `_manifest_output`

**Kind:** def · **Scope:** module

Purpose is defined by the implementation and call sites; no declaration docstring is attached.

```python
def _manifest_output(path: str, kind: str, data: bytes, *, language: str | None = None) -> dict:
```

## `_build_outputs`

**Kind:** def · **Scope:** module

Purpose is defined by the implementation and call sites; no declaration docstring is attached.

```python
def _build_outputs(model: SemanticModel, fingerprint: Fingerprint, cpp_namespace: str) -> dict[str, bytes]:
```

## `_build_manifest`

**Kind:** def · **Scope:** module

Purpose is defined by the implementation and call sites; no declaration docstring is attached.

```python
def _build_manifest(model: SemanticModel, fingerprint: Fingerprint, outputs: dict[str, bytes]) -> dict:
```

## `generate_to_directory`

**Kind:** def · **Scope:** module

Purpose is defined by the implementation and call sites; no declaration docstring is attached.

```python
def generate_to_directory(
source_root: Path,
platform_bundle: Path,
schema_inventories: Iterable[Path],
output_root: Path,
cpp_namespace: str,
) -> GeneratedSet:
```

## `_remove_path`

**Kind:** def · **Scope:** module

Purpose is defined by the implementation and call sites; no declaration docstring is attached.

```python
def _remove_path(path: Path) -> None:
```

## `compile_generated_set`

**Kind:** def · **Scope:** module

Validate and publish one complete generated set with rollback on publication failure.

```python
def compile_generated_set(
source_root: Path,
platform_bundle: Path,
schema_inventories: Iterable[Path],
output_root: Path,
cpp_namespace: str,
) -> GeneratedSet:
```

