# src/localisation/InBinaryPackSource.hpp

**Primary classification:** PUBLIC PROVIDER / EXTENSION API

**Source baseline:** `702f9d2d4080d5548e186fa1fd3f4156e581939b`

[Open exact source](https://github.com/ESPressio-Development-Platform/EDP-Localisation/blob/702f9d2d4080d5548e186fa1fd3f4156e581939b/src/localisation/InBinaryPackSource.hpp)

## Direct includes

- `cstddef`
- `cstdint`
- `memory/ByteOperationsContract.hpp`
- `memory/MemoryTypes.hpp`
- `LanguageIdentifierView.hpp`
- `LocalisationComposition.hpp`
- `LocalisationTypes.hpp`
- `PackSourceContract.hpp`

## Documented declarations

### `InBinaryPackDescriptor`

**Classification:** PUBLIC PROVIDER / EXTENSION API

Immutable in-binary pack descriptor owned by generated/application storage.

```cpp
struct InBinaryPackDescriptor final
```

### `Language`

**Classification:** PUBLIC PROVIDER / EXTENSION API · source access: `public`

Canonical language identity represented by this pack.

```cpp
LanguageIdentifierView Language;
```

### `Data`

**Classification:** PUBLIC PROVIDER / EXTENSION API · source access: `public`

First immutable pack byte.

```cpp
const std::uint8_t* Data;
```

### `SizeBytes`

**Classification:** PUBLIC PROVIDER / EXTENSION API · source access: `public`

Number of immutable pack bytes.

```cpp
std::size_t SizeBytes;
```

### `TByteOperationsProvider`

**Classification:** PUBLIC PROVIDER / EXTENSION API

Pack Source exposing immutable generated EDPL bytes compiled into the application.

- **Template parameter `TByteOperationsProvider`:** Concrete provider satisfying EDP-Memory ByteOperations.

```cpp
template<class TByteOperationsProvider>
    class InBinaryPackSource final : public Framework::Provider<
        Domain,
```

### `Resource`

**Classification:** PRIVATE IMPLEMENTATION · source access: `private`

Stable provider-associated locator returned after successful Locate.

```cpp
class Resource final
```

### `Descriptor_`

**Classification:** PRIVATE IMPLEMENTATION · source access: `private`

Immutable descriptor represented by this resource.

```cpp
const InBinaryPackDescriptor* Descriptor_;
```

### `Resource`

**Classification:** PRIVATE IMPLEMENTATION · source access: `private`

Constructs a resource for one provider-owned descriptor table entry.

```cpp
explicit constexpr Resource(
                const InBinaryPackDescriptor* Descriptor
            ) noexcept :
                Descriptor_(Descriptor) {}
```

### `InBinaryPackSource`

**Classification:** PRIVATE IMPLEMENTATION · source access: `private`

Grants the owning Pack Source access to resource construction.

```cpp
friend class InBinaryPackSource<TByteOperationsProvider>;
```

### `Resource`

**Classification:** PUBLIC PROVIDER / EXTENSION API · source access: `public`

Copies a stable in-binary resource locator.

```cpp
constexpr Resource(const Resource&) noexcept = default;
```

### `operator`

**Classification:** PUBLIC PROVIDER / EXTENSION API · source access: `public`

Replaces a stable in-binary resource locator.

```cpp
constexpr Resource& operator=(const Resource&) noexcept = default;
```

### `Resource`

**Classification:** PUBLIC PROVIDER / EXTENSION API · source access: `public`

Destroys the stable descriptor locator.

```cpp
~Resource() = default;
```

### `Descriptors_`

**Classification:** PRIVATE IMPLEMENTATION · source access: `private`

First immutable pack descriptor.

```cpp
const InBinaryPackDescriptor* Descriptors_;
```

### `DescriptorCount_`

**Classification:** PRIVATE IMPLEMENTATION · source access: `private`

Number of immutable pack descriptors.

```cpp
std::size_t DescriptorCount_;
```

### `ByteOperations_`

**Classification:** PRIVATE IMPLEMENTATION · source access: `private`

Bootstrap-owned raw byte operations provider.

```cpp
const TByteOperationsProvider* ByteOperations_;
```

### `IsLanguageMatch`

**Classification:** PRIVATE IMPLEMENTATION · source access: `private`

Indicates whether Descriptor represents Language exactly.

```cpp
[[nodiscard]] bool IsLanguageMatch(
            const InBinaryPackDescriptor& Descriptor,
            LanguageIdentifierView Language
        ) const noexcept
```

### `PackResource`

**Classification:** PUBLIC PROVIDER / EXTENSION API · source access: `public`

Provider-associated resource type consumed by Pack Source operations.

```cpp
using PackResource = Resource;
```

### `InBinaryPackSource`

**Classification:** PUBLIC PROVIDER / EXTENSION API · source access: `public`

Binds one immutable descriptor table and the Bootstrap-owned ByteOperations provider.

```cpp
InBinaryPackSource(
            const InBinaryPackDescriptor* Descriptors,
            std::size_t DescriptorCount,
            const TByteOperationsProvider& ByteOperations
        ) noexcept :
            Descriptors_(Descriptors),
```

### `Locate`

**Classification:** PUBLIC PROVIDER / EXTENSION API · source access: `public`

Locates one immutable pack by canonical language identity.

```cpp
[[nodiscard]] PackLocateResult<PackResource> Locate(
            LanguageIdentifierView Language
        ) const noexcept
```

### `Size`

**Classification:** PUBLIC PROVIDER / EXTENSION API

Reports the complete immutable pack size.

```cpp
[[nodiscard]] PackSizeResult Size(
            const PackResource& ResourceValue
        ) const noexcept
```

### `Read`

**Classification:** PUBLIC PROVIDER / EXTENSION API

Copies exactly the requested immutable pack byte range.

```cpp
[[nodiscard]] PackReadResult Read(
            const PackResource& ResourceValue,
            std::uint64_t Offset,
            WritableByteView Destination
        ) const noexcept
```

### `LanguageIdentity`

**Classification:** PUBLIC PROVIDER / EXTENSION API

Returns the canonical language identity represented by ResourceValue.

```cpp
[[nodiscard]] LanguageIdentifierView LanguageIdentity(
            const PackResource& ResourceValue
        ) const noexcept
```

