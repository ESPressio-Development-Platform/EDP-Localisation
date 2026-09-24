# src/localisation/detail/EdplReader.hpp

**Primary classification:** PRIVATE IMPLEMENTATION

**Source baseline:** `ba0643c5b34c28feef30cd6f82a50c0e0a996681`

[Open exact source](https://github.com/ESPressio-Development-Platform/EDP-Localisation/blob/ba0643c5b34c28feef30cd6f82a50c0e0a996681/src/localisation/detail/EdplReader.hpp)

## Direct includes

- `ESPressio_Memory.hpp`
- `array`
- `cstddef`
- `cstdint`
- `limits`
- `optional`
- `../LanguageIdentifierView.hpp`
- `../LocalisationTypes.hpp`
- `../PackSourceContract.hpp`
- `EdplFormat.hpp`

## Documented declarations

### `RepresentationState`

**Classification:** PRIVATE IMPLEMENTATION

Authored presence state of one textual representation.

```cpp
enum class RepresentationState : std::uint8_t
```

### `PresentationRepresentation`

**Classification:** PRIVATE IMPLEMENTATION

Selects the Name or Description presentation surface for Type/Field lookup.

```cpp
enum class PresentationRepresentation : std::uint8_t
```

### `SectionDescriptor`

**Classification:** PRIVATE IMPLEMENTATION

Absolute persisted bounds of one EDPL section.

```cpp
struct SectionDescriptor final
```

### `Offset`

**Classification:** PRIVATE IMPLEMENTATION · source access: `public`

Absolute first byte of the section.

```cpp
std::uint32_t Offset;
```

### `Length`

**Classification:** PRIVATE IMPLEMENTATION · source access: `public`

Complete persisted section byte count.

```cpp
std::uint32_t Length;
```

### `IsPresent`

**Classification:** PRIVATE IMPLEMENTATION · source access: `public`

Indicates whether the required section was found.

```cpp
bool IsPresent;
```

### `PackLayout`

**Classification:** PRIVATE IMPLEMENTATION

Complete current-format EDPL section layout observed from one pack.

```cpp
struct PackLayout final
```

### `FileSizeBytes`

**Classification:** PRIVATE IMPLEMENTATION · source access: `public`

Freshly observed complete resource size.

```cpp
std::uint64_t FileSizeBytes;
```

### `HeaderSizeBytes`

**Classification:** PRIVATE IMPLEMENTATION · source access: `public`

Persisted complete header size.

```cpp
std::uint16_t HeaderSizeBytes;
```

### `FileCrc32c`

**Classification:** PRIVATE IMPLEMENTATION · source access: `public`

Persisted whole-file CRC32C.

```cpp
std::uint32_t FileCrc32c;
```

### `LanguageMetadata`

**Classification:** PRIVATE IMPLEMENTATION · source access: `public`

Language Metadata section.

```cpp
SectionDescriptor LanguageMetadata;
```

### `LanguageDisplayNames`

**Classification:** PRIVATE IMPLEMENTATION · source access: `public`

Language Display-Name section.

```cpp
SectionDescriptor LanguageDisplayNames;
```

### `GeneralStrings`

**Classification:** PRIVATE IMPLEMENTATION · source access: `public`

General Strings section.

```cpp
SectionDescriptor GeneralStrings;
```

### `TypeSchema`

**Classification:** PRIVATE IMPLEMENTATION · source access: `public`

Type Schema section.

```cpp
SectionDescriptor TypeSchema;
```

### `Utf8Payload`

**Classification:** PRIVATE IMPLEMENTATION · source access: `public`

UTF-8 Payload section.

```cpp
SectionDescriptor Utf8Payload;
```

### `RepresentationLocation`

**Classification:** PRIVATE IMPLEMENTATION

Location and authored presence state of one representation payload.

```cpp
struct RepresentationLocation final
```

### `State`

**Classification:** PRIVATE IMPLEMENTATION · source access: `public`

Authored representation state.

```cpp
RepresentationState State;
```

### `PayloadOffset`

**Classification:** PRIVATE IMPLEMENTATION · source access: `public`

Byte offset relative to the first UTF-8 payload byte.

```cpp
std::uint32_t PayloadOffset;
```

### `PayloadLength`

**Classification:** PRIVATE IMPLEMENTATION · source access: `public`

Complete representation byte count.

```cpp
std::uint32_t PayloadLength;
```

### `IsEntityPresent`

**Classification:** PRIVATE IMPLEMENTATION · source access: `public`

Indicates whether the machine entity owning this representation exists.

```cpp
bool IsEntityPresent;
```

### `RepresentationLookupResult`

**Classification:** PRIVATE IMPLEMENTATION

Typed result of locating one representation without copying payload bytes.

```cpp
struct RepresentationLookupResult final
```

### `Status`

**Classification:** PRIVATE IMPLEMENTATION · source access: `public`

Lookup/format/provider outcome.

```cpp
LocalisationStatus Status;
```

### `Representation`

**Classification:** PRIVATE IMPLEMENTATION · source access: `public`

Representation location present only when Status is Success.

```cpp
std::optional<RepresentationLocation> Representation;
```

### `PayloadDescriptor`

**Classification:** PRIVATE IMPLEMENTATION

Absolute persisted bounds of the UTF-8 payload section.

```cpp
struct PayloadDescriptor final
```

### `PayloadBytesOffset`

**Classification:** PRIVATE IMPLEMENTATION · source access: `public`

Absolute offset of the first UTF-8 payload byte.

```cpp
std::uint64_t PayloadBytesOffset;
```

### `PayloadLength`

**Classification:** PRIVATE IMPLEMENTATION · source access: `public`

Complete UTF-8 payload byte count.

```cpp
std::uint32_t PayloadLength;
```

### `TPackSource`

**Classification:** PRIVATE IMPLEMENTATION

Current-release EDPL reader over one selected Pack Source.

- **Template parameter `TPackSource`:** Concrete Pack Source provider.
- **Template parameter `TByteOperationsProvider`:** Concrete EDP-Memory ByteOperations provider.
- **Template parameter `TContract`:** Generated Localisation ContractFamily descriptor.

```cpp
template<
        class TPackSource,
```

### `PackResource`

**Classification:** PRIVATE IMPLEMENTATION · source access: `private`

Provider-associated immutable pack resource type.

```cpp
using PackResource = typename TPackSource::PackResource;
```

### `Identifiers`

**Classification:** PRIVATE IMPLEMENTATION · source access: `private`

Contract-derived identifier vocabulary used while parsing this ContractFamily.

```cpp
using Identifiers = ContractIdentifiers<TContract>;
```

### `LanguageMetadataValue`

**Classification:** PRIVATE IMPLEMENTATION · source access: `private`

Owned language metadata extracted from one validated EDPL pack.

```cpp
struct LanguageMetadataValue final
```

### `IsTerminal`

**Classification:** PRIVATE IMPLEMENTATION · source access: `public`

Indicates whether this pack is the unique terminal language.

```cpp
bool IsTerminal;
```

### `LanguageLength`

**Classification:** PRIVATE IMPLEMENTATION · source access: `public`

Canonical language identity length.

```cpp
std::uint8_t LanguageLength;
```

### `ParentLength`

**Classification:** PRIVATE IMPLEMENTATION · source access: `public`

Explicit parent identity length, zero only for terminal language.

```cpp
std::uint8_t ParentLength;
```

### `char`

**Classification:** PRIVATE IMPLEMENTATION · source access: `public`

Owned canonical language identity bytes.

```cpp
std::array<
                char,
```

### `char`

**Classification:** PRIVATE IMPLEMENTATION · source access: `public`

Owned canonical parent identity bytes.

```cpp
std::array<
                char,
```

### `PackSource_`

**Classification:** PRIVATE IMPLEMENTATION · source access: `private`

Pack Source supplying persisted bytes.

```cpp
const TPackSource* PackSource_;
```

### `ByteOperations_`

**Classification:** PRIVATE IMPLEMENTATION · source access: `private`

Raw byte operations provider used for exact comparisons.

```cpp
const TByteOperationsProvider* ByteOperations_;
```

### `ReadUInt16`

**Classification:** PRIVATE IMPLEMENTATION · source access: `private`

Decodes one canonical little-endian uint16 value.

```cpp
[[nodiscard]] static constexpr std::uint16_t ReadUInt16(
            const std::uint8_t* Bytes
        ) noexcept
```

### `ReadUInt32`

**Classification:** PRIVATE IMPLEMENTATION · source access: `private`

Decodes one canonical little-endian uint32 value.

```cpp
[[nodiscard]] static constexpr std::uint32_t ReadUInt32(
            const std::uint8_t* Bytes
        ) noexcept
```

### `ReadUnsigned`

**Classification:** PRIVATE IMPLEMENTATION · source access: `private`

Decodes one 1/2/4/8-byte canonical little-endian unsigned value.

```cpp
[[nodiscard]] static constexpr std::uint64_t ReadUnsigned(
            const std::uint8_t* Bytes,
            std::uint8_t ByteCount
        ) noexcept
```

### `IsRelativeRangeValid`

**Classification:** PRIVATE IMPLEMENTATION · source access: `private`

Indicates whether one relative range is completely contained by TotalLength.

```cpp
[[nodiscard]] static constexpr bool IsRelativeRangeValid(
            std::uint64_t Offset,
            std::uint64_t Length,
            std::uint64_t TotalLength
        ) noexcept
```

### `DoRangesOverlap`

**Classification:** PRIVATE IMPLEMENTATION · source access: `private`

Indicates whether two non-empty absolute ranges overlap.

```cpp
[[nodiscard]] static constexpr bool DoRangesOverlap(
            const SectionDescriptor& Left,
            const SectionDescriptor& Right
        ) noexcept
```

### `ReadExact`

**Classification:** PRIVATE IMPLEMENTATION · source access: `private`

Maps one exact Pack Source read into the public Localisation failure vocabulary.

```cpp
[[nodiscard]] LocalisationStatus ReadExact(
            const PackResource& Resource,
            std::uint64_t Offset,
            std::uint8_t* Destination,
            std::size_t ByteCount
        ) const noexcept
```

### `AssignSection`

**Classification:** PRIVATE IMPLEMENTATION · source access: `private`

Records one required section while rejecting duplicate declarations.

```cpp
[[nodiscard]] static LocalisationStatus AssignSection(
            Edpl::SectionType Type,
            const SectionDescriptor& Value,
            PackLayout& Layout
        ) noexcept
```

### `ReadLayout`

**Classification:** PRIVATE IMPLEMENTATION · source access: `private`

Reads and validates the current-format preamble and required section directory.

```cpp
[[nodiscard]] LocalisationStatus ReadLayout(
            const PackResource& Resource,
            PackLayout& Layout
        ) const noexcept
```

### `ReadLanguageMetadata`

**Classification:** PRIVATE IMPLEMENTATION

Reads and validates language identity, parent, fingerprint and terminal metadata.

```cpp
[[nodiscard]] LocalisationStatus ReadLanguageMetadata(
            const PackResource& Resource,
            const PackLayout& Layout,
            LanguageIdentifierView ExpectedLanguage,
            LanguageMetadataValue& Metadata
        ) const noexcept
```

### `ReadPayloadDescriptor`

**Classification:** PRIVATE IMPLEMENTATION

Reads and validates the shared UTF-8 Payload section header.

```cpp
[[nodiscard]] LocalisationStatus ReadPayloadDescriptor(
            const PackResource& Resource,
            const PackLayout& Layout,
            PayloadDescriptor& Payload
        ) const noexcept
```

### `IsPayloadReferenceValid`

**Classification:** PRIVATE IMPLEMENTATION

Indicates whether one payload reference fits the shared UTF-8 payload.

```cpp
[[nodiscard]] static constexpr bool IsPayloadReferenceValid(
            const PayloadDescriptor& Payload,
            std::uint32_t Offset,
            std::uint32_t Length
        ) noexcept
```

### `ReadGeneralStringsHeader`

**Classification:** PRIVATE IMPLEMENTATION

Reads and validates the fixed General Strings section header and table bounds.

```cpp
[[nodiscard]] LocalisationStatus ReadGeneralStringsHeader(
            const PackResource& Resource,
            const PackLayout& Layout,
            GeneralStringsHeader& HeaderValue
        ) const noexcept
```

### `FindDomain`

**Classification:** PRIVATE IMPLEMENTATION

Locates one Domain record and returns its SubDomain range.

```cpp
[[nodiscard]] LocalisationStatus FindDomain(
            const PackResource& Resource,
            const PackLayout& Layout,
            const GeneralStringsHeader& Header,
            std::uint64_t Target,
            bool& Found,
            std::uint32_t& FirstSubDomain,
            std::uint32_t& SubDomainCount
        ) const noexcept
```

### `FindSubDomain`

**Classification:** PRIVATE IMPLEMENTATION

Locates one SubDomain record within its owning Domain range.

```cpp
[[nodiscard]] LocalisationStatus FindSubDomain(
            const PackResource& Resource,
            const PackLayout& Layout,
            const GeneralStringsHeader& Header,
            std::uint32_t FirstSubDomain,
            std::uint32_t SubDomainCount,
            std::uint64_t Target,
            bool& Found,
            std::uint32_t& FirstString,
            std::uint32_t& StringCount
        ) const noexcept
```

### `FindString`

**Classification:** PRIVATE IMPLEMENTATION

Locates one String record within its owning SubDomain range.

```cpp
[[nodiscard]] RepresentationLookupResult FindString(
            const PackResource& Resource,
            const PackLayout& Layout,
            const GeneralStringsHeader& Header,
            const PayloadDescriptor& Payload,
            std::uint32_t FirstString,
            std::uint32_t StringCount,
            std::uint64_t Target
        ) const noexcept
```

### `ReadTypeSchemaHeader`

**Classification:** PRIVATE IMPLEMENTATION

Reads and validates the fixed Type Schema section header and table bounds.

```cpp
[[nodiscard]] LocalisationStatus ReadTypeSchemaHeader(
            const PackResource& Resource,
            const PackLayout& Layout,
            TypeSchemaHeader& HeaderValue
        ) const noexcept
```

### `FindType`

**Classification:** PRIVATE IMPLEMENTATION

Locates one Type record by canonical raw TypeIdentifier bytes.

```cpp
[[nodiscard]] LocalisationStatus FindType(
            const PackResource& Resource,
            const PackLayout& Layout,
            const TypeSchemaHeader& Header,
            const typename Identifiers::TypeIdentifier& Target,
            bool& Found,
            TypeRecordValue& Value
        ) const noexcept
```

### `FindField`

**Classification:** PRIVATE IMPLEMENTATION

Locates one Field record inside its owning Type's contiguous field range.

```cpp
[[nodiscard]] LocalisationStatus FindField(
            const PackResource& Resource,
            const PackLayout& Layout,
            const TypeSchemaHeader& Header,
            const TypeRecordValue& TypeValue,
            std::uint64_t Target,
            bool& Found,
            FieldRecordValue& Value
        ) const noexcept
```

### `SelectRepresentation`

**Classification:** PRIVATE IMPLEMENTATION

Converts one present/absent Type or Field representation into a lookup result.

```cpp
[[nodiscard]] static RepresentationLookupResult SelectRepresentation(
            bool EntityPresent,
            std::uint8_t Flags,
            std::uint32_t NameOffset,
            std::uint32_t NameLength,
            std::uint32_t DescriptionOffset,
            std::uint32_t DescriptionLength,
            PresentationRepresentation Representation,
            const PayloadDescriptor& Payload
        ) noexcept
```

### `AdvanceCrc32c`

**Classification:** PRIVATE IMPLEMENTATION

Advances one byte through the reflected CRC32C state.

```cpp
[[nodiscard]] static constexpr std::uint32_t AdvanceCrc32c(
            std::uint32_t Crc,
            std::uint8_t Byte
        ) noexcept
```

### `ValidateCrc32c`

**Classification:** PRIVATE IMPLEMENTATION

Verifies the whole-file CRC32C while treating the persisted CRC field as zero.

```cpp
[[nodiscard]] LocalisationStatus ValidateCrc32c(
            const PackResource& Resource,
            const PackLayout& Layout
        ) const noexcept
```

### `ValidateUtf8Payload`

**Classification:** PRIVATE IMPLEMENTATION

Verifies that one complete payload representation is valid UTF-8 without embedded NUL.

```cpp
[[nodiscard]] LocalisationStatus ValidateUtf8Payload(
            const PackResource& Resource,
            const PayloadDescriptor& Payload,
            std::uint32_t PayloadOffset,
            std::uint32_t PayloadLength
        ) const noexcept
```

### `CompareText`

**Classification:** PRIVATE IMPLEMENTATION

Compares two canonical language tags lexicographically.

```cpp
[[nodiscard]] ESPressio::Memory::ByteComparison CompareText(
            const char* Left,
            std::size_t LeftLength,
            const char* Right,
            std::size_t RightLength
        ) const noexcept
```

### `ValidateCompleteSectionDirectory`

**Classification:** PRIVATE IMPLEMENTATION

Verifies that every section-directory range is pairwise non-overlapping.

```cpp
[[nodiscard]] LocalisationStatus ValidateCompleteSectionDirectory(
            const PackResource& Resource,
            const PackLayout& Layout
        ) const noexcept
```

### `ValidateLanguageDisplayNames`

**Classification:** PRIVATE IMPLEMENTATION

Verifies all language-display-name records and referenced UTF-8 payload.

```cpp
[[nodiscard]] LocalisationStatus ValidateLanguageDisplayNames(
            const PackResource& Resource,
            const PackLayout& Layout,
            const PayloadDescriptor& Payload
        ) const noexcept
```

### `ValidateGeneralStrings`

**Classification:** PRIVATE IMPLEMENTATION

Verifies every General Strings table record, range and UTF-8 payload reference.

```cpp
[[nodiscard]] LocalisationStatus ValidateGeneralStrings(
            const PackResource& Resource,
            const PackLayout& Layout,
            const PayloadDescriptor& Payload
        ) const noexcept
```

### `ValidatePresentationPair`

**Classification:** PRIVATE IMPLEMENTATION

Verifies one Type/Field representation payload pair.

```cpp
[[nodiscard]] LocalisationStatus ValidatePresentationPair(
            const PackResource& Resource,
            const PayloadDescriptor& Payload,
            std::uint8_t Flags,
            std::uint8_t PresenceFlag,
            std::uint32_t Offset,
            std::uint32_t Length
        ) const noexcept
```

### `ValidateTypeSchema`

**Classification:** PRIVATE IMPLEMENTATION

Verifies every Type Schema record, including the universal System TypeIdentifier validity rules, canonical ordering, field ranges and presentation payload references.

```cpp
[[nodiscard]] LocalisationStatus ValidateTypeSchema(
            const PackResource& Resource,
            const PackLayout& Layout,
            const PayloadDescriptor& Payload
        ) const noexcept
```

### `LanguageMetadata`

**Classification:** PRIVATE IMPLEMENTATION · source access: `public`

Exposes owned language metadata for format-independent fallback traversal.

```cpp
using LanguageMetadata = LanguageMetadataValue;
```

### `EdplReader`

**Classification:** PRIVATE IMPLEMENTATION · source access: `public`

Binds the selected Pack Source and raw byte-operations provider.

```cpp
EdplReader(
            const TPackSource& PackSourceValue,
            const TByteOperationsProvider& ByteOperations
        ) noexcept :
            PackSource_(&PackSourceValue),
```

### `InspectPack`

**Classification:** PRIVATE IMPLEMENTATION · source access: `public`

Reads current pack layout and language metadata without scanning unrelated payload.

```cpp
[[nodiscard]] LocalisationStatus InspectPack(
            const PackResource& Resource,
            LanguageIdentifierView ExpectedLanguage,
            PackLayout& Layout,
            LanguageMetadata& Metadata,
            PayloadDescriptor& Payload
        ) const noexcept
```

### `ValidateCompletePack`

**Classification:** PRIVATE IMPLEMENTATION · source access: `public`

Performs complete current-format structural, integrity, ordering, and UTF-8 validation.

```cpp
[[nodiscard]] LocalisationStatus ValidateCompletePack(
            const PackResource& Resource,
            LanguageIdentifierView ExpectedLanguage
        ) const noexcept
```

### `LanguageView`

**Classification:** PRIVATE IMPLEMENTATION · source access: `public`

Returns a view into Metadata's owned canonical language bytes.

```cpp
[[nodiscard]] static LanguageIdentifierView LanguageView(
            const LanguageMetadata& Metadata
        ) noexcept
```

### `ParentView`

**Classification:** PRIVATE IMPLEMENTATION · source access: `public`

Returns a view into Metadata's owned parent bytes; caller uses only when ParentLength is non-zero.

```cpp
[[nodiscard]] static LanguageIdentifierView ParentView(
            const LanguageMetadata& Metadata
        ) noexcept
```

### `LookupGeneralString`

**Classification:** PRIVATE IMPLEMENTATION · source access: `public`

Locates one general-string representation without copying payload bytes.

```cpp
[[nodiscard]] RepresentationLookupResult LookupGeneralString(
            const PackResource& Resource,
            const PackLayout& Layout,
            const PayloadDescriptor& Payload,
            const typename Identifiers::GeneralStringIdentifier& Identifier
        ) const noexcept
```

### `LookupLanguageDisplayName`

**Classification:** PRIVATE IMPLEMENTATION · source access: `public`

Locates one localised language display-name representation.

```cpp
[[nodiscard]] RepresentationLookupResult LookupLanguageDisplayName(
            const PackResource& Resource,
            const PackLayout& Layout,
            const PayloadDescriptor& Payload,
            LanguageIdentifierView TargetLanguage
        ) const noexcept
```

### `LookupTypeRepresentation`

**Classification:** PRIVATE IMPLEMENTATION · source access: `public`

Locates one Type Name or Description representation.

```cpp
[[nodiscard]] RepresentationLookupResult LookupTypeRepresentation(
            const PackResource& Resource,
            const PackLayout& Layout,
            const PayloadDescriptor& Payload,
            const typename Identifiers::TypeIdentifier& Type,
            PresentationRepresentation Representation
        ) const noexcept
```

### `LookupFieldRepresentation`

**Classification:** PRIVATE IMPLEMENTATION · source access: `public`

Locates one Field Name or Description representation.

```cpp
[[nodiscard]] RepresentationLookupResult LookupFieldRepresentation(
            const PackResource& Resource,
            const PackLayout& Layout,
            const PayloadDescriptor& Payload,
            const typename Identifiers::FieldPresentationIdentifier& Field,
            PresentationRepresentation Representation
        ) const noexcept
```

### `ReadPayload`

**Classification:** PRIVATE IMPLEMENTATION · source access: `public`

Reads representation payload bytes selected by a previous lookup.

```cpp
[[nodiscard]] LocalisationStatus ReadPayload(
            const PackResource& Resource,
            const PayloadDescriptor& Payload,
            const RepresentationLocation& Representation,
            std::uint8_t* Destination,
            std::size_t ByteCount
        ) const noexcept
```


## Fixed Type identity width

`ReadTypeSchemaHeader` compares the persisted Type width against the generated Contract. Generated Contracts use `8` for every real Type/Field universe and `0` only for absence. `ValidateTypeSchema` also reconstructs each record's `System::TypeIdentifier` and rejects a zero 24-bit Type Authority or zero 40-bit authority-local Type value, while retaining the exact canonical eight bytes for ordering and lookup.
