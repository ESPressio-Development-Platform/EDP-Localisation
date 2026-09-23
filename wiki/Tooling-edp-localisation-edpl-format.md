# tools/edp_localisation/edpl_format.py

**Classification:** INTERNAL TOOLING

**Source baseline:** `9d6a6814097c3eb99792d84b134d7d2dcb059c6f`

[Open exact source](https://github.com/ESPressio-Development-Platform/EDP-Localisation/blob/9d6a6814097c3eb99792d84b134d7d2dcb059c6f/tools/edp_localisation/edpl_format.py)

## `SECTION_LANGUAGE_METADATA`

**Kind:** constant · **Scope:** module

Module-level constant or predefinition used by the localisation toolchain.

```python
SECTION_LANGUAGE_METADATA = 1
```

## `SECTION_LANGUAGE_DISPLAY_NAMES`

**Kind:** constant · **Scope:** module

Module-level constant or predefinition used by the localisation toolchain.

```python
SECTION_LANGUAGE_DISPLAY_NAMES = 2
```

## `SECTION_GENERAL_STRINGS`

**Kind:** constant · **Scope:** module

Module-level constant or predefinition used by the localisation toolchain.

```python
SECTION_GENERAL_STRINGS = 3
```

## `SECTION_TYPE_SCHEMA`

**Kind:** constant · **Scope:** module

Module-level constant or predefinition used by the localisation toolchain.

```python
SECTION_TYPE_SCHEMA = 4
```

## `SECTION_UTF8_PAYLOAD`

**Kind:** constant · **Scope:** module

Module-level constant or predefinition used by the localisation toolchain.

```python
SECTION_UTF8_PAYLOAD = 5
```

## `REQUIRED_SECTIONS`

**Kind:** constant · **Scope:** module

Module-level constant or predefinition used by the localisation toolchain.

```python
REQUIRED_SECTIONS = (
```

## `PREAMBLE_SIZE`

**Kind:** constant · **Scope:** module

Module-level constant or predefinition used by the localisation toolchain.

```python
PREAMBLE_SIZE = 22
```

## `DIRECTORY_ENTRY_SIZE`

**Kind:** constant · **Scope:** module

Module-level constant or predefinition used by the localisation toolchain.

```python
DIRECTORY_ENTRY_SIZE = 12
```

## `HEADER_SIZE`

**Kind:** constant · **Scope:** module

Module-level constant or predefinition used by the localisation toolchain.

```python
HEADER_SIZE = PREAMBLE_SIZE + len(REQUIRED_SECTIONS) * DIRECTORY_ENTRY_SIZE
```

## `SECTION_VERSION`

**Kind:** constant · **Scope:** module

Module-level constant or predefinition used by the localisation toolchain.

```python
SECTION_VERSION = 1
```

## `TERMINAL_FLAG`

**Kind:** constant · **Scope:** module

Module-level constant or predefinition used by the localisation toolchain.

```python
TERMINAL_FLAG = 0x01
```

## `NAME_PRESENT`

**Kind:** constant · **Scope:** module

Module-level constant or predefinition used by the localisation toolchain.

```python
NAME_PRESENT = 0x01
```

## `DESCRIPTION_PRESENT`

**Kind:** constant · **Scope:** module

Module-level constant or predefinition used by the localisation toolchain.

```python
DESCRIPTION_PRESENT = 0x02
```

## `CRC_OFFSET`

**Kind:** constant · **Scope:** module

Module-level constant or predefinition used by the localisation toolchain.

```python
CRC_OFFSET = 18
```

## `CRC_SIZE`

**Kind:** constant · **Scope:** module

Module-level constant or predefinition used by the localisation toolchain.

```python
CRC_SIZE = 4
```

## `crc32c`

**Kind:** def · **Scope:** module

Purpose is defined by the implementation and call sites; no module docstring is attached to this declaration.

```python
def crc32c(data: bytes, zero_range: tuple[int, int] | None = None) -> int:
```

