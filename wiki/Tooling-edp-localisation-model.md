# tools/edp_localisation/model.py

**Classification:** INTERNAL TOOLING

**Source baseline:** `42425a3a33051fdf148408792a21a647ddeac753`

[Open exact source](https://github.com/ESPressio-Development-Platform/EDP-Localisation/blob/42425a3a33051fdf148408792a21a647ddeac753/tools/edp_localisation/model.py)

## `_validate_catalogue_parent_state`

**Kind:** def · **Scope:** module

Purpose is defined by the implementation and call sites; no declaration docstring is attached.

```python
def _validate_catalogue_parent_state(catalogue: dict[int, CatalogueDomain]) -> None:
```

## `_general_policy`

**Kind:** def · **Scope:** module

Purpose is defined by the implementation and call sites; no declaration docstring is attached.

```python
def _general_policy(catalogue: dict[int, CatalogueDomain], key: tuple[int, int, int]) -> str:
```

## `_validate_semantics`

**Kind:** def · **Scope:** module

Purpose is defined by the implementation and call sites; no declaration docstring is attached.

```python
def _validate_semantics(model: SemanticModel) -> None:
```

## `load_semantic_model`

**Kind:** def · **Scope:** module

Purpose is defined by the implementation and call sites; no declaration docstring is attached.

```python
def load_semantic_model(
source_root: Path,
platform_bundle: Path,
schema_inventory_inputs: Iterable[Path],
) -> SemanticModel:
```


## Diagnostic template validation (source commit 278ef8fd)

`_validate_diagnostic_placeholders(model: SemanticModel)` validates the full language collection's present representations **only** for catalogue Strings having nonempty governed `parameters`. The scanner is strictly bounded by each existing representation string length, accepts numeric `{N}` and escaped braces `{{` / `}}`, and rejects leading-zero, Unicode digit, undeclared/out-of-range and malformed tokens. It is invoked once by `_validate_semantics` before compilation.

A valid template may repeat or omit a parameter and may reorder occurrences. Parameter type coercion and rendering remain separate future capabilities. Ordinary Strings have no change in brace semantics. Long numeric indices are rejected before integer conversion to avoid interpreter overflow/limits. This is internal Python authoring validation, not a C++ runtime formatter.

## Zero-argument diagnostic placeholder checks

`_validate_diagnostic_placeholders` now runs for **explicitly declared** diagnostic Strings, even when the `parameters` array is empty. This rejects `{0}` on a zero-argument signature while leaving ordinary strings with no declaration and their literal braces unmodified. The metadata comes from `CatalogueString.diagnostic_declared`.
