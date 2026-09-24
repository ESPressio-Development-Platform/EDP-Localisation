# tools/edp_localisation/model_types.py

**Classification:** INTERNAL TOOLING

**Source baseline:** `9a0ca6321eaaf9940bfb0d84556e438cca5b7e55`

[Open exact source](https://github.com/ESPressio-Development-Platform/EDP-Localisation/blob/9a0ca6321eaaf9940bfb0d84556e438cca5b7e55/tools/edp_localisation/model_types.py)

## `PlatformPin`

**Kind:** class · **Scope:** module

Purpose is defined by the implementation and call sites; no declaration docstring is attached.

```python
class PlatformPin:
```

## `Manifest`

**Kind:** class · **Scope:** module

Purpose is defined by the implementation and call sites; no declaration docstring is attached.

```python
class Manifest:
```

## `MetadataPolicy`

**Kind:** class · **Scope:** module

Purpose is defined by the implementation and call sites; no declaration docstring is attached.

```python
class MetadataPolicy:
```

## `for_string_id`

**Kind:** def · **Scope:** nested/class

Purpose is defined by the implementation and call sites; no declaration docstring is attached.

```python
def for_string_id(self, string_id: int) -> str:
```

## `CatalogueString`

**Kind:** class · **Scope:** module

Purpose is defined by the implementation and call sites; no declaration docstring is attached.

```python
class CatalogueString:
```

## `CatalogueSubDomain`

**Kind:** class · **Scope:** module

Purpose is defined by the implementation and call sites; no declaration docstring is attached.

```python
class CatalogueSubDomain:
```

## `CatalogueDomain`

**Kind:** class · **Scope:** module

Purpose is defined by the implementation and call sites; no declaration docstring is attached.

```python
class CatalogueDomain:
```

## `SchemaField`

**Kind:** class · **Scope:** module

Purpose is defined by the implementation and call sites; no declaration docstring is attached.

```python
class SchemaField:
```

## `SchemaType`

**Kind:** class · **Scope:** module

Purpose is defined by the implementation and call sites; no declaration docstring is attached.

```python
class SchemaType:
```

## `SchemaUniverse`

**Kind:** class · **Scope:** module

Represents the merged authoritative schema universe. It stores the configurable Type-local Field identifier width and the Type declarations. Type identifier width is intentionally absent from this model because all EDP Type identities are fixed at 64 bits.

```python
class SchemaUniverse:
    field_identifier_bytes: int
    types: dict[bytes, SchemaType]
```

## `FieldPresentation`

**Kind:** class · **Scope:** module

Purpose is defined by the implementation and call sites; no declaration docstring is attached.

```python
class FieldPresentation:
```

## `TypePresentation`

**Kind:** class · **Scope:** module

Purpose is defined by the implementation and call sites; no declaration docstring is attached.

```python
class TypePresentation:
```

## `LanguageSource`

**Kind:** class · **Scope:** module

Purpose is defined by the implementation and call sites; no declaration docstring is attached.

```python
class LanguageSource:
```

## `PlatformManifest`

**Kind:** class · **Scope:** module

Purpose is defined by the implementation and call sites; no declaration docstring is attached.

```python
class PlatformManifest:
```

## `SemanticModel`

**Kind:** class · **Scope:** module

Purpose is defined by the implementation and call sites; no declaration docstring is attached.

```python
class SemanticModel:
```

