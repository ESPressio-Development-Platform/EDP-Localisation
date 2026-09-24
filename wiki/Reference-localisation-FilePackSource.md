# src/localisation/FilePackSource.hpp

**Primary classification:** PUBLIC PROVIDER / EXTENSION API

**Source baseline:** `0e7513faf9a126c9a36bf66de7ef116cfcf1226f`

[Open exact source](https://github.com/ESPressio-Development-Platform/EDP-Localisation/blob/0e7513faf9a126c9a36bf66de7ef116cfcf1226f/src/localisation/FilePackSource.hpp)

## Direct includes

- `ESPressio_Memory.hpp`
- `array`
- `cstddef`
- `cstdint`
- `optional`
- `type_traits`
- `ESPressio_Persistence.hpp`
- `LanguageIdentifierView.hpp`
- `LocalisationComposition.hpp`
- `LocalisationTypes.hpp`
- `PackSourceContract.hpp`

## Documented declarations

### `StaticPackDirectory`

**Classification:** PUBLIC PROVIDER / EXTENSION API

Compile-time provider-relative directory used by FilePackSource.

An empty value selects the FileStorage provider root. Non-empty values must satisfy
EDP-Persistence FilePathView canonical path rules and must not end in a separator.

- **Template parameter `TExtent`:** String-literal extent including the terminating NUL byte.

```cpp
template<std::size_t TExtent>
    struct StaticPackDirectory final
```

### `char`

**Classification:** PUBLIC PROVIDER / EXTENSION API · source access: `public`

Provider-relative directory bytes excluding the source literal terminator.

```cpp
std::array<char, TExtent - 1U> Bytes{};
```

### `StaticPackDirectory`

**Classification:** PUBLIC PROVIDER / EXTENSION API · source access: `public`

Copies one compile-time string literal into this structural value.

```cpp
consteval StaticPackDirectory(
            const char (&Value)[TExtent]
        ) noexcept
```

### `Size`

**Classification:** PUBLIC PROVIDER / EXTENSION API · source access: `public`

Returns the exact provider-relative directory byte count.

```cpp
[[nodiscard]] constexpr std::size_t Size() const noexcept
```

### `StaticPackDirectory`

**Classification:** PUBLIC PROVIDER / EXTENSION API

Deduces StaticPackDirectory extent from one string literal.

- **Template parameter `TExtent`:** String-literal extent including the terminating NUL byte.

```cpp
template<std::size_t TExtent>
    StaticPackDirectory(const char (&)[TExtent]) -> StaticPackDirectory<TExtent>;
```

### `TPackDirectory`

**Classification:** PUBLIC PROVIDER / EXTENSION API

Consolidated external FileStorage Requirement used by FilePackSource.

- **Template parameter `TPackDirectory`:** Compile-time provider-relative pack directory.
- **Template parameter `TLocalisationContract`:** Generated Localisation ContractFamily descriptor.
- **Template parameter `TStorageSelectionConstraints`:** Additional FileStorage qualification constraints/Attributes.

```cpp
template<
        StaticPackDirectory TPackDirectory,
```

### `TPersistenceComposition`

**Classification:** PUBLIC PROVIDER / EXTENSION API

Generic file-backed Localisation Pack Source over one qualified EDP-Persistence FileStorage provider.

- **Template parameter `TPersistenceComposition`:** Persistence Composition from which one FileStorage provider is selected.
- **Template parameter `TPackDirectory`:** Compile-time provider-relative pack directory; empty selects provider root.
- **Template parameter `TLocalisationContract`:** Generated Localisation ContractFamily descriptor.
- **Template parameter `TByteOperationsProvider`:** Concrete EDP-Memory ByteOperations provider supplied by Bootstrap.
- **Template parameter `TStorageSelectionConstraints`:** Additional FileStorage Requirement constraints, including optional Attributes.

```cpp
template<
        class TPersistenceComposition,
```

### `char`

**Classification:** PRIVATE IMPLEMENTATION · source access: `private`

Persisted pack filename suffix.

```cpp
inline static constexpr std::array<char, 7U> PackExtension_ =
```

### `Size`

**Classification:** PUBLIC PROVIDER / EXTENSION API

Complete maximum canonical file-path byte count for this ContractFamily.

```cpp
inline static constexpr std::size_t RequiredMaximumPackPathBytes_ =
            TPackDirectory.Size() +
            (TPackDirectory.Size() == 0U ? 0U : 1U) +
            TLocalisationContract::MaximumSupportedLanguageIdentifierBytes +
            PackExtension_.size();
```

### `StorageRequirement`

**Classification:** PUBLIC PROVIDER / EXTENSION API

FileStorage Requirement used both for provider Contract declaration and unique selection.

```cpp
using StorageRequirement = FileStorageRequirement<
            TPackDirectory,
```

### `StorageProvider`

**Classification:** PUBLIC PROVIDER / EXTENSION API

Unique FileStorage provider selected explicitly from the supplied Persistence Composition.

```cpp
using StorageProvider =
            typename TPersistenceComposition::template Select<
                StorageRequirement,
```

### `Resource`

**Classification:** PUBLIC PROVIDER / EXTENSION API

Stable file-backed locator returned by successful Locate.

```cpp
class Resource final
```

### `LanguageLength_`

**Classification:** PRIVATE IMPLEMENTATION · source access: `private`

Exact canonical language byte count.

```cpp
std::uint8_t LanguageLength_;
```

### `char`

**Classification:** PRIVATE IMPLEMENTATION · source access: `private`

Inline canonical language bytes sized from the generated ContractFamily.

```cpp
std::array<
                char,
```

### `Resource`

**Classification:** PRIVATE IMPLEMENTATION · source access: `private`

Copies one validated canonical language identity into stable inline storage.

```cpp
Resource(
                LanguageIdentifierView Language,
                const TByteOperationsProvider& ByteOperations
            ) noexcept :
                LanguageLength_(Language.Length()),
```

### `FilePackSource`

**Classification:** PRIVATE IMPLEMENTATION · source access: `private`

Grants the owning source resource construction and identity access.

```cpp
friend class FilePackSource<
                TPersistenceComposition,
```

### `Resource`

**Classification:** PUBLIC PROVIDER / EXTENSION API · source access: `public`

Copies one stable file-backed resource locator.

```cpp
Resource(const Resource&) noexcept = default;
```

### `operator`

**Classification:** PUBLIC PROVIDER / EXTENSION API · source access: `public`

Replaces one stable file-backed resource locator.

```cpp
Resource& operator=(const Resource&) noexcept = default;
```

### `Storage_`

**Classification:** PUBLIC PROVIDER / EXTENSION API

Selected FileStorage provider instance.

```cpp
const StorageProvider* Storage_;
```

### `ByteOperations_`

**Classification:** PUBLIC PROVIDER / EXTENSION API

Raw byte operations provider used for stable identity/path copying.

```cpp
const TByteOperationsProvider* ByteOperations_;
```

### `IsPackDirectoryValid`

**Classification:** PUBLIC PROVIDER / EXTENSION API

Indicates whether the configured provider-relative directory satisfies Persistence path rules.

```cpp
[[nodiscard]] static consteval bool IsPackDirectoryValid() noexcept
```

### `BuildPackPath`

**Classification:** PUBLIC PROVIDER / EXTENSION API

Builds one provider-relative canonical pack path into caller-owned operation scratch.

```cpp
[[nodiscard]] std::size_t BuildPackPath(
            LanguageIdentifierView Language,
            std::array<char, RequiredMaximumPackPathBytes_>& Path
        ) const noexcept
```

### `BuildResourcePath`

**Classification:** PUBLIC PROVIDER / EXTENSION API

Reconstructs and validates one pack path from a stable resource.

```cpp
[[nodiscard]] ESPressio::Persistence::FilePathView::ValidationResult BuildResourcePath(
            const Resource& ResourceValue,
            std::array<char, RequiredMaximumPackPathBytes_>& Path
        ) const noexcept
```

### `MapLocateStatus`

**Classification:** PUBLIC PROVIDER / EXTENSION API

Maps FileSizeStatus into Locate semantics.

```cpp
[[nodiscard]] static constexpr PackLocateStatus MapLocateStatus(
            ESPressio::Persistence::FileSizeStatus Status
        ) noexcept
```

### `MapSizeStatus`

**Classification:** PUBLIC PROVIDER / EXTENSION API

Maps FileSizeStatus into observation-after-Locate semantics.

```cpp
[[nodiscard]] static constexpr PackSizeStatus MapSizeStatus(
            ESPressio::Persistence::FileSizeStatus Status
        ) noexcept
```

### `MapReadFailureStatus`

**Classification:** PUBLIC PROVIDER / EXTENSION API

Maps a non-successful FileReadStatus into Pack Source semantics.

```cpp
[[nodiscard]] static constexpr PackReadStatus MapReadFailureStatus(
            ESPressio::Persistence::FileReadStatus Status
        ) noexcept
```

### `PackResource`

**Classification:** PUBLIC PROVIDER / EXTENSION API · source access: `public`

Provider-associated stable resource type consumed by Pack Source operations.

```cpp
using PackResource = Resource;
```

### `PersistenceRequirement`

**Classification:** PUBLIC PROVIDER / EXTENSION API · source access: `public`

Exposes the consolidated cross-domain Persistence Requirement for Architecture validation/testing.

```cpp
using PersistenceRequirement = StorageRequirement;
```

### `RequiredMaximumPackPathBytes`

**Classification:** PUBLIC PROVIDER / EXTENSION API · source access: `public`

Maximum canonical pack path bytes required by this FilePackSource specialization.

```cpp
inline static constexpr std::size_t RequiredMaximumPackPathBytes =
            RequiredMaximumPackPathBytes_;
```

### `FilePackSource`

**Classification:** PUBLIC PROVIDER / EXTENSION API · source access: `public`

Binds the compile-time selected FileStorage instance and Bootstrap-owned ByteOperations provider.

```cpp
FilePackSource(
            const StorageProvider& Storage,
            const TByteOperationsProvider& ByteOperations
        ) noexcept :
            Storage_(&Storage),
```

### `Locate`

**Classification:** PUBLIC PROVIDER / EXTENSION API · source access: `public`

Locates one currently addressable language pack and returns a stable resource identity.

```cpp
[[nodiscard]] PackLocateResult<PackResource> Locate(
            LanguageIdentifierView Language
        ) const noexcept
```

### `Size`

**Classification:** PUBLIC PROVIDER / EXTENSION API · source access: `public`

Performs a fresh FileStorage size observation for one previously located resource.

```cpp
[[nodiscard]] PackSizeResult Size(
            const PackResource& ResourceValue
        ) const noexcept
```

### `Read`

**Classification:** PUBLIC PROVIDER / EXTENSION API · source access: `public`

Performs one exact bounded read over the selected FileStorage resource.

```cpp
[[nodiscard]] PackReadResult Read(
            const PackResource& ResourceValue,
            std::uint64_t Offset,
            WritableByteView Destination
        ) const noexcept
```

### `LanguageIdentity`

**Classification:** PUBLIC PROVIDER / EXTENSION API · source access: `public`

Returns the stable canonical language identity owned by ResourceValue without touching Persistence.

```cpp
[[nodiscard]] LanguageIdentifierView LanguageIdentity(
            const PackResource& ResourceValue
        ) const noexcept
```

