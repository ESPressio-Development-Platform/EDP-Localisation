# tools/edp_localisation/model_types.py

**Classification:** INTERNAL TOOLING

**Original reference baseline:** retained for historical symbols; new diagnostic additions reviewed at feature source commit `bd58bd419c69439a376251468473dd79babf2278`.

[Open exact source](https://github.com/ESPressio-Development-Platform/EDP-Localisation/blob/83aa04323787a3b9ce48fa452dffdd960f1358af/tools/edp_localisation/model_types.py)

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

Represents the merged authoritative schema universe. It stores only the merged Type declarations; neither Type nor Field identifier width is retained as authoring state because both widths are fixed platform contracts (8 bytes and 1 byte respectively).

```python
class SchemaUniverse:
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


## DiagnosticParameter and CatalogueString.parameters

`DiagnosticParameter` is an **INTERNAL TOOLING** frozen model value comprising governed zero-based `index`, `type_name`, `status` and optional authoring `symbol`. The symbol is not diagnostic-schema identity; position and canonical type are. Its lifecycle status allows retaining reserved historical parameter metadata on retired Strings without treating those as valid active call signatures.

`CatalogueString.parameters` is an immutable tuple of these declarations, default empty. This preserves existing code constructing CatalogueString with its previous positional arguments. Neither model value retains host-language translation content in the type identity. Source ownership and validation remain in catalogue.py; diagnostic fingerprint traversal remains in fingerprint.py.
