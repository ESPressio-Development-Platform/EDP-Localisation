#pragma once

#include <cstddef>
#include <cstdint>

namespace ESPressio::Localisation {

    enum class LanguageIdentifierValidationStatus : std::uint8_t {
        Succeeded = 0U,
        Empty = 1U,
        TooLong = 2U,
        NullData = 3U,
        InvalidSyntax = 4U,
        NonCanonicalCase = 5U
    };


    /// Non-owning view over one canonical ASCII BCP47 language identity.
    class LanguageIdentifierView final {
    private:

        // Referenced canonical identity.

        /// First canonical BCP47 byte.
        const char* Data_;

        /// Exact canonical BCP47 byte count.
        std::uint8_t Length_;


        // Construction.

        /// Constructs a view after successful validation.
        constexpr LanguageIdentifierView(
            const char* Data,
            std::uint8_t Length
        ) noexcept :
            Data_(Data),
            Length_(Length) {}


        // ASCII predicates.

        /// Indicates whether Value is an ASCII alphabetic character.
        [[nodiscard]] static constexpr bool IsAlpha(char Value) noexcept {
            return
                (Value >= 'A' && Value <= 'Z') ||
                (Value >= 'a' && Value <= 'z');
        }

        /// Indicates whether Value is an ASCII decimal digit.
        [[nodiscard]] static constexpr bool IsDigit(char Value) noexcept {
            return Value >= '0' && Value <= '9';
        }

        /// Indicates whether Value is an ASCII alphanumeric character.
        [[nodiscard]] static constexpr bool IsAlphaNumeric(char Value) noexcept {
            return IsAlpha(Value) || IsDigit(Value);
        }

        /// Indicates whether Value is lowercase ASCII alphabetic.
        [[nodiscard]] static constexpr bool IsLowerAlpha(char Value) noexcept {
            return Value >= 'a' && Value <= 'z';
        }

        /// Indicates whether Value is uppercase ASCII alphabetic.
        [[nodiscard]] static constexpr bool IsUpperAlpha(char Value) noexcept {
            return Value >= 'A' && Value <= 'Z';
        }


        // Subtag scanning.

        /// Returns the first hyphen or end position after Start.
        [[nodiscard]] static constexpr std::size_t FindSubtagEnd(
            const char* Data,
            std::size_t Length,
            std::size_t Start
        ) noexcept {
            std::size_t End = Start;

            while (
                End < Length &&
                Data[End] != '-'
            ) {
                ++End;
            }

            return End;
        }

        /// Indicates whether the supplied range contains only ASCII alphabetic characters.
        [[nodiscard]] static constexpr bool IsAlphaRange(
            const char* Data,
            std::size_t Start,
            std::size_t End
        ) noexcept {
            if (Start == End) { return false; }

            for (std::size_t Index = Start; Index < End; ++Index) {
                if (!IsAlpha(Data[Index])) { return false; }
            }

            return true;
        }

        /// Indicates whether the supplied range contains only ASCII decimal digits.
        [[nodiscard]] static constexpr bool IsDigitRange(
            const char* Data,
            std::size_t Start,
            std::size_t End
        ) noexcept {
            if (Start == End) { return false; }

            for (std::size_t Index = Start; Index < End; ++Index) {
                if (!IsDigit(Data[Index])) { return false; }
            }

            return true;
        }

        /// Indicates whether the supplied range is lowercase canonical ASCII alphanumeric text.
        [[nodiscard]] static constexpr bool IsLowerAlphaNumericRange(
            const char* Data,
            std::size_t Start,
            std::size_t End
        ) noexcept {
            if (Start == End) { return false; }

            for (std::size_t Index = Start; Index < End; ++Index) {
                const char Value = Data[Index];

                if (IsDigit(Value)) { continue; }
                if (!IsLowerAlpha(Value)) { return false; }
            }

            return true;
        }

        /// Indicates whether the supplied range is lowercase canonical ASCII alphabetic text.
        [[nodiscard]] static constexpr bool IsLowerAlphaRange(
            const char* Data,
            std::size_t Start,
            std::size_t End
        ) noexcept {
            if (!IsAlphaRange(
                Data,
                Start,
                End
            )) {
                return false;
            }

            for (std::size_t Index = Start; Index < End; ++Index) {
                if (!IsLowerAlpha(Data[Index])) { return false; }
            }

            return true;
        }

