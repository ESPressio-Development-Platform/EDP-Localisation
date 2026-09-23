# src/localisation/PackSourceContract.hpp

**Primary classification:** INTERNAL PROVIDER API

**Source baseline:** `702f9d2d4080d5548e186fa1fd3f4156e581939b`

[Open exact source](https://github.com/ESPressio-Development-Platform/EDP-Localisation/blob/702f9d2d4080d5548e186fa1fd3f4156e581939b/src/localisation/PackSourceContract.hpp)

## Direct includes

- `concepts`
- `cstddef`
- `cstdint`
- `optional`
- `type_traits`
- `utility`
- `LanguageIdentifierView.hpp`
- `LocalisationComposition.hpp`
- `LocalisationTypes.hpp`

## Documented declarations

### `PackLocateStatus`

**Classification:** INTERNAL PROVIDER API

Mutually exclusive outcome of locating one immutable Localisation pack.

```cpp
enum class PackLocateStatus : std::uint8_t
```

### `PackSizeStatus`

**Classification:** INTERNAL PROVIDER API

Mutually exclusive outcome of querying one located pack's current size.

```cpp
enum class PackSizeStatus : std::uint8_t
```

### `PackReadStatus`

**Classification:** INTERNAL PROVIDER API

Mutually exclusive outcome of reading one exact bounded pack range.

```cpp
enum class PackReadStatus : std::uint8_t
```

### `TPackResource`

**Classification:** INTERNAL PROVIDER API

Locate result containing a resource only on success.

- **Template parameter `TPackResource`:** Provider-associated stable resource type.

```cpp
template<class TPackResource>
    struct PackLocateResult final
```

### `Status`

**Classification:** INTERNAL PROVIDER API · source access: `public`

Locate outcome.

```cpp
PackLocateStatus Status;
```

### `Resource`

**Classification:** INTERNAL PROVIDER API · source access: `public`

Located resource when Status is Success.

```cpp
std::optional<TPackResource> Resource;
```

### `PackSizeResult`

**Classification:** INTERNAL PROVIDER API

Current size of one previously located pack resource.

```cpp
struct PackSizeResult final
```

### `Status`

**Classification:** INTERNAL PROVIDER API · source access: `public`

Size-query outcome.

```cpp
PackSizeStatus Status;
```

### `SizeBytes`

**Classification:** INTERNAL PROVIDER API · source access: `public`

Current complete resource size on Success; zero otherwise.

```cpp
std::uint64_t SizeBytes;
```

### `PackReadResult`

**Classification:** INTERNAL PROVIDER API

Exact-read result for one bounded pack range.

```cpp
struct PackReadResult final
```

### `Status`

**Classification:** INTERNAL PROVIDER API · source access: `public`

Read outcome.

```cpp
PackReadStatus Status;
```

### `BytesRead`

**Classification:** INTERNAL PROVIDER API · source access: `public`

Number of destination bytes written before return.

```cpp
std::size_t BytesRead;
```

### `TPackSource`

**Classification:** INTERNAL PROVIDER API

Validates the public operation shape required from a Localisation Pack Source.

- **Template parameter `TPackSource`:** Candidate provider type.

```cpp
template<class TPackSource>
    concept PackSourceProvider =
        requires(
            const TPackSource& Source,
            LanguageIdentifierView Language,
            const typename TPackSource::PackResource& Resource,
            std::uint64_t Offset,
            WritableByteView Destination
        )
```

### `TPackSource`

**Classification:** INTERNAL PROVIDER API

Emits focused compile-time diagnostics for one candidate Pack Source provider.

- **Template parameter `TPackSource`:** Candidate provider type.

```cpp
template<class TPackSource>
    constexpr void ValidatePackSourceProvider() noexcept
```

