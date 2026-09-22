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

        /// Constructs a view after successful validation.
        constexpr LanguageIdentifierView(
            const char* Data,
            std::uint8_t Length
        ) noexcept :
            Data_(Data),
            Length_(Length) {}


        // Syntax helpers.

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

        /// Validates one ordinary language-tag subtag as lowercase alphanumeric text.
        [[nodiscard]] static constexpr bool IsLowerAlphaNumericSubtag(
            const char* Data,
            std::size_t Size
        ) noexcept {
            if (Size == 0U || Size > 8U) { return false; }

            for (std::size_t Index = 0U; Index < Size; ++Index) {
                const char Value = Data[Index];

                if (IsDigit(Value)) { continue; }
                if (!IsLowerAlpha(Value)) { return false; }
            }

            return true;
        }

    public:

        /// Structured LanguageIdentifierView validation result.
        struct ValidationResult;

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

        /// Validates a canonical runtime BCP47 identity view.
        ///
        /// Runtime validation deliberately enforces the canonical syntax/case form needed by
        /// generated Localisation contracts. Registry-level alias/preferred-value canonicalisation
        /// remains a toolchain responsibility.
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

        std::size_t SegmentStart = 0U;
        std::size_t SegmentIndex = 0U;

        while (SegmentStart < Length) {
            std::size_t SegmentEnd = SegmentStart;

            while (
                SegmentEnd < Length &&
                Data[SegmentEnd] != '-'
            ) {
                const unsigned char Byte = static_cast<unsigned char>(Data[SegmentEnd]);

                if (Byte > 0x7FU || !IsAlphaNumeric(Data[SegmentEnd])) {
                    return {
                        LanguageIdentifierValidationStatus::InvalidSyntax,
                        EmptyValue,
                        false
                    };
                }

                ++SegmentEnd;
            }

            const std::size_t SegmentSize = SegmentEnd - SegmentStart;

            if (SegmentSize == 0U || SegmentSize > 8U) {
                return {
                    LanguageIdentifierValidationStatus::InvalidSyntax,
                    EmptyValue,
                    false
                };
            }

            if (SegmentIndex == 0U) {
                if (SegmentSize < 2U || SegmentSize > 8U) {
                    return {
                        LanguageIdentifierValidationStatus::InvalidSyntax,
                        EmptyValue,
                        false
                    };
                }

                for (std::size_t Index = SegmentStart; Index < SegmentEnd; ++Index) {
                    if (!IsAlpha(Data[Index])) {
                        return {
                            LanguageIdentifierValidationStatus::InvalidSyntax,
                            EmptyValue,
                            false
                        };
                    }

                    if (!IsLowerAlpha(Data[Index])) {
                        return {
                            LanguageIdentifierValidationStatus::NonCanonicalCase,
                            EmptyValue,
                            false
                        };
                    }
                }
            } else if (
                SegmentSize == 4U &&
                IsAlpha(Data[SegmentStart]) &&
                IsAlpha(Data[SegmentStart + 1U]) &&
                IsAlpha(Data[SegmentStart + 2U]) &&
                IsAlpha(Data[SegmentStart + 3U])
            ) {
                if (
                    !IsUpperAlpha(Data[SegmentStart]) ||
                    !IsLowerAlpha(Data[SegmentStart + 1U]) ||
                    !IsLowerAlpha(Data[SegmentStart + 2U]) ||
                    !IsLowerAlpha(Data[SegmentStart + 3U])
                ) {
                    return {
                        LanguageIdentifierValidationStatus::NonCanonicalCase,
                        EmptyValue,
                        false
                    };
                }
            } else if (
                SegmentSize == 2U &&
                IsAlpha(Data[SegmentStart]) &&
                IsAlpha(Data[SegmentStart + 1U])
            ) {
                if (
                    !IsUpperAlpha(Data[SegmentStart]) ||
                    !IsUpperAlpha(Data[SegmentStart + 1U])
                ) {
                    return {
                        LanguageIdentifierValidationStatus::NonCanonicalCase,
                        EmptyValue,
                        false
                    };
                }
            } else if (
                SegmentSize == 3U &&
                IsDigit(Data[SegmentStart]) &&
                IsDigit(Data[SegmentStart + 1U]) &&
                IsDigit(Data[SegmentStart + 2U])
            ) {
                // Canonical numeric region; no case rule applies.
            } else if (!IsLowerAlphaNumericSubtag(
                Data + SegmentStart,
                SegmentSize
            )) {
                return {
                    LanguageIdentifierValidationStatus::NonCanonicalCase,
                    EmptyValue,
                    false
                };
            }

            ++SegmentIndex;

            if (SegmentEnd == Length) { break; }

            SegmentStart = SegmentEnd + 1U;

            if (SegmentStart == Length) {
                return {
                    LanguageIdentifierValidationStatus::InvalidSyntax,
                    EmptyValue,
                    false
                };
            }
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