        /// Indicates whether a four-character script subtag has canonical title case.
        [[nodiscard]] static constexpr bool IsCanonicalScript(
            const char* Data,
            std::size_t Start,
            std::size_t End
        ) noexcept {
            return
                End - Start == 4U &&
                IsUpperAlpha(Data[Start]) &&
                IsLowerAlpha(Data[Start + 1U]) &&
                IsLowerAlpha(Data[Start + 2U]) &&
                IsLowerAlpha(Data[Start + 3U]);
        }

        /// Indicates whether one subtag satisfies the BCP47 variant grammar.
        [[nodiscard]] static constexpr bool IsVariant(
            const char* Data,
            std::size_t Start,
            std::size_t End
        ) noexcept {
            const std::size_t Size = End - Start;

            if (Size >= 5U && Size <= 8U) {
                return IsLowerAlphaNumericRange(
                    Data,
                    Start,
                    End
                );
            }

            if (
                Size == 4U &&
                IsDigit(Data[Start])
            ) {
                return IsLowerAlphaNumericRange(
                    Data,
                    Start,
                    End
                );
            }

            return false;
        }

        /// Returns an ordering value for one canonical extension singleton.
        [[nodiscard]] static constexpr std::uint8_t SingletonOrder(char Value) noexcept {
            if (IsDigit(Value)) {
                return static_cast<std::uint8_t>(Value - '0');
            }

            return static_cast<std::uint8_t>(
                10U + static_cast<std::uint8_t>(Value - 'a')
            );
        }

    public:

        /// Structured LanguageIdentifierView validation result.
        struct ValidationResult;


        // Identity access.

        /// Returns the first referenced canonical BCP47 byte.
        [[nodiscard]] constexpr const char* Data() const noexcept {
            return Data_;
        }

        /// Returns the exact referenced byte count.
        [[nodiscard]] constexpr std::uint8_t Length() const noexcept {
            return Length_;
        }

        /// Indicates whether this view and Other contain exactly the same canonical bytes.
        [[nodiscard]] constexpr bool IsEqualTo(
            const LanguageIdentifierView& Other
        ) const noexcept {
            if (Length_ != Other.Length_) { return false; }

            for (std::size_t Index = 0U; Index < Length_; ++Index) {
                if (Data_[Index] != Other.Data_[Index]) { return false; }
            }

            return true;
        }


        // Validation.

        /// Validates a canonical runtime BCP47 identity view.
        ///
        /// Runtime validation enforces the bounded RFC 5646 structural grammar and canonical
        /// casing used by generated Localisation contracts. Registry alias/preferred-value
        /// canonicalisation remains a compiler/toolchain responsibility.
        [[nodiscard]] static constexpr ValidationResult Validate(
            const char* Data,
            std::size_t Length
        ) noexcept;

        /// Validates a null-terminated language-tag literal.
        ///
        /// @tparam TSize Compile-time literal extent including the terminator.
        template<std::size_t TSize>
        [[nodiscard]] static constexpr ValidationResult Validate(
            const char (&Value)[TSize]
        ) noexcept;

    };


    struct LanguageIdentifierView::ValidationResult final {

        /// Validation outcome.
        LanguageIdentifierValidationStatus Status;

        /// Canonical view when IsValuePresent is true.
        LanguageIdentifierView Value;

        /// Indicates whether Value contains a successfully validated identity.
        bool IsValuePresent;

    };


