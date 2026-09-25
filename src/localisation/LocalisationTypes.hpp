#pragma once

#include <array>
#include <cstddef>
#include <cstdint>
#include <type_traits>

#include <ESPressio_System.hpp>

namespace ESPressio::Localisation {

    /// Mutually exclusive outcome of one Localisation resolution operation.
    enum class LocalisationStatus : std::uint8_t {
        Success = 0U,
        NoStringFoundForIdentifier = 1U,
        LanguagePackUnavailable = 2U,
        ProviderUnavailable = 3U,
        UnsupportedFormatVersion = 4U,
        IncompatibleLanguagePack = 5U,
        InvalidDataset = 6U,
        ReadFailure = 7U,
        InvalidArgument = 8U
    };

    /// Orthogonal facts reported alongside a successful Localisation resolution.
    enum class LocalisationFact : std::uint8_t {
        LanguageFallbackUsed = 0U,
        BufferTooSmall = 1U
    };

    /// Mutually exclusive outcome of materialising one resolved textual identity.
    enum class TextMaterialisationStatus : std::uint8_t {
        Success = 0U,
        InvalidArgument = 1U
    };

    /// Orthogonal facts reported alongside successful text materialisation.
    enum class TextMaterialisationFact : std::uint8_t {
        BufferTooSmall = 0U
    };

    /// Caller-selected representation mode for UTF-8 output buffers.
    enum class TextOutputMode : std::uint8_t {
        RawUtf8 = 0U,
        NullTerminatedUtf8 = 1U
    };

    /// Mutually exclusive outcome of explicit pack/context validation.
    enum class ValidationStatus : std::uint8_t {
        Success = 0U,
        LanguagePackUnavailable = 1U,
        ProviderUnavailable = 2U,
        IncompatibleLanguagePack = 3U,
        InvalidDataset = 4U,
        ReadFailure = 5U,
        InvalidArgument = 6U
    };


    /// Compact typed successful-resolution facts.
    using LocalisationFacts = ESPressio::System::FlagSet<
        LocalisationFact,
        std::uint8_t
    >;

    /// Compact typed successful text-materialisation facts.
    using TextMaterialisationFacts = ESPressio::System::FlagSet<
        TextMaterialisationFact,
        std::uint8_t
    >;


    /// Result of materialising one already-resolved canonical language identity.
    struct TextMaterialisationResult final {

        /// Materialisation outcome.
        TextMaterialisationStatus Status;

        /// Orthogonal successful materialisation facts.
        TextMaterialisationFacts Facts;

        /// Number of UTF-8 payload bytes written, excluding a NUL terminator.
        std::size_t BytesWritten;

        /// Complete UTF-8 payload byte count required, excluding a NUL terminator.
        std::size_t RequiredBytes;

    };


    /// Result of explicit pack or context validation.
    struct ValidationResult final {

        /// Validation outcome.
        ValidationStatus Status;

    };


    /// Describes caller-owned writable text storage.
    struct WritableTextView final {

        /// First writable byte, or null only when Capacity is zero.
        char* Data;

        /// Number of writable bytes available from Data.
        std::size_t Capacity;

    };


    /// Describes caller-owned writable opaque byte storage.
    struct WritableByteView final {

        /// First writable byte, or null only when Size is zero.
        std::uint8_t* Data;

        /// Number of writable bytes available from Data.
        std::size_t Size;

    };


    /// Fixed canonical width of every EDP schema Type identity.
    ///
    /// This width is platform-wide so ESPressio libraries, applications and third-party
    /// ESPressio-compatible libraries can exchange the same Type identities without
    /// ContractFamily-specific width negotiation.
    inline constexpr std::size_t TypeIdentifierBytes =
        ESPressio::System::TypeIdentifier::Size;

    /// Fixed canonical width of every EDP schema Field identity.
    ///
    /// Field identity is local to an owning Type and uses the universal one-byte
    /// System::FieldIdentifier contract. This is not a ContractFamily tuning parameter.
    inline constexpr std::size_t FieldIdentifierBytes =
        ESPressio::System::FieldIdentifier::Size;


    namespace Detail {

        /// Selects an unsigned integer storage type from one supported byte width.
        ///
        /// @tparam TBytes Exact unsigned storage width in bytes.
        template<std::size_t TBytes>
        struct UnsignedStorageForBytes;

        /// One-byte unsigned identifier storage.
        template<>
        struct UnsignedStorageForBytes<1U> final {

            /// Unsigned integer type occupying one byte.
            using Type = std::uint8_t;

        };

        /// Two-byte unsigned identifier storage.
        template<>
        struct UnsignedStorageForBytes<2U> final {

            /// Unsigned integer type occupying two bytes.
            using Type = std::uint16_t;

        };

