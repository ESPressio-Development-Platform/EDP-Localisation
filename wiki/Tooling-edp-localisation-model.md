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

