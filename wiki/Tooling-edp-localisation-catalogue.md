# tools/edp_localisation/catalogue.py

**Classification:** INTERNAL TOOLING

**Source baseline:** `9d6a6814097c3eb99792d84b134d7d2dcb059c6f`

[Open exact source](https://github.com/ESPressio-Development-Platform/EDP-Localisation/blob/9d6a6814097c3eb99792d84b134d7d2dcb059c6f/tools/edp_localisation/catalogue.py)

## `_parse_metadata_policy`

**Kind:** def · **Scope:** module

Purpose is defined by the implementation and call sites; no module docstring is attached to this declaration.

```python
def _parse_metadata_policy(value: Any, source: Path, location: str) -> MetadataPolicy:
```

## `parse_manifest`

**Kind:** def · **Scope:** module

Purpose is defined by the implementation and call sites; no module docstring is attached to this declaration.

```python
def parse_manifest(source_root: Path) -> Manifest:
```

## `_parse_catalogue`

**Kind:** def · **Scope:** module

Purpose is defined by the implementation and call sites; no module docstring is attached to this declaration.

```python
def _parse_catalogue(
path: Path,
*,
domain_bytes: int,
subdomain_bytes: int,
string_bytes: int,
allowed_domain: callable,
authority_name: str,
) -> dict[int, CatalogueDomain]:
```

## `parse_application_catalogue`

**Kind:** def · **Scope:** module

Purpose is defined by the implementation and call sites; no module docstring is attached to this declaration.

```python
def parse_application_catalogue(source_root: Path, manifest: Manifest) -> dict[int, CatalogueDomain]:
```

## `parse_platform_manifest`

**Kind:** def · **Scope:** module

Purpose is defined by the implementation and call sites; no module docstring is attached to this declaration.

```python
def parse_platform_manifest(bundle_root: Path) -> PlatformManifest:
```

## `validate_platform_pin`

**Kind:** def · **Scope:** module

Purpose is defined by the implementation and call sites; no module docstring is attached to this declaration.

```python
def validate_platform_pin(bundle_root: Path, manifest: Manifest) -> PlatformManifest:
```

## `parse_platform_catalogue`

**Kind:** def · **Scope:** module

Purpose is defined by the implementation and call sites; no module docstring is attached to this declaration.

```python
def parse_platform_catalogue(bundle_root: Path, manifest: Manifest) -> dict[int, CatalogueDomain]:
```

