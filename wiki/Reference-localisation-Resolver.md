# src/localisation/Resolver.hpp

**Primary classification:** PUBLIC API

**Source baseline:** `f2588c7bb6e34b5983b1a0a16fcd12ae5ba08010`

[Open exact source](https://github.com/ESPressio-Development-Platform/EDP-Localisation/blob/f2588c7bb6e34b5983b1a0a16fcd12ae5ba08010/src/localisation/Resolver.hpp)

## Direct includes

- `ESPressio_Memory.hpp`
- `array`
- `cstddef`
- `cstdint`
- `optional`
- `LanguageIdentifierView.hpp`
- `LocalisationContext.hpp`
- `LocalisationTypes.hpp`
- `PackSourceContract.hpp`
- `detail/EdplReader.hpp`

## Documented declarations

### `TPackSource`

**Classification:** PUBLIC API

Stateless V1 Localisation resolver over one selected Pack Source and generated ContractFamily.

- **Template parameter `TPackSource`:** Concrete provider satisfying the Localisation Pack Source contract.
- **Template parameter `TByteOperationsProvider`:** Concrete provider satisfying EDP-Memory ByteOperations.
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

### `Reader`

**Classification:** PRIVATE IMPLEMENTATION · source access: `private`

Current-format EDPL reader bound to this Resolver specialization.

```cpp
using Reader = Detail::EdplReader<
            TPackSource,
```

### `IdentifierVocabulary`

**Classification:** PRIVATE IMPLEMENTATION · source access: `private`

Internal contract-derived identifier vocabulary.

```cpp
using IdentifierVocabulary = ContractIdentifiers<TContract>;
```

### `LanguageHandleValue`

**Classification:** PRIVATE IMPLEMENTATION · source access: `private`

Internal opaque runtime handle value exposed through the public LanguageHandle alias.

```cpp
class LanguageHandleValue final
```

### `Resource_`

**Classification:** PRIVATE IMPLEMENTATION · source access: `private`

Provider-associated resource whose language supplied the representation.

```cpp
PackResource Resource_;
```

### `LanguageHandleValue`

**Classification:** PRIVATE IMPLEMENTATION · source access: `private`

Constructs a successful handle from one stable PackResource.

```cpp
explicit LanguageHandleValue(
                const PackResource& Resource
            ) noexcept :
                Resource_(Resource) {}
```

### `Resolver`

**Classification:** PRIVATE IMPLEMENTATION · source access: `private`

Grants this Resolver specialization handle-construction access.

```cpp
friend class Resolver<
                TPackSource,
```

### `LanguageHandleValue`

**Classification:** PUBLIC API · source access: `public`

Copies one runtime language handle.

```cpp
LanguageHandleValue(const LanguageHandleValue&) noexcept = default;
```

### `operator`

**Classification:** PUBLIC API · source access: `public`

Replaces one runtime language handle.

```cpp
LanguageHandleValue& operator=(const LanguageHandleValue&) noexcept = default;
```

### `LanguageHandleValue`

**Classification:** PUBLIC API · source access: `public`

Destroys the wrapped provider-associated resource.

```cpp
~LanguageHandleValue() = default;
```

### `ResolveResultValue`

**Classification:** PRIVATE IMPLEMENTATION · source access: `private`

Internal result value exposed through the public ResolveResult alias.

```cpp
struct ResolveResultValue final
```

### `Status`

**Classification:** PUBLIC API · source access: `public`

Mutually exclusive resolution outcome.

```cpp
LocalisationStatus Status;
```

### `Facts`

**Classification:** PUBLIC API · source access: `public`

Orthogonal facts describing one successful resolution.

```cpp
LocalisationFacts Facts;
```

### `BytesWritten`

**Classification:** PUBLIC API · source access: `public`

Number of UTF-8 payload bytes written, excluding an optional NUL terminator.

```cpp
std::size_t BytesWritten;
```

### `RequiredBytes`

**Classification:** PUBLIC API · source access: `public`

Complete UTF-8 payload byte count required, excluding an optional NUL terminator.

```cpp
std::size_t RequiredBytes;
```

### `ResolvedLanguage`

**Classification:** PUBLIC API · source access: `public`

Supplying language handle present only when Status is Success.

```cpp
std::optional<LanguageHandleValue> ResolvedLanguage;
```

### `Utf8PrefixAnalysis`

**Classification:** PRIVATE IMPLEMENTATION · source access: `private`

Result of analysing the largest complete UTF-8 prefix in one bounded byte range.

```cpp
struct Utf8PrefixAnalysis final
```

### `IsValid`

**Classification:** PUBLIC API · source access: `public`

Indicates whether every fully observed code point is valid UTF-8.

```cpp
bool IsValid;
```

### `CompleteBytes`

**Classification:** PUBLIC API · source access: `public`

Largest complete UTF-8 prefix in the inspected byte range.

```cpp
std::size_t CompleteBytes;
```

### `PackSource_`

**Classification:** PRIVATE IMPLEMENTATION · source access: `private`

Pack Source supplying current ContractFamily language-pack bytes.

```cpp
const TPackSource* PackSource_;
```

### `ByteOperations_`

**Classification:** PRIVATE IMPLEMENTATION · source access: `private`

Bootstrap-owned raw byte operations provider.

```cpp
const TByteOperationsProvider* ByteOperations_;
```

### `Reader_`

**Classification:** PRIVATE IMPLEMENTATION · source access: `private`

Current-release EDPL reader bound to the same providers.

```cpp
Reader Reader_;
```

### `IsOutputModeValid`

**Classification:** PRIVATE IMPLEMENTATION · source access: `private`

Indicates whether Mode is one of the two public V1 text-output modes.

```cpp
[[nodiscard]] static constexpr bool IsOutputModeValid(
            TextOutputMode Mode
        ) noexcept
```

### `IsDestinationValid`

**Classification:** PRIVATE IMPLEMENTATION · source access: `private`

Indicates whether Destination satisfies its caller-owned pointer/capacity invariant.

```cpp
[[nodiscard]] static constexpr bool IsDestinationValid(
            WritableTextView Destination
        ) noexcept
```

### `MakeFailure`

**Classification:** PRIVATE IMPLEMENTATION · source access: `private`

Creates one normalized failed Resolve result.

```cpp
[[nodiscard]] static ResolveResultValue MakeFailure(
            LocalisationStatus Status
        ) noexcept
```

### `MapLocateStatus`

**Classification:** PUBLIC API

Maps a Pack Source locate failure into Localisation semantics.

```cpp
[[nodiscard]] static constexpr LocalisationStatus MapLocateStatus(
            PackLocateStatus Status
        ) noexcept
```

### `MapValidationStatus`

**Classification:** PUBLIC API

Maps the Resolver's broader LocalisationStatus into validation-specific vocabulary.

```cpp
[[nodiscard]] static constexpr ValidationStatus MapValidationStatus(
            LocalisationStatus Status
        ) noexcept
```

### `AnalyseUtf8Prefix`

**Classification:** PUBLIC API

Analyses an already-read UTF-8 prefix and optionally tolerates one incomplete trailing code point.

```cpp
[[nodiscard]] static constexpr Utf8PrefixAnalysis AnalyseUtf8Prefix(
            const char* Data,
            std::size_t Size,
            bool AllowIncompleteTrailingCodePoint
        ) noexcept
```

### `MaterialiseRepresentation`

**Classification:** PUBLIC API

Copies one selected representation into caller-owned text storage.

```cpp
[[nodiscard]] ResolveResultValue MaterialiseRepresentation(
            const PackResource& Resource,
            const Detail::PayloadDescriptor& Payload,
            const Detail::RepresentationLocation& Representation,
            LanguageIdentifierView RequestedLanguage,
            WritableTextView Destination,
            TextOutputMode OutputMode
        ) const noexcept
```

### `ValidateTerminalForResolve`

**Classification:** PUBLIC API

Performs the touched-data validation required to establish the caller's terminal contract boundary.

```cpp
[[nodiscard]] LocalisationStatus ValidateTerminalForResolve(
            const LocalisationContext& Context
        ) const noexcept
```

### `TLookup`

**Classification:** PUBLIC API

Resolves one representation surface through the explicit parent chain.

- **Template parameter `TLookup`:** Callable accepting Resource, Layout and Payload and returning RepresentationLookupResult.

```cpp
template<class TLookup>
        [[nodiscard]] ResolveResultValue ResolveRepresentation(
            const LocalisationContext& Context,
            WritableTextView Destination,
            TextOutputMode OutputMode,
            bool RequireTerminalNameWhenEntityExists,
            TLookup&& Lookup
        ) const noexcept
```

### `ValidatePackForContext`

**Classification:** PUBLIC API

Fully validates one already-located language pack and returns its metadata for chain traversal.

```cpp
[[nodiscard]] ValidationStatus ValidatePackForContext(
            LanguageIdentifierView Language,
            typename Reader::LanguageMetadata& Metadata
        ) const noexcept
```

### `Identifiers`

**Classification:** PUBLIC API · source access: `public`

Contract-derived identifier vocabulary used by this Resolver specialization.

```cpp
using Identifiers = IdentifierVocabulary;
```

### `GeneralStringIdentifier`

**Classification:** PUBLIC API · source access: `public`

Strong general-string lookup identity.

```cpp
using GeneralStringIdentifier =
            typename Identifiers::GeneralStringIdentifier;
```

### `TypeIdentifier`

**Classification:** PUBLIC API · source access: `public`

Strong globally unique fixed 64-bit EDP Type identity.

```cpp
using TypeIdentifier =
            typename Identifiers::TypeIdentifier;
```

### `FieldPresentationIdentifier`

**Classification:** PUBLIC API · source access: `public`

Strong Type-local Field presentation identity.

```cpp
using FieldPresentationIdentifier =
            typename Identifiers::FieldPresentationIdentifier;
```

### `CompositionContract`

**Classification:** PUBLIC API · source access: `public`

Consolidated compile-time dependency contract of one Resolver consumer.

```cpp
using CompositionContract = ResolverContract;
```

### `LanguageHandle`

**Classification:** PUBLIC API · source access: `public`

Opaque runtime handle identifying the pack that supplied a successful representation.

```cpp
using LanguageHandle = LanguageHandleValue;
```

### `ResolveResult`

**Classification:** PUBLIC API · source access: `public`

Result returned by every Localisation representation lookup.

```cpp
using ResolveResult = ResolveResultValue;
```

### `Resolver`

**Classification:** PUBLIC API · source access: `public`

Binds the stateless Resolver to Bootstrap-owned Pack Source and ByteOperations providers.

```cpp
Resolver(
            const TPackSource& PackSourceValue,
            const TByteOperationsProvider& ByteOperations
        ) noexcept :
            PackSource_(&PackSourceValue),
```

### `ResolveString`

**Classification:** PUBLIC API · source access: `public`

Resolves one Domain/SubDomain/String presentation value through explicit fallback.

```cpp
[[nodiscard]] ResolveResult ResolveString(
            const LocalisationContext& Context,
            const GeneralStringIdentifier& Identifier,
            WritableTextView Destination,
            TextOutputMode OutputMode
        ) const noexcept
```

### `ResolveLanguageDisplayName`

**Classification:** PUBLIC API · source access: `public`

Resolves the localised display name of TargetLanguage using Context's requested language.

```cpp
[[nodiscard]] ResolveResult ResolveLanguageDisplayName(
            const LocalisationContext& Context,
            LanguageIdentifierView TargetLanguage,
            WritableTextView Destination,
            TextOutputMode OutputMode
        ) const noexcept
```

### `ResolveTypeName`

**Classification:** PUBLIC API · source access: `public`

Resolves one Type Name through explicit language fallback. A schema-backed invalid `System::TypeIdentifier` returns `InvalidArgument` before lookup.

```cpp
[[nodiscard]] ResolveResult ResolveTypeName(
            const LocalisationContext& Context,
            TypeIdentifier Type,
            WritableTextView Destination,
            TextOutputMode OutputMode
        ) const noexcept
```

### `ResolveTypeDescription`

**Classification:** PUBLIC API · source access: `public`

Resolves one optional Type Description through explicit language fallback. A schema-backed invalid `System::TypeIdentifier` returns `InvalidArgument` before lookup.

```cpp
[[nodiscard]] ResolveResult ResolveTypeDescription(
            const LocalisationContext& Context,
            TypeIdentifier Type,
            WritableTextView Destination,
            TextOutputMode OutputMode
        ) const noexcept
```

### `ResolveFieldName`

**Classification:** PUBLIC API · source access: `public`

Resolves one Field Name through explicit language fallback. An invalid owning `System::TypeIdentifier` returns `InvalidArgument` before lookup.

```cpp
[[nodiscard]] ResolveResult ResolveFieldName(
            const LocalisationContext& Context,
            const FieldPresentationIdentifier& Field,
            WritableTextView Destination,
            TextOutputMode OutputMode
        ) const noexcept
```

### `ResolveFieldDescription`

**Classification:** PUBLIC API · source access: `public`

Resolves one optional Field Description through explicit language fallback. An invalid owning `System::TypeIdentifier` returns `InvalidArgument` before lookup.

```cpp
[[nodiscard]] ResolveResult ResolveFieldDescription(
            const LocalisationContext& Context,
            const FieldPresentationIdentifier& Field,
            WritableTextView Destination,
            TextOutputMode OutputMode
        ) const noexcept
```

### `ResolveLanguageIdentity`

**Classification:** PUBLIC API · source access: `public`

Copies the canonical BCP47 identity represented by Language into caller-owned storage.

```cpp
[[nodiscard]] TextMaterialisationResult ResolveLanguageIdentity(
            const LanguageHandle& Language,
            WritableTextView Destination,
            TextOutputMode OutputMode
        ) const noexcept
```

### `ValidateLanguagePack`

**Classification:** PUBLIC API · source access: `public`

Performs complete current-format validation of one language pack.

```cpp
[[nodiscard]] ValidationResult ValidateLanguagePack(
            LanguageIdentifierView Language
        ) const noexcept
```

### `ValidateContext`

**Classification:** PUBLIC API · source access: `public`

Fully validates the requested-to-terminal fallback chain supplied by Context.

```cpp
[[nodiscard]] ValidationResult ValidateContext(
            const LocalisationContext& Context
        ) const noexcept
```