        /// Four-byte unsigned identifier storage.
        template<>
        struct UnsignedStorageForBytes<4U> final {

            /// Unsigned integer type occupying four bytes.
            using Type = std::uint32_t;

        };

        /// Eight-byte unsigned identifier storage.
        template<>
        struct UnsignedStorageForBytes<8U> final {

            /// Unsigned integer type occupying eight bytes.
            using Type = std::uint64_t;

        };

        /// Exact unsigned integer type selected by TBytes.
        ///
        /// @tparam TBytes Exact unsigned storage width in bytes.
        template<std::size_t TBytes>
        using UnsignedStorageForBytesType = typename UnsignedStorageForBytes<TBytes>::Type;


        /// Semantic tag for Domain identifiers.
        struct DomainIdentifierTag final {};

        /// Semantic tag for SubDomain identifiers.
        struct SubDomainIdentifierTag final {};

        /// Semantic tag for general String identifiers.
        struct StringIdentifierTag final {};

        /// Semantic tag for an unavailable globally unique Type identifier universe.
        struct TypeIdentifierTag final {};

        /// Semantic tag used only for the unavailable Field-identifier sentinel.
        struct FieldIdentifierTag final {};

    } // ESPressio::Localisation::Detail


    /// Strong numeric identifier whose storage width is part of the generated ContractFamily.
    ///
    /// @tparam TTag Semantic identifier domain.
    /// @tparam TBytes Exact storage width in bytes.
    template<class TTag, std::size_t TBytes>
    class NumericIdentifier final {
    private:

        static_assert(
            TBytes == 1U ||
            TBytes == 2U ||
            TBytes == 4U ||
            TBytes == 8U,
            "NumericIdentifier supports 1, 2, 4, or 8-byte unsigned storage"
        );

        // Numeric identity.

        /// Exact numeric identity value.
        Detail::UnsignedStorageForBytesType<TBytes> Value_;

    public:

        /// Exact unsigned storage type selected by TBytes.
        using Storage = Detail::UnsignedStorageForBytesType<TBytes>;

        /// Constructs an identifier from its exact numeric representation.
        constexpr explicit NumericIdentifier(Storage Value) noexcept :
            Value_(Value) {}

        /// Returns the exact numeric representation.
        [[nodiscard]] constexpr Storage Value() const noexcept {
            return Value_;
        }

        /// Compares two identifiers from the same semantic domain.
        [[nodiscard]] constexpr bool operator==(const NumericIdentifier&) const noexcept = default;

    };


    /// Deliberately unconstructible identifier used when one optional identifier universe is absent.
    ///
    /// The type remains complete so generic Resolver declarations remain well-formed, while callers
    /// cannot accidentally manufacture an identity for a ContractFamily that has no such universe.
    ///
    /// @tparam TTag Semantic identifier domain intentionally unavailable in this ContractFamily.
    template<class TTag>
    class UnavailableIdentifier final {
    private:

        /// Prevents construction when the corresponding identifier universe is absent.
        UnavailableIdentifier() = delete;

    };


    namespace Detail {

        /// Selects an available numeric identifier or the unconstructible unavailable form.
        ///
        /// @tparam TTag Semantic identifier domain.
        /// @tparam TBytes Exact numeric identifier width.
        /// @tparam TAvailable Indicates whether the identifier universe exists.
        template<class TTag, std::size_t TBytes, bool TAvailable = (TBytes != 0U)>
        struct NumericIdentifierSelector;

        /// Selects the concrete numeric identifier when its universe exists.
        ///
        /// @tparam TTag Semantic identifier domain.
        /// @tparam TBytes Exact numeric identifier width.
        template<class TTag, std::size_t TBytes>
        struct NumericIdentifierSelector<TTag, TBytes, true> final {

            /// Available numeric identifier type.
            using Type = NumericIdentifier<TTag, TBytes>;

        };

        /// Selects the unavailable identifier when the numeric universe is absent.
        ///
        /// @tparam TTag Semantic identifier domain.
        /// @tparam TBytes Zero-width marker for the absent identifier universe.
        template<class TTag, std::size_t TBytes>
        struct NumericIdentifierSelector<TTag, TBytes, false> final {

            /// Deliberately unconstructible identifier type.
            using Type = UnavailableIdentifier<TTag>;

        };


        /// Selects the universal System Type identifier or the unconstructible unavailable form.
        ///
        /// @tparam TTag Semantic identifier domain used by the unavailable sentinel.
        /// @tparam TAvailable Indicates whether the ContractFamily contains a Type universe.
        template<class TTag, bool TAvailable>
        struct TypeIdentifierSelector;

        /// Selects the universal System Type identifier when the Type universe exists.
        ///
        /// @tparam TTag Semantic identifier domain retained for selector symmetry.
        template<class TTag>
        struct TypeIdentifierSelector<TTag, true> final {

