# src/localisation/LanguageIdentifierView.hpp

**Primary classification:** PUBLIC API

**Source baseline:** `702f9d2d4080d5548e186fa1fd3f4156e581939b`

[Open exact source](https://github.com/ESPressio-Development-Platform/EDP-Localisation/blob/702f9d2d4080d5548e186fa1fd3f4156e581939b/src/localisation/LanguageIdentifierView.hpp)

## Direct includes

- `cstddef`
- `cstdint`

## Documented declarations

### `LanguageIdentifierValidationStatus`

**Classification:** PUBLIC API

Mutually exclusive outcome of validating one canonical BCP47 language identity.

```cpp
enum class LanguageIdentifierValidationStatus : std::uint8_t
```

### `LanguageIdentifierView`

**Classification:** PUBLIC API

Non-owning view over one canonical ASCII BCP47 language identity.

```cpp
class LanguageIdentifierView final
```

### `Data_`

**Classification:** PRIVATE IMPLEMENTATION · source access: `private`

First canonical BCP47 byte.

```cpp
const char* Data_;
```

### `Length_`

**Classification:** PRIVATE IMPLEMENTATION · source access: `private`

Exact canonical BCP47 byte count.

```cpp
std::uint8_t Length_;
```

### `LanguageIdentifierView`

**Classification:** PRIVATE IMPLEMENTATION · source access: `private`

Constructs a view after successful validation.

```cpp
constexpr LanguageIdentifierView(
            const char* Data,
            std::uint8_t Length
        ) noexcept :
            Data_(Data),
```

### `IsAlpha`

**Classification:** PRIVATE IMPLEMENTATION · source access: `private`

Indicates whether Value is an ASCII alphabetic character.

```cpp
[[nodiscard]] static constexpr bool IsAlpha(char Value) noexcept
```

### `IsDigit`

**Classification:** PRIVATE IMPLEMENTATION · source access: `private`

Indicates whether Value is an ASCII decimal digit.

```cpp
[[nodiscard]] static constexpr bool IsDigit(char Value) noexcept
```

### `IsAlphaNumeric`

**Classification:** PRIVATE IMPLEMENTATION · source access: `private`

Indicates whether Value is an ASCII alphanumeric character.

```cpp
[[nodiscard]] static constexpr bool IsAlphaNumeric(char Value) noexcept
```

### `IsLowerAlpha`

**Classification:** PRIVATE IMPLEMENTATION · source access: `private`

Indicates whether Value is lowercase ASCII alphabetic.

```cpp
[[nodiscard]] static constexpr bool IsLowerAlpha(char Value) noexcept
```

### `IsUpperAlpha`

**Classification:** PRIVATE IMPLEMENTATION · source access: `private`

Indicates whether Value is uppercase ASCII alphabetic.

```cpp
[[nodiscard]] static constexpr bool IsUpperAlpha(char Value) noexcept
```

### `FindSubtagEnd`

**Classification:** PRIVATE IMPLEMENTATION · source access: `private`

Returns the first hyphen or end position after Start.

```cpp
[[nodiscard]] static constexpr std::size_t FindSubtagEnd(
            const char* Data,
            std::size_t Length,
            std::size_t Start
        ) noexcept
```

### `IsAlphaRange`

**Classification:** PRIVATE IMPLEMENTATION · source access: `private`

Indicates whether the supplied range contains only ASCII alphabetic characters.

```cpp
[[nodiscard]] static constexpr bool IsAlphaRange(
            const char* Data,
            std::size_t Start,
            std::size_t End
        ) noexcept
```

### `IsDigitRange`

**Classification:** PRIVATE IMPLEMENTATION · source access: `private`

Indicates whether the supplied range contains only ASCII decimal digits.

```cpp
[[nodiscard]] static constexpr bool IsDigitRange(
            const char* Data,
            std::size_t Start,
            std::size_t End
        ) noexcept
```

### `IsLowerAlphaNumericRange`

**Classification:** PRIVATE IMPLEMENTATION · source access: `private`

Indicates whether the supplied range is lowercase canonical ASCII alphanumeric text.

```cpp
[[nodiscard]] static constexpr bool IsLowerAlphaNumericRange(
            const char* Data,
            std::size_t Start,
            std::size_t End
        ) noexcept
```

### `IsLowerAlphaRange`

**Classification:** PRIVATE IMPLEMENTATION · source access: `private`

Indicates whether the supplied range is lowercase canonical ASCII alphabetic text.

```cpp
[[nodiscard]] static constexpr bool IsLowerAlphaRange(
            const char* Data,
            std::size_t Start,
            std::size_t End
        ) noexcept
```

### `IsCanonicalScript`

**Classification:** PRIVATE IMPLEMENTATION · source access: `private`

Indicates whether a four-character script subtag has canonical title case.

```cpp
[[nodiscard]] static constexpr bool IsCanonicalScript(
            const char* Data,
            std::size_t Start,
            std::size_t End
        ) noexcept
```

### `IsVariant`

**Classification:** PRIVATE IMPLEMENTATION · source access: `private`

Indicates whether one subtag satisfies the BCP47 variant grammar.

```cpp
[[nodiscard]] static constexpr bool IsVariant(
            const char* Data,
            std::size_t Start,
            std::size_t End
        ) noexcept
```

### `SingletonOrder`

**Classification:** PRIVATE IMPLEMENTATION · source access: `private`

Returns an ordering value for one canonical extension singleton.

```cpp
[[nodiscard]] static constexpr std::uint8_t SingletonOrder(char Value) noexcept
```

### `ValidationResult`

**Classification:** PUBLIC API · source access: `public`

Structured LanguageIdentifierView validation result.

```cpp
struct ValidationResult;
```

### `Data`

**Classification:** PUBLIC API · source access: `public`

Returns the first referenced canonical BCP47 byte.

```cpp
[[nodiscard]] constexpr const char* Data() const noexcept
```

### `Length`

**Classification:** PUBLIC API · source access: `public`

Returns the exact referenced byte count.

```cpp
[[nodiscard]] constexpr std::uint8_t Length() const noexcept
```

### `IsEqualTo`

**Classification:** PUBLIC API · source access: `public`

Indicates whether this view and Other contain exactly the same canonical bytes.

```cpp
[[nodiscard]] constexpr bool IsEqualTo(
            const LanguageIdentifierView& Other
        ) const noexcept
```

### `Validate`

**Classification:** PUBLIC API · source access: `public`

Validates a canonical runtime BCP47 identity view.

Runtime validation enforces the bounded RFC 5646 structural grammar and canonical
casing used by generated Localisation contracts. Registry alias/preferred-value
canonicalisation remains a compiler/toolchain responsibility.

```cpp
[[nodiscard]] static constexpr ValidationResult Validate(
            const char* Data,
            std::size_t Length
        ) noexcept;
```

### `Validate`

**Classification:** PUBLIC API · source access: `public`

Validates a null-terminated language-tag literal.

- **Template parameter `TSize`:** Compile-time literal extent including the terminator.

```cpp
template<std::size_t TSize>
        [[nodiscard]] static constexpr ValidationResult Validate(
            const char (&Value)[TSize]
        ) noexcept;
```

### `Status`

**Classification:** PUBLIC API · source access: `public`

Validation outcome.

```cpp
LanguageIdentifierValidationStatus Status;
```

### `Value`

**Classification:** PUBLIC API · source access: `public`

Canonical view when IsValuePresent is true.

```cpp
LanguageIdentifierView Value;
```

### `IsValuePresent`

**Classification:** PUBLIC API · source access: `public`

Indicates whether Value contains a successfully validated identity.

```cpp
bool IsValuePresent;
```

