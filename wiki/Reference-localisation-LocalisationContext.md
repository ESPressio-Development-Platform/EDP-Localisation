# src/localisation/LocalisationContext.hpp

**Primary classification:** PUBLIC API

**Source baseline:** `702f9d2d4080d5548e186fa1fd3f4156e581939b`

[Open exact source](https://github.com/ESPressio-Development-Platform/EDP-Localisation/blob/702f9d2d4080d5548e186fa1fd3f4156e581939b/src/localisation/LocalisationContext.hpp)

## Direct includes

- `cstdint`
- `LanguageIdentifierView.hpp`

## Documented declarations

### `LocalisationContextValidationStatus`

**Classification:** PUBLIC API

Mutually exclusive outcome of validating one caller-owned Localisation context.

```cpp
enum class LocalisationContextValidationStatus : std::uint8_t
```

### `LocalisationContext`

**Classification:** PUBLIC API

Caller-owned language policy supplied independently to each Localisation operation.

```cpp
struct LocalisationContext final
```

### `RequestedLanguage`

**Classification:** PUBLIC API · source access: `public`

Canonical language requested for the current operation.

```cpp
LanguageIdentifierView RequestedLanguage;
```

### `TerminalLanguage`

**Classification:** PUBLIC API · source access: `public`

Canonical unique terminal fallback language for the ContractFamily.

```cpp
LanguageIdentifierView TerminalLanguage;
```

### `LocalisationContextValidationResult`

**Classification:** PUBLIC API

Result of validating one LocalisationContext's language identities.

```cpp
struct LocalisationContextValidationResult final
```

### `Status`

**Classification:** PUBLIC API · source access: `public`

Validation outcome.

```cpp
LocalisationContextValidationStatus Status;
```

### `ValidateLocalisationContext`

**Classification:** PUBLIC API

Validates the two canonical language identities carried by Context.

```cpp
[[nodiscard]] constexpr LocalisationContextValidationResult ValidateLocalisationContext(
        const LocalisationContext& Context
    ) noexcept
```

