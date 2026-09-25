# src/localisation/detail/EdplFormat.hpp

**Primary classification:** PRIVATE IMPLEMENTATION

**Source baseline:** `9a0ca6321eaaf9940bfb0d84556e438cca5b7e55`

[Open exact source](https://github.com/ESPressio-Development-Platform/EDP-Localisation/blob/9a0ca6321eaaf9940bfb0d84556e438cca5b7e55/src/localisation/detail/EdplFormat.hpp)

## Direct includes

- `cstddef`
- `cstdint`

## Documented declarations

### `SectionType`

**Classification:** PRIVATE IMPLEMENTATION

Current persisted section identifiers for the release-owned EDPL format.

```cpp
enum class SectionType : std::uint8_t
```

### `FixedPreambleBytes`

**Classification:** PRIVATE IMPLEMENTATION

Number of fixed bytes preceding the section directory.

```cpp
inline constexpr std::size_t FixedPreambleBytes = 22U;
```

### `SectionDirectoryEntryBytes`

**Classification:** PRIVATE IMPLEMENTATION

Number of bytes in one section-directory entry.

```cpp
inline constexpr std::size_t SectionDirectoryEntryBytes = 12U;
```

### `FileCrc32cOffset`

**Classification:** PRIVATE IMPLEMENTATION

Offset of the persisted whole-file CRC32C field inside the fixed preamble.

```cpp
inline constexpr std::size_t FileCrc32cOffset = 18U;
```

### `FileCrc32cBytes`

**Classification:** PRIVATE IMPLEMENTATION

Number of bytes occupied by the persisted whole-file CRC32C field.

```cpp
inline constexpr std::size_t FileCrc32cBytes = 4U;
```

### `RequiredSectionCount`

**Classification:** PRIVATE IMPLEMENTATION

Number of required current-format sections.

```cpp
inline constexpr std::size_t RequiredSectionCount = 5U;
```

### `LanguageMetadataFixedBytes`

**Classification:** PRIVATE IMPLEMENTATION

Fixed Language Metadata bytes preceding language and parent tag payloads.

```cpp
inline constexpr std::size_t LanguageMetadataFixedBytes = 20U;
```

### `LanguageDisplayHeaderBytes`

**Classification:** PRIVATE IMPLEMENTATION

Fixed Language Display-Name section header size.

```cpp
inline constexpr std::size_t LanguageDisplayHeaderBytes = 4U;
```

### `LanguageDisplayEntryBytes`

**Classification:** PRIVATE IMPLEMENTATION

Fixed Language Display-Name record size.

```cpp
inline constexpr std::size_t LanguageDisplayEntryBytes = 16U;
```

### `GeneralStringsHeaderBytes`

**Classification:** PRIVATE IMPLEMENTATION

Fixed General Strings section header size.

```cpp
inline constexpr std::size_t GeneralStringsHeaderBytes = 28U;
```

### `TypeSchemaHeaderBytes`

**Classification:** PRIVATE IMPLEMENTATION

Fixed Type Schema section header size.

```cpp
inline constexpr std::size_t TypeSchemaHeaderBytes = 20U;
```

### `PayloadHeaderBytes`

**Classification:** PRIVATE IMPLEMENTATION

Fixed UTF-8 Payload section header size.

```cpp
inline constexpr std::size_t PayloadHeaderBytes = 8U;
```

### `SectionVersion`

**Classification:** PRIVATE IMPLEMENTATION

Current section schema version used by every V1 section.

```cpp
inline constexpr std::uint8_t SectionVersion = 1U;
```

### `LanguageTerminalFlag`

**Classification:** PRIVATE IMPLEMENTATION

Language Metadata flag selecting the unique terminal language.

```cpp
inline constexpr std::uint8_t LanguageTerminalFlag = 0x01U;
```

### `NamePresentFlag`

**Classification:** PRIVATE IMPLEMENTATION

Type/Field representation flag indicating a present Name.

```cpp
inline constexpr std::uint8_t NamePresentFlag = 0x01U;
```

### `DescriptionPresentFlag`

**Classification:** PRIVATE IMPLEMENTATION

Type/Field representation flag indicating a present Description.

```cpp
inline constexpr std::uint8_t DescriptionPresentFlag = 0x02U;
```


## Type Schema identity width

Although the V1 Type Schema section retains a one-byte Type-width field as structural self-description, the semantic contract is fixed: `8` means the platform-wide 64-bit EDP Type identity and `0` is reserved for the no-Type/Field-universe sentinel. No other Type width is valid.


## Fixed Field width contract

Although the Type Schema header retains a one-byte persisted `FieldIdentifierBytes` field for structural self-description, the semantic contract is fixed:

- `1` for every non-empty Type/Field universe;
- `0` only together with `TypeIdentifierBytes == 0` for the explicit no-schema sentinel.

No two- or four-byte Field identity form is valid in the current format.
