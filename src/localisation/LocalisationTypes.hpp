#pragma once

#include <cstddef>
#include <cstdint>
#include <type_traits>

#include <ESPressio_System.hpp>

namespace ESPressio::Localisation {

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

    enum class LocalisationFact : std::uint8_t {
        LanguageFallbackUsed = 0U,
        BufferTooSmall = 1U
    };

    enum class TextMaterialisationStatus : std::uint8_t {
        Success = 0U,
        InvalidArgument = 1U
    };

    enum class TextMaterialisationFact : std::uint8_t {
        BufferTooSmall = 0U
    };

    enum class TextOutputMode : std::uint8_t {
        RawUtf8 = 0U,
        NullTerminatedUtf8 = 1U
    };

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


    namespace Detail {

        template<std::size_t TBytes>
        struct UnsignedStorageForBytes;

        template<>
        struct UnsignedStorageForBytes<1U> final {
            using Type = std::uint8_t;
        };

        template<>
        struct UnsignedStorageForBytes<2U> final {
            using Type = std::uint16_t;
        };

        template<>
        struct UnsignedStorageForBytes<4U> final {
            using Type = std::uint32_t;
        };

        template<>
        struct UnsignedStorageForBytes<8U> final {
            using Type = std::uint64_t;
        };

        template<std::size_t TBytes>
        using UnsignedStorageForBytesType = typename UnsignedStorageForBytes<TBytes>::Type;


        struct DomainIdentifierTag final {};
        struct SubDomainIdentifierTag final {};
        struct StringIdentifierTag final {};
        struct TypeIdentifierTag final {};
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
            TContract::TypeIdentifierBytes == 1U ||
            TContract::TypeIdentifierBytes == 2U ||
            TContract::TypeIdentifierBytes == 4U ||
            TContract::TypeIdentifierBytes == 8U,
            "TypeIdentifierBytes must be 1, 2, 4, or 8"
        );

        static_assert(
            TContract::FieldIdentifierBytes == 1U ||
            TContract::FieldIdentifierBytes == 2U ||
            TContract::FieldIdentifierBytes == 4U,
            "FieldIdentifierBytes must be 1, 2, or 4"
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

        /// Strong globally unique schema Type identifier.
        using TypeIdentifier = NumericIdentifier<
            Detail::TypeIdentifierTag,
            TContract::TypeIdentifierBytes
        >;

        /// Strong Field identifier local to a Type.
        using FieldIdentifier = NumericIdentifier<
            Detail::FieldIdentifierTag,
            TContract::FieldIdentifierBytes
        >;


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
