# tools/tests/test_toolchain.py

**Classification:** INTERNAL TOOLING TEST

**Source baseline:** `83aa04323787a3b9ce48fa452dffdd960f1358af`

[Open exact source](https://github.com/ESPressio-Development-Platform/EDP-Localisation/blob/83aa04323787a3b9ce48fa452dffdd960f1358af/tools/tests/test_toolchain.py)

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

## `test_schema_inventory_rejects_configurable_type_identifier_width`

**Kind:** def · **Scope:** nested/class

Proves that authoring schema inventories cannot reintroduce a configurable Type identifier width.

```python
def test_schema_inventory_rejects_configurable_type_identifier_width(self) -> None:
```

## `test_type_identifier_requires_fixed_64_bit_width`

**Kind:** def · **Scope:** nested/class

Proves that schema Type keys must occupy the fixed 64-bit EDP identity width.

```python
def test_type_identifier_requires_fixed_64_bit_width(self) -> None:
```

## `test_type_identifier_rejects_zero_authority`

**Kind:** def · **Scope:** nested/class

Proves that the universal Type Authority component cannot be zero.

```python
def test_type_identifier_rejects_zero_authority(self) -> None:
```

## `test_type_identifier_rejects_zero_authority_local_value`

**Kind:** def · **Scope:** nested/class

Proves that the 40-bit authority-local Type component cannot be zero.

```python
def test_type_identifier_rejects_zero_authority_local_value(self) -> None:
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



## Fixed Field schema regression coverage

### `test_schema_inventory_rejects_configurable_field_identifier_width`

**Kind:** test · **Scope:** `ToolchainTests`

Proves that `fieldIdentifierBytes` is no longer an accepted schema-inventory property.

### `test_schema_inventory_rejects_field_identifier_above_fixed_range`

**Kind:** test · **Scope:** `ToolchainTests`

Proves that Type-local Field identifiers are restricted to the fixed one-byte range `0..255`.

The deterministic generation tests additionally assert `FieldIdentifierBytes = 1U` for schema-backed generated contracts, while no-schema generation continues to assert the explicit `0/0` sentinel.
