# src/localisation/LocalisationTypes.hpp

**Primary classification:** PUBLIC API

**Source baseline:** `9162728ae3e27adc1707f12881009c1c7b11fa3b`

[Open exact source](https://github.com/ESPressio-Development-Platform/EDP-Localisation/blob/9162728ae3e27adc1707f12881009c1c7b11fa3b/src/localisation/LocalisationTypes.hpp)

## Direct includes

- `array`
- `cstddef`
- `cstdint`
- `type_traits`
- `ESPressio_System.hpp`

## Documented declarations

### `TypeIdentifierBytes`

**Classification:** PUBLIC API

Platform-wide canonical EDP Type identity width.

```cpp
inline constexpr std::size_t TypeIdentifierBytes = 8U;
```

Every real EDP Type identity is therefore exactly 64 bits. The value is owned by EDP-System; Localisation reflects the System-owned width rather than defining a competing identity domain.


### `LocalisationStatus`

**Classification:** PUBLIC API

Mutually exclusive outcome of one Localisation resolution operation.

```cpp
enum class LocalisationStatus : std::uint8_t
```

### `LocalisationFact`

**Classification:** PUBLIC API

Orthogonal facts reported alongside a successful Localisation resolution.

```cpp
enum class LocalisationFact : std::uint8_t
```

### `TextMaterialisationStatus`

**Classification:** PUBLIC API

Mutually exclusive outcome of materialising one resolved textual identity.

```cpp
enum class TextMaterialisationStatus : std::uint8_t
```

### `TextMaterialisationFact`

**Classification:** PUBLIC API

Orthogonal facts reported alongside successful text materialisation.

```cpp
enum class TextMaterialisationFact : std::uint8_t
```

### `TextOutputMode`

**Classification:** PUBLIC API

Caller-selected representation mode for UTF-8 output buffers.

```cpp
enum class TextOutputMode : std::uint8_t
```

### `ValidationStatus`

**Classification:** PUBLIC API

Mutually exclusive outcome of explicit pack/context validation.

```cpp
enum class ValidationStatus : std::uint8_t
```

### `LocalisationFacts`

**Classification:** PUBLIC API

Compact typed successful-resolution facts.

```cpp
using LocalisationFacts = ESPressio::System::FlagSet<
        LocalisationFact,
```

### `TextMaterialisationFacts`

**Classification:** PUBLIC API

Compact typed successful text-materialisation facts.

```cpp
using TextMaterialisationFacts = ESPressio::System::FlagSet<
        TextMaterialisationFact,
```

### `TextMaterialisationResult`

**Classification:** PUBLIC API

Result of materialising one already-resolved canonical language identity.

```cpp
struct TextMaterialisationResult final
```

### `Status`

**Classification:** PUBLIC API · source access: `public`

Materialisation outcome.

```cpp
TextMaterialisationStatus Status;
```

### `Facts`

**Classification:** PUBLIC API · source access: `public`

Orthogonal successful materialisation facts.

```cpp
TextMaterialisationFacts Facts;
```

### `BytesWritten`

**Classification:** PUBLIC API · source access: `public`

Number of UTF-8 payload bytes written, excluding a NUL terminator.

```cpp
std::size_t BytesWritten;
```

### `RequiredBytes`

**Classification:** PUBLIC API · source access: `public`

Complete UTF-8 payload byte count required, excluding a NUL terminator.

```cpp
std::size_t RequiredBytes;
```

### `ValidationResult`

**Classification:** PUBLIC API

Result of explicit pack or context validation.

```cpp
struct ValidationResult final
```

### `Status`

**Classification:** PUBLIC API · source access: `public`

Validation outcome.

```cpp
ValidationStatus Status;
```

### `WritableTextView`

**Classification:** PUBLIC API

Describes caller-owned writable text storage.

```cpp
struct WritableTextView final
```

### `Data`

**Classification:** PUBLIC API · source access: `public`

First writable byte, or null only when Capacity is zero.

```cpp
char* Data;
```

### `Capacity`

**Classification:** PUBLIC API · source access: `public`

Number of writable bytes available from Data.

```cpp
std::size_t Capacity;
```

### `WritableByteView`

**Classification:** PUBLIC API

Describes caller-owned writable opaque byte storage.

```cpp
struct WritableByteView final
```

### `Data`

**Classification:** PUBLIC API · source access: `public`

First writable byte, or null only when Size is zero.

```cpp
std::uint8_t* Data;
```

### `Size`

**Classification:** PUBLIC API · source access: `public`

Number of writable bytes available from Data.

```cpp
std::size_t Size;
```

### `UnsignedStorageForBytes`

**Classification:** PUBLIC API

Selects an unsigned integer storage type from one supported byte width.

- **Template parameter `TBytes`:** Exact unsigned storage width in bytes.

```cpp
template<std::size_t TBytes>
        struct UnsignedStorageForBytes;
```

### `UnsignedStorageForBytes`

**Classification:** PUBLIC API

One-byte unsigned identifier storage.

```cpp
template<>
        struct UnsignedStorageForBytes<1U> final
```

### `Type`

**Classification:** PUBLIC API · source access: `public`

Unsigned integer type occupying one byte.

```cpp
using Type = std::uint8_t;
```

### `UnsignedStorageForBytes`

**Classification:** PUBLIC API

Two-byte unsigned identifier storage.

```cpp
template<>
        struct UnsignedStorageForBytes<2U> final
```

### `Type`

**Classification:** PUBLIC API · source access: `public`

Unsigned integer type occupying two bytes.

```cpp
using Type = std::uint16_t;
```

### `UnsignedStorageForBytes`

**Classification:** PUBLIC API

Four-byte unsigned identifier storage.

```cpp
template<>
        struct UnsignedStorageForBytes<4U> final
```

### `Type`

**Classification:** PUBLIC API · source access: `public`

Unsigned integer type occupying four bytes.

```cpp
using Type = std::uint32_t;
```

### `UnsignedStorageForBytes`

**Classification:** PUBLIC API

Eight-byte unsigned identifier storage.

```cpp
template<>
        struct UnsignedStorageForBytes<8U> final
```

### `Type`

**Classification:** PUBLIC API · source access: `public`

Unsigned integer type occupying eight bytes.

```cpp
using Type = std::uint64_t;
```

### `UnsignedStorageForBytesType`

**Classification:** PUBLIC API

Exact unsigned integer type selected by TBytes.

- **Template parameter `TBytes`:** Exact unsigned storage width in bytes.

```cpp
template<std::size_t TBytes>
        using UnsignedStorageForBytesType = typename UnsignedStorageForBytes<TBytes>::Type;
```

### `DomainIdentifierTag`

**Classification:** PUBLIC API

Semantic tag for Domain identifiers.

```cpp
struct DomainIdentifierTag final {};
```

### `SubDomainIdentifierTag`

**Classification:** PUBLIC API · source access: `public`

Semantic tag for SubDomain identifiers.

```cpp
struct SubDomainIdentifierTag final {};
```

### `StringIdentifierTag`

**Classification:** PUBLIC API · source access: `public`

Semantic tag for general String identifiers.

```cpp
struct StringIdentifierTag final {};
```

### `TypeIdentifierTag`

**Classification:** PUBLIC API · source access: `public`

Semantic tag used only for the unconstructible sentinel when a ContractFamily has no Type presentation universe.

```cpp
struct TypeIdentifierTag final {};
```

### `FieldIdentifierTag`

**Classification:** PUBLIC API · source access: `public`

Semantic tag for Type-local Field identifiers.

```cpp
struct FieldIdentifierTag final {};
```

### `TTag`

**Classification:** PUBLIC API · source access: `public`

Strong numeric identifier whose storage width is part of the generated ContractFamily.

- **Template parameter `TTag`:** Semantic identifier domain.
- **Template parameter `TBytes`:** Exact storage width in bytes.

```cpp
template<class TTag, std::size_t TBytes>
    class NumericIdentifier final
```

### `Value_`

**Classification:** PRIVATE IMPLEMENTATION · source access: `private`

Exact numeric identity value.

```cpp
Detail::UnsignedStorageForBytesType<TBytes> Value_;
```

### `Storage`

**Classification:** PUBLIC API · source access: `public`

Exact unsigned storage type selected by TBytes.

```cpp
using Storage = Detail::UnsignedStorageForBytesType<TBytes>;
```

### `NumericIdentifier`

**Classification:** PUBLIC API · source access: `public`

Constructs an identifier from its exact numeric representation.

```cpp
constexpr explicit NumericIdentifier(Storage Value) noexcept :
            Value_(Value) {}
```

### `Value`

**Classification:** PUBLIC API · source access: `public`

Returns the exact numeric representation.

```cpp
[[nodiscard]] constexpr Storage Value() const noexcept
```

### `operator`

**Classification:** PUBLIC API · source access: `public`

Compares two identifiers from the same semantic domain.

```cpp
[[nodiscard]] constexpr bool operator==(const NumericIdentifier&) const noexcept = default;
```

### `TTag`

**Classification:** PUBLIC API · source access: `public`

Deliberately unconstructible identifier used when one optional identifier universe is absent.

The type remains complete so generic Resolver declarations remain well-formed, while callers
cannot accidentally manufacture an identity for a ContractFamily that has no such universe.

- **Template parameter `TTag`:** Semantic identifier domain intentionally unavailable in this ContractFamily.

```cpp
template<class TTag>
    class UnavailableIdentifier final
```

### `UnavailableIdentifier`

**Classification:** PRIVATE IMPLEMENTATION · source access: `private`

Prevents construction when the corresponding identifier universe is absent.

```cpp
UnavailableIdentifier() = delete;
```

### `TTag`

**Classification:** PUBLIC API · source access: `public`

Selects an available numeric identifier or the unconstructible unavailable form.

- **Template parameter `TTag`:** Semantic identifier domain.
- **Template parameter `TBytes`:** Exact numeric identifier width.
- **Template parameter `TAvailable`:** Indicates whether the identifier universe exists.

```cpp
template<class TTag, std::size_t TBytes, bool TAvailable = (TBytes != 0U)>
        struct NumericIdentifierSelector;
```

### `TTag`

**Classification:** PUBLIC API · source access: `public`

Selects the concrete numeric identifier when its universe exists.

- **Template parameter `TTag`:** Semantic identifier domain.
- **Template parameter `TBytes`:** Exact numeric identifier width.

```cpp
template<class TTag, std::size_t TBytes>
        struct NumericIdentifierSelector<TTag, TBytes, true> final
```

### `Type`

**Classification:** PUBLIC API · source access: `public`

Available numeric identifier type.

```cpp
using Type = NumericIdentifier<TTag, TBytes>;
```

### `TTag`

**Classification:** PUBLIC API · source access: `public`

Selects the unavailable identifier when the numeric universe is absent.

- **Template parameter `TTag`:** Semantic identifier domain.
- **Template parameter `TBytes`:** Zero-width marker for the absent identifier universe.

```cpp
template<class TTag, std::size_t TBytes>
        struct NumericIdentifierSelector<TTag, TBytes, false> final
```

### `Type`

**Classification:** PUBLIC API · source access: `public`

Deliberately unconstructible identifier type.

```cpp
using Type = UnavailableIdentifier<TTag>;
```

### `TTag`

**Classification:** INTERNAL API · source access: `public`

Selects the universal System Type identifier or the unconstructible unavailable form.

- **Template parameter `TTag`:** Semantic identifier domain used by the unavailable sentinel.
- **Template parameter `TAvailable`:** Indicates whether the ContractFamily contains a Type presentation universe.

```cpp
template<class TTag, bool TAvailable>
        struct TypeIdentifierSelector;
```

### `TypeIdentifierSelector<TTag, true>`

**Classification:** INTERNAL API · source access: `public`

Selects `ESPressio::System::TypeIdentifier` when the ContractFamily exposes a Type presentation universe.

- **Template parameter `TTag`:** Retained for selector symmetry and the unavailable alternative.

```cpp
template<class TTag>
        struct TypeIdentifierSelector<TTag, true> final
```

### `Type`

**Classification:** INTERNAL API · source access: `public`

The System-owned universal Type identity.

```cpp
using Type = ESPressio::System::TypeIdentifier;
```

### `TypeIdentifierSelector<TTag, false>`

**Classification:** INTERNAL API · source access: `public`

Selects the deliberately unconstructible Localisation sentinel when no Type presentation universe exists.

- **Template parameter `TTag`:** Semantic identifier domain intentionally unavailable.

```cpp
template<class TTag>
        struct TypeIdentifierSelector<TTag, false> final
```

### `Type`

**Classification:** INTERNAL API · source access: `public`

Unavailable Type-identity sentinel.

```cpp
using Type = UnavailableIdentifier<TTag>;
```

### `TContract`

**Classification:** PUBLIC API · source access: `public`

Contract-derived Localisation identifier vocabulary.

- **Template parameter `TContract`:** Generated Localisation contract descriptor defining identifier widths.

```cpp
template<class TContract>
    struct ContractIdentifiers final
```

### `DomainIdentifier`

**Classification:** PUBLIC API · source access: `public`

Strong Application/Platform Domain identifier.

```cpp
using DomainIdentifier = NumericIdentifier<
            Detail::DomainIdentifierTag,
```

### `SubDomainIdentifier`

**Classification:** PUBLIC API · source access: `public`

Strong SubDomain identifier local to a Domain.

```cpp
using SubDomainIdentifier = NumericIdentifier<
            Detail::SubDomainIdentifierTag,
```

### `StringIdentifierValue`

**Classification:** PUBLIC API · source access: `public`

Strong String identifier local to a SubDomain.

```cpp
using StringIdentifierValue = NumericIdentifier<
            Detail::StringIdentifierTag,
```

### `TypeIdentifier`

**Classification:** PUBLIC API · source access: `public`

System-owned universal Type identifier when the ContractFamily exposes Type presentations; otherwise the deliberately unconstructible Localisation sentinel.

```cpp
using TypeIdentifier = typename Detail::TypeIdentifierSelector<
            Detail::TypeIdentifierTag,
            (TContract::TypeIdentifierBytes != 0U)
```

### `FieldIdentifier`

**Classification:** PUBLIC API · source access: `public`

Strong Field identifier local to a Type, unavailable when no schema universe exists.

```cpp
using FieldIdentifier = typename Detail::NumericIdentifierSelector<
            Detail::FieldIdentifierTag,
```

### `GeneralStringIdentifier`

**Classification:** PUBLIC API · source access: `public`

Complete identity of one general Localisation string.

```cpp
struct GeneralStringIdentifier final
```

### `Domain`

**Classification:** PUBLIC API · source access: `public`

Domain containing the represented string.

```cpp
DomainIdentifier Domain;
```

### `SubDomain`

**Classification:** PUBLIC API · source access: `public`

SubDomain containing the represented string.

```cpp
SubDomainIdentifier SubDomain;
```

### `String`

**Classification:** PUBLIC API · source access: `public`

String identity within SubDomain.

```cpp
StringIdentifierValue String;
```

### `FieldPresentationIdentifier`

**Classification:** PUBLIC API · source access: `public`

Complete identity of one presentable field.

```cpp
struct FieldPresentationIdentifier final
```

### `Type`

**Classification:** PUBLIC API · source access: `public`

Globally unique owning Type identity.

```cpp
TypeIdentifier Type;
```

### `Field`

**Classification:** PUBLIC API · source access: `public`

Field identity local to Type.

```cpp
FieldIdentifier Field;
```