            /// System-owned universal Type identifier.
            using Type = ESPressio::System::TypeIdentifier;

        };

        /// Selects the unavailable identifier when the Type universe is absent.
        ///
        /// @tparam TTag Semantic identifier domain intentionally unavailable.
        template<class TTag>
        struct TypeIdentifierSelector<TTag, false> final {

            /// Deliberately unconstructible identifier Type.
            using Type = UnavailableIdentifier<TTag>;

        };


        /// Selects the universal System Field identifier or the unconstructible unavailable form.
        ///
        /// @tparam TTag Semantic identifier domain used by the unavailable sentinel.
        /// @tparam TAvailable Indicates whether the ContractFamily contains a Field universe.
        template<class TTag, bool TAvailable>
        struct FieldIdentifierSelector;

        /// Selects the universal System Field identifier when the Field universe exists.
        ///
        /// @tparam TTag Semantic identifier domain retained for selector symmetry.
        template<class TTag>
        struct FieldIdentifierSelector<TTag, true> final {

            /// System-owned universal Type-local Field identifier.
            using Type = ESPressio::System::FieldIdentifier;

        };

        /// Selects the unavailable identifier when the Field universe is absent.
        ///
        /// @tparam TTag Semantic identifier domain intentionally unavailable.
        template<class TTag>
        struct FieldIdentifierSelector<TTag, false> final {

            /// Deliberately unconstructible identifier Type.
            using Type = UnavailableIdentifier<TTag>;

        };


    } // ESPressio::Localisation::Detail


    /// Contract-derived Localisation identifier vocabulary.
    ///
    /// @tparam TContract Generated Localisation contract descriptor defining identifier widths.
    template<class TContract>
    struct ContractIdentifiers final {

        static_assert(
            TContract::DomainIdentifierBytes == 1U ||
            TContract::DomainIdentifierBytes == 2U ||
            TContract::DomainIdentifierBytes == 4U,
            "DomainIdentifierBytes must be 1, 2, or 4"
        );

        static_assert(
            TContract::SubDomainIdentifierBytes == 1U ||
            TContract::SubDomainIdentifierBytes == 2U ||
            TContract::SubDomainIdentifierBytes == 4U,
            "SubDomainIdentifierBytes must be 1, 2, or 4"
        );

        static_assert(
            TContract::StringIdentifierBytes == 1U ||
            TContract::StringIdentifierBytes == 2U ||
            TContract::StringIdentifierBytes == 4U,
            "StringIdentifierBytes must be 1, 2, or 4"
        );

        static_assert(
            (
                TContract::TypeIdentifierBytes == 0U &&
                TContract::FieldIdentifierBytes == 0U
            ) ||
            (
                TContract::TypeIdentifierBytes == ESPressio::Localisation::TypeIdentifierBytes &&
                TContract::FieldIdentifierBytes == ESPressio::Localisation::FieldIdentifierBytes
            ),
            "Type/Field identifier widths must both be absent (0/0) or use the fixed 8-byte Type and 1-byte Field widths"
        );

        /// Strong Application/Platform Domain identifier.
        using DomainIdentifier = NumericIdentifier<
            Detail::DomainIdentifierTag,
            TContract::DomainIdentifierBytes
        >;

        /// Strong SubDomain identifier local to a Domain.
        using SubDomainIdentifier = NumericIdentifier<
            Detail::SubDomainIdentifierTag,
            TContract::SubDomainIdentifierBytes
        >;

        /// Strong String identifier local to a SubDomain.
        using StringIdentifierValue = NumericIdentifier<
            Detail::StringIdentifierTag,
            TContract::StringIdentifierBytes
        >;

        /// System-owned universal Type identifier, unavailable when no Type presentation universe exists.
        using TypeIdentifier = typename Detail::TypeIdentifierSelector<
            Detail::TypeIdentifierTag,
            (TContract::TypeIdentifierBytes != 0U)
        >::Type;

        /// System-owned Field identifier local to a Type, unavailable when no schema universe exists.
        using FieldIdentifier = typename Detail::FieldIdentifierSelector<
            Detail::FieldIdentifierTag,
            (TContract::FieldIdentifierBytes != 0U)
        >::Type;


        /// Complete identity of one general Localisation string.
        struct GeneralStringIdentifier final {

            /// Domain containing the represented string.
            DomainIdentifier Domain;

            /// SubDomain containing the represented string.
            SubDomainIdentifier SubDomain;

            /// String identity within SubDomain.
            StringIdentifierValue String;

        };


        /// Complete identity of one presentable field.
        struct FieldPresentationIdentifier final {

            /// Globally unique owning Type identity.
            TypeIdentifier Type;

            /// Field identity local to Type.
            FieldIdentifier Field;

        };

    };

} // ESPressio::Localisation