    [[nodiscard]] constexpr LanguageIdentifierView::ValidationResult LanguageIdentifierView::Validate(
        const char* Data,
        std::size_t Length
    ) noexcept {
        const LanguageIdentifierView EmptyValue(
            nullptr,
            0U
        );

        if (Length == 0U) {
            return {
                LanguageIdentifierValidationStatus::Empty,
                EmptyValue,
                false
            };
        }

        if (Length > 255U) {
            return {
                LanguageIdentifierValidationStatus::TooLong,
                EmptyValue,
                false
            };
        }

        if (Data == nullptr) {
            return {
                LanguageIdentifierValidationStatus::NullData,
                EmptyValue,
                false
            };
        }

        for (std::size_t Index = 0U; Index < Length; ++Index) {
            const unsigned char Byte = static_cast<unsigned char>(Data[Index]);

            if (
                Byte > 0x7FU ||
                (
                    Data[Index] != '-' &&
                    !IsAlphaNumeric(Data[Index])
                )
            ) {
                return {
                    LanguageIdentifierValidationStatus::InvalidSyntax,
                    EmptyValue,
                    false
                };
            }
        }

        std::size_t Cursor = 0U;
        std::size_t End = FindSubtagEnd(
            Data,
            Length,
            Cursor
        );
        const std::size_t PrimarySize = End - Cursor;

        // Private-use-only tags are valid BCP47 identities and remain lowercase.
        if (
            PrimarySize == 1U &&
            Data[Cursor] == 'x'
        ) {
            Cursor = End + 1U;

            if (Cursor >= Length) {
                return {
                    LanguageIdentifierValidationStatus::InvalidSyntax,
                    EmptyValue,
                    false
                };
            }

            while (Cursor < Length) {
                End = FindSubtagEnd(
                    Data,
                    Length,
                    Cursor
                );
                const std::size_t Size = End - Cursor;

                if (
                    Size == 0U ||
                    Size > 8U ||
                    !IsLowerAlphaNumericRange(
                        Data,
                        Cursor,
                        End
                    )
                ) {
                    return {
                        LanguageIdentifierValidationStatus::InvalidSyntax,
                        EmptyValue,
                        false
                    };
                }

                Cursor = End == Length
                    ? Length
                    : End + 1U;
            }

            return {
                LanguageIdentifierValidationStatus::Succeeded,
                LanguageIdentifierView(
                    Data,
                    static_cast<std::uint8_t>(Length)
                ),
                true
            };
        }

        if (
            PrimarySize < 2U ||
            PrimarySize > 8U ||
            !IsAlphaRange(
                Data,
                Cursor,
                End
            )
        ) {
            return {
                LanguageIdentifierValidationStatus::InvalidSyntax,
                EmptyValue,
                false
            };
        }

        if (!IsLowerAlphaRange(
            Data,
            Cursor,
            End
        )) {
            return {
                LanguageIdentifierValidationStatus::NonCanonicalCase,
                EmptyValue,
                false
            };
        }

        Cursor = End == Length
            ? Length
            : End + 1U;

        // A 2-3 character primary language may carry up to three extlang subtags.
        if (PrimarySize <= 3U) {
            std::size_t ExtlangCount = 0U;

            while (
                Cursor < Length &&
                ExtlangCount < 3U
            ) {
                End = FindSubtagEnd(
                    Data,
                    Length,
                    Cursor
                );

                if (
                    End - Cursor != 3U ||
                    !IsAlphaRange(
                        Data,
                        Cursor,
                        End
                    )
                ) {
                    break;
                }

                if (!IsLowerAlphaRange(
                    Data,
                    Cursor,
                    End
                )) {
                    return {
                        LanguageIdentifierValidationStatus::NonCanonicalCase,
                        EmptyValue,
                        false
                    };
                }

                ++ExtlangCount;
                Cursor = End == Length
                    ? Length
                    : End + 1U;
            }
        }

        // Optional script.
        if (Cursor < Length) {
            End = FindSubtagEnd(
                Data,
                Length,
                Cursor
            );

            if (
                End - Cursor == 4U &&
                IsAlphaRange(
                    Data,
                    Cursor,
                    End
                )
            ) {
                if (!IsCanonicalScript(
                    Data,
                    Cursor,
                    End
                )) {
                    return {
                        LanguageIdentifierValidationStatus::NonCanonicalCase,
                        EmptyValue,
                        false
                    };
                }

                Cursor = End == Length
                    ? Length
                    : End + 1U;
            }
        }

        // Optional region.
        if (Cursor < Length) {
            End = FindSubtagEnd(
                Data,
                Length,
                Cursor
            );
            const std::size_t RegionSize = End - Cursor;

            if (
                RegionSize == 2U &&
                IsAlphaRange(
                    Data,
                    Cursor,
                    End
                )
            ) {
                if (
                    !IsUpperAlpha(Data[Cursor]) ||
                    !IsUpperAlpha(Data[Cursor + 1U])
                ) {
                    return {
                        LanguageIdentifierValidationStatus::NonCanonicalCase,
                        EmptyValue,
                        false
                    };
                }

                Cursor = End == Length
                    ? Length
                    : End + 1U;
            } else if (
                RegionSize == 3U &&
                IsDigitRange(
                    Data,
                    Cursor,
                    End
                )
            ) {
                Cursor = End == Length
                    ? Length
                    : End + 1U;
            }
        }

        // Zero or more variants.
        while (Cursor < Length) {
            End = FindSubtagEnd(
                Data,
                Length,
                Cursor
            );

            if (!IsVariant(
                Data,
                Cursor,
                End
            )) {
                break;
            }

            Cursor = End == Length
                ? Length
                : End + 1U;
        }

        // Zero or more ordered extension sequences.
        std::uint8_t PreviousSingletonOrder = 0U;
        bool HasPreviousSingleton = false;

        while (Cursor < Length) {
            End = FindSubtagEnd(
                Data,
                Length,
                Cursor
            );

            if (End - Cursor != 1U) { break; }

            const char Singleton = Data[Cursor];

            if (
                Singleton == 'x' ||
                !IsLowerAlphaNumericRange(
                    Data,
                    Cursor,
                    End
                )
            ) {
                break;
            }

            const std::uint8_t CurrentOrder = SingletonOrder(Singleton);

            if (
                HasPreviousSingleton &&
                CurrentOrder <= PreviousSingletonOrder
            ) {
                return {
                    LanguageIdentifierValidationStatus::InvalidSyntax,
                    EmptyValue,
                    false
                };
            }

            PreviousSingletonOrder = CurrentOrder;
            HasPreviousSingleton = true;
            Cursor = End + 1U;

            if (Cursor >= Length) {
                return {
                    LanguageIdentifierValidationStatus::InvalidSyntax,
                    EmptyValue,
                    false
                };
            }

            std::size_t ExtensionSubtagCount = 0U;

            while (Cursor < Length) {
                End = FindSubtagEnd(
                    Data,
                    Length,
                    Cursor
                );
                const std::size_t Size = End - Cursor;

                if (Size == 1U) { break; }

                if (
                    Size < 2U ||
                    Size > 8U ||
                    !IsLowerAlphaNumericRange(
                        Data,
                        Cursor,
                        End
                    )
                ) {
                    return {
                        LanguageIdentifierValidationStatus::InvalidSyntax,
                        EmptyValue,
                        false
                    };
                }

                ++ExtensionSubtagCount;
                Cursor = End == Length
                    ? Length
                    : End + 1U;
            }

            if (ExtensionSubtagCount == 0U) {
                return {
                    LanguageIdentifierValidationStatus::InvalidSyntax,
                    EmptyValue,
                    false
                };
            }
        }

        // Optional terminal private-use sequence.
        if (Cursor < Length) {
            End = FindSubtagEnd(
                Data,
                Length,
                Cursor
            );

            if (
                End - Cursor != 1U ||
                Data[Cursor] != 'x'
            ) {
                return {
                    LanguageIdentifierValidationStatus::InvalidSyntax,
                    EmptyValue,
                    false
                };
            }

            Cursor = End + 1U;

            if (Cursor >= Length) {
                return {
                    LanguageIdentifierValidationStatus::InvalidSyntax,
                    EmptyValue,
                    false
                };
            }

            while (Cursor < Length) {
                End = FindSubtagEnd(
                    Data,
                    Length,
                    Cursor
                );
                const std::size_t Size = End - Cursor;

                if (
                    Size == 0U ||
                    Size > 8U ||
                    !IsLowerAlphaNumericRange(
                        Data,
                        Cursor,
                        End
                    )
                ) {
                    return {
                        LanguageIdentifierValidationStatus::InvalidSyntax,
                        EmptyValue,
                        false
                    };
                }

                Cursor = End == Length
                    ? Length
                    : End + 1U;
            }
        }

        if (Cursor != Length) {
            return {
                LanguageIdentifierValidationStatus::InvalidSyntax,
                EmptyValue,
                false
            };
        }

        return {
            LanguageIdentifierValidationStatus::Succeeded,
            LanguageIdentifierView(
                Data,
                static_cast<std::uint8_t>(Length)
            ),
            true
        };
    }


    template<std::size_t TSize>
    [[nodiscard]] constexpr LanguageIdentifierView::ValidationResult LanguageIdentifierView::Validate(
        const char (&Value)[TSize]
    ) noexcept {
        static_assert(
            TSize > 0U,
            "LanguageIdentifierView literal extent must include a terminator"
        );

        return Validate(
            Value,
            TSize - 1U
        );
    }

} // ESPressio::Localisation
