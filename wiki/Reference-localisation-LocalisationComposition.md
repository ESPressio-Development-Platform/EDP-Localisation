# src/localisation/LocalisationComposition.hpp

**Primary classification:** PUBLIC API

**Source baseline:** `702f9d2d4080d5548e186fa1fd3f4156e581939b`

[Open exact source](https://github.com/ESPressio-Development-Platform/EDP-Localisation/blob/702f9d2d4080d5548e186fa1fd3f4156e581939b/src/localisation/LocalisationComposition.hpp)

## Direct includes

- `ESPressio_System.hpp`
- `memory/MemoryComposition.hpp`

## Documented declarations

### `Framework`

**Classification:** PUBLIC API

Short name for the EDP-System compile-time Composition Framework.

```cpp
namespace Framework = ESPressio::System::CompositionFramework;
```

### `Domain`

**Classification:** PUBLIC API

Composition domain containing Localisation capabilities.

```cpp
struct Domain final : Framework::Domain {};
```

### `PackSource`

**Classification:** PUBLIC API · source access: `public`

Shared capability supplying immutable compiled Localisation pack bytes.

```cpp
struct PackSource final : Framework::SharedCapability<Domain> {};
```

### `PackSourceRequirement`

**Classification:** PUBLIC API · source access: `public`

Standalone consumer Requirement for exactly one Localisation PackSource provider.

```cpp
using PackSourceRequirement = Framework::Requirement<
        PackSource,
```

### `ExternalByteOperationsRequirement`

**Classification:** PUBLIC API · source access: `public`

Provider Contract Requirement for exactly one external EDP-Memory ByteOperations provider.

```cpp
using ExternalByteOperationsRequirement = Framework::Requirement<
        ESPressio::Memory::ByteOperations,
```

### `ByteOperationsRequirement`

**Classification:** PUBLIC API · source access: `public`

Standalone consumer Requirement for exactly one EDP-Memory ByteOperations provider.

```cpp
using ByteOperationsRequirement = Framework::Requirement<
        ESPressio::Memory::ByteOperations,
```

### `ResolverContract`

**Classification:** PUBLIC API · source access: `public`

Complete compile-time dependency contract of one Resolver consumer.

Runtime provider ownership remains explicit: Bootstrap still constructs and binds the
selected PackSource and ByteOperations instances to Resolver.

```cpp
using ResolverContract = Framework::Contract<
        PackSourceRequirement,
```

