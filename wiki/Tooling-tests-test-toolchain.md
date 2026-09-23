# tools/tests/test_toolchain.py

**Classification:** INTERNAL TOOLING TEST

**Source baseline:** `42425a3a33051fdf148408792a21a647ddeac753`

[Open exact source](https://github.com/ESPressio-Development-Platform/EDP-Localisation/blob/42425a3a33051fdf148408792a21a647ddeac753/tools/tests/test_toolchain.py)

## `write_json`

**Kind:** def · **Scope:** module

Purpose is defined by the implementation and call sites; no declaration docstring is attached.

```python
def write_json(path: Path, value: object) -> None:
```

## `ToolchainTests`

**Kind:** class · **Scope:** module

Purpose is defined by the implementation and call sites; no declaration docstring is attached.

```python
class ToolchainTests(unittest.TestCase):
```

## `make_fixture`

**Kind:** def · **Scope:** nested/class

Purpose is defined by the implementation and call sites; no declaration docstring is attached.

```python
def make_fixture(self, root: Path) -> tuple[Path, Path, Path]:
```

## `test_init_creates_compileable_three_language_skeleton`

**Kind:** def · **Scope:** nested/class

Purpose is defined by the implementation and call sites; no declaration docstring is attached.

```python
def test_init_creates_compileable_three_language_skeleton(self) -> None:
```

## `test_compile_is_deterministic_and_resolution_matches_fallback_semantics`

**Kind:** def · **Scope:** nested/class

Purpose is defined by the implementation and call sites; no declaration docstring is attached.

```python
def test_compile_is_deterministic_and_resolution_matches_fallback_semantics(self) -> None:
```

## `test_verify_generated_detects_stale_output`

**Kind:** def · **Scope:** nested/class

Purpose is defined by the implementation and call sites; no declaration docstring is attached.

```python
def test_verify_generated_detects_stale_output(self) -> None:
```

## `test_active_domain_requires_root_namespace`

**Kind:** def · **Scope:** nested/class

Purpose is defined by the implementation and call sites; no declaration docstring is attached.

```python
def test_active_domain_requires_root_namespace(self) -> None:
```

## `test_domain_root_metadata_policy_has_single_authority`

**Kind:** def · **Scope:** nested/class

Purpose is defined by the implementation and call sites; no declaration docstring is attached.

```python
def test_domain_root_metadata_policy_has_single_authority(self) -> None:
```

## `test_platform_integrity_mismatch_is_rejected`

**Kind:** def · **Scope:** nested/class

Purpose is defined by the implementation and call sites; no declaration docstring is attached.

```python
def test_platform_integrity_mismatch_is_rejected(self) -> None:
```

## `test_generated_corruption_is_rejected`

**Kind:** def · **Scope:** nested/class

Purpose is defined by the implementation and call sites; no declaration docstring is attached.

```python
def test_generated_corruption_is_rejected(self) -> None:
```

## `test_duplicate_type_across_schema_inventories_is_rejected`

**Kind:** def · **Scope:** nested/class

Purpose is defined by the implementation and call sites; no declaration docstring is attached.

```python
def test_duplicate_type_across_schema_inventories_is_rejected(self) -> None:
```

## `test_cpp_namespace_reserved_word_is_rejected`

**Kind:** def · **Scope:** nested/class

Purpose is defined by the implementation and call sites; no declaration docstring is attached.

```python
def test_cpp_namespace_reserved_word_is_rejected(self) -> None:
```

## `test_generated_identifier_collision_is_rejected`

**Kind:** def · **Scope:** nested/class

Purpose is defined by the implementation and call sites; no declaration docstring is attached.

```python
def test_generated_identifier_collision_is_rejected(self) -> None:
```

## `test_noncanonical_language_in_build_manifest_is_rejected`

**Kind:** def · **Scope:** nested/class

Purpose is defined by the implementation and call sites; no declaration docstring is attached.

```python
def test_noncanonical_language_in_build_manifest_is_rejected(self) -> None:
```

## `test_contract_family_without_schema_inventory_compiles`

**Kind:** def · **Scope:** nested/class

Purpose is defined by the implementation and call sites; no declaration docstring is attached.

```python
def test_contract_family_without_schema_inventory_compiles(self) -> None:
```

## `test_duplicate_json_object_key_is_rejected`

**Kind:** def · **Scope:** nested/class

Purpose is defined by the implementation and call sites; no declaration docstring is attached.

```python
def test_duplicate_json_object_key_is_rejected(self) -> None:
```

## `test_terminal_general_string_completeness_is_enforced`

**Kind:** def · **Scope:** nested/class

Purpose is defined by the implementation and call sites; no declaration docstring is attached.

```python
def test_terminal_general_string_completeness_is_enforced(self) -> None:
```

## `test_fallback_cycle_is_rejected`

**Kind:** def · **Scope:** nested/class

Purpose is defined by the implementation and call sites; no declaration docstring is attached.

```python
def test_fallback_cycle_is_rejected(self) -> None:
```

## `test_non_terminal_canonical_override_is_rejected`

**Kind:** def · **Scope:** nested/class

Purpose is defined by the implementation and call sites; no declaration docstring is attached.

```python
def test_non_terminal_canonical_override_is_rejected(self) -> None:
```

