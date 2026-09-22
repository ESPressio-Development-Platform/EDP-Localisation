#pragma once

#include <array>
#include <cstddef>
#include <cstdint>
#include <limits>

#include <memory/ByteOperationsContract.hpp>
#include <memory/MemoryTypes.hpp>

#include "../LanguageIdentifierView.hpp"
#include "../LocalisationTypes.hpp"
#include "../PackSourceContract.hpp"
#include "EdplFormat.hpp"

namespace ESPressio::Localisation::Detail {

    enum class RepresentationState : std::uint8_t {
        Absent = 0U,
        PresentEmpty = 1U,
        PresentValue = 2U
    };

    enum class PresentationRepresentation : std::uint8_t {
        Name = 0U,
        Description = 1U
    };


    struct SectionDescriptor final {

        /// Absolute first byte of the section.
        std::uint32_t Offset;

        /// Complete persisted section byte count.
        std::uint32_t Length;

        /// Indicates whether the required section was found.
        bool IsPresent;

    };


    struct PackLayout final {

        /// Freshly observed complete resource size.
        std::uint64_t FileSizeBytes;

        /// Persisted complete header size.
        std::uint16_t HeaderSizeBytes;

        /// Persisted whole-file CRC32C.
        std::uint32_t FileCrc32c;

        /// Language Metadata section.
        SectionDescriptor LanguageMetadata;

        /// Language Display-Name section.
        SectionDescriptor LanguageDisplayNames;

        /// General Strings section.
        SectionDescriptor GeneralStrings;

        /// Type Schema section.
        SectionDescriptor TypeSchema;

        /// UTF-8 Payload section.
        SectionDescriptor Utf8Payload;

    };


    struct RepresentationLocation final {

        /// Authored representation state.
        RepresentationState State;

        /// Byte offset relative to the first UTF-8 payload byte.
        std::uint32_t PayloadOffset;

        /// Complete representation byte count.
        std::uint32_t PayloadLength;

        /// Indicates whether the machine entity owning this representation exists.
        bool IsEntityPresent;

    };


    struct RepresentationLookupResult final {

        /// Lookup/format/provider outcome.
        LocalisationStatus Status;

        /// Representation state when Status is Success.
        RepresentationLocation Representation;

    };


    struct PayloadDescriptor final {

        /// Absolute offset of the first UTF-8 payload byte.
        std::uint64_t PayloadBytesOffset;

        /// Complete UTF-8 payload byte count.
        std::uint32_t PayloadLength;

    };


    /// Current-release EDPL reader over one selected Pack Source.
    ///
    /// @tparam TPackSource Concrete Pack Source provider.
    /// @tparam TByteOperationsProvider Concrete EDP-Memory ByteOperations provider.
    /// @tparam TContract Generated Localisation ContractFamily descriptor.
    template<
        class TPackSource,
        class TByteOperationsProvider,
        class TContract
    >
    class EdplReader final {
    private:

        static_assert(
            PackSourceProvider<TPackSource>,
            "EdplReader requires a conforming Localisation Pack Source"
        );

        static_assert(
            sizeof(
                ESPressio::Memory::Detail::ByteOperationsProviderTraits<
                    TByteOperationsProvider
                >
            ) > 0U,
            "EdplReader requires an EDP-Memory ByteOperations provider"
        );

        static_assert(
            TContract::MaximumSupportedLanguageIdentifierBytes > 0U &&
            TContract::MaximumSupportedLanguageIdentifierBytes <= 255U,
            "Generated Localisation contract must declare a 1..255 byte maximum language identity"
        );

        static_assert(
            TContract::ContractFamilyFingerprint.size() == 16U,
            "ContractFamilyFingerprint must contain exactly 16 bytes"
        );


        using PackResource = typename TPackSource::PackResource;
        using Identifiers = ContractIdentifiers<TContract>;


        struct LanguageMetadataValue final {

            /// Indicates whether this pack is the unique terminal language.
            bool IsTerminal;

            /// Canonical language identity length.
            std::uint8_t LanguageLength;

            /// Explicit parent identity length, zero only for terminal language.
            std::uint8_t ParentLength;

            /// Owned canonical language identity bytes.
            std::array<
                char,
                TContract::MaximumSupportedLanguageIdentifierBytes
            > LanguageBytes;

            /// Owned canonical parent identity bytes.
            std::array<
                char,
                TContract::MaximumSupportedLanguageIdentifierBytes
            > ParentBytes;

        };


        struct GeneralStringsHeader final {

            std::uint32_t DomainCount;
            std::uint32_t SubDomainCount;
            std::uint32_t StringCount;
            std::uint32_t DomainTableOffset;
            std::uint32_t SubDomainTableOffset;
            std::uint32_t StringTableOffset;

        };


        struct TypeSchemaHeader final {

            std::uint32_t TypeCount;
            std::uint32_t FieldCount;
            std::uint32_t TypeTableOffset;
            std::uint32_t FieldTableOffset;

        };


        struct TypeRecordValue final {

            std::uint8_t Flags;
            std::uint32_t NameOffset;
            std::uint32_t NameLength;
            std::uint32_t DescriptionOffset;
            std::uint32_t DescriptionLength;
            std::uint32_t FirstFieldIndex;
            std::uint32_t FieldCount;

        };


        struct FieldRecordValue final {

            std::uint8_t Flags;
            std::uint32_t NameOffset;
            std::uint32_t NameLength;
            std::uint32_t DescriptionOffset;
            std::uint32_t DescriptionLength;

        };


        // Bound providers.

        /// Pack Source supplying persisted bytes.
        const TPackSource* PackSource_;

        /// Raw byte operations provider used for exact comparisons.
        const TByteOperationsProvider* ByteOperations_;


        // Numeric decoding.

        /// Decodes one canonical little-endian uint16 value.
        [[nodiscard]] static constexpr std::uint16_t ReadUInt16(
            const std::uint8_t* Bytes
        ) noexcept {
            return static_cast<std::uint16_t>(
                static_cast<std::uint16_t>(Bytes[0]) |
                static_cast<std::uint16_t>(
                    static_cast<std::uint16_t>(Bytes[1]) << 8U
                )
            );
        }

        /// Decodes one canonical little-endian uint32 value.
        [[nodiscard]] static constexpr std::uint32_t ReadUInt32(
            const std::uint8_t* Bytes
        ) noexcept {
            return
                static_cast<std::uint32_t>(Bytes[0]) |
                (
                    static_cast<std::uint32_t>(Bytes[1]) << 8U
                ) |
                (
                    static_cast<std::uint32_t>(Bytes[2]) << 16U
                ) |
                (
                    static_cast<std::uint32_t>(Bytes[3]) << 24U
                );
        }

        /// Decodes one 1/2/4/8-byte canonical little-endian unsigned value.
        [[nodiscard]] static constexpr std::uint64_t ReadUnsigned(
            const std::uint8_t* Bytes,
            std::uint8_t ByteCount
        ) noexcept {
            std::uint64_t Value = 0U;

            for (std::uint8_t Index = 0U; Index < ByteCount; ++Index) {
                Value |= static_cast<std::uint64_t>(Bytes[Index]) << (
                    static_cast<std::uint64_t>(Index) * 8U
                );
            }

            return Value;
        }


        // Range validation.

        /// Indicates whether one relative range is completely contained by TotalLength.
        [[nodiscard]] static constexpr bool IsRelativeRangeValid(
            std::uint64_t Offset,
            std::uint64_t Length,
            std::uint64_t TotalLength
        ) noexcept {
            return
                Offset <= TotalLength &&
                Length <= TotalLength - Offset;
        }

        /// Indicates whether two non-empty absolute ranges overlap.
        [[nodiscard]] static constexpr bool DoRangesOverlap(
            const SectionDescriptor& Left,
            const SectionDescriptor& Right
        ) noexcept {
            if (
                Left.Length == 0U ||
                Right.Length == 0U
            ) {
                return false;
            }

            const std::uint64_t LeftEnd =
                static_cast<std::uint64_t>(Left.Offset) +
                static_cast<std::uint64_t>(Left.Length);
            const std::uint64_t RightEnd =
                static_cast<std::uint64_t>(Right.Offset) +
                static_cast<std::uint64_t>(Right.Length);

            return
                static_cast<std::uint64_t>(Left.Offset) < RightEnd &&
                static_cast<std::uint64_t>(Right.Offset) < LeftEnd;
        }


        // Pack Source result mapping.

        /// Maps one exact Pack Source read into the public Localisation failure vocabulary.
        [[nodiscard]] LocalisationStatus ReadExact(
            const PackResource& Resource,
            std::uint64_t Offset,
            std::uint8_t* Destination,
            std::size_t ByteCount
        ) const noexcept {
            const auto Result = PackSource_->Read(
                Resource,
                Offset,
                {
                    Destination,
                    ByteCount
                }
            );

            switch (Result.Status) {
                case PackReadStatus::Success:
                    return Result.BytesRead == ByteCount
                        ? LocalisationStatus::Success
                        : LocalisationStatus::InvalidDataset;

                case PackReadStatus::ResourceUnavailable:
                    return LocalisationStatus::LanguagePackUnavailable;

                case PackReadStatus::ProviderUnavailable:
                    return LocalisationStatus::ProviderUnavailable;

                case PackReadStatus::OutOfRange:
                    return LocalisationStatus::InvalidDataset;

                case PackReadStatus::ReadFailure:
                    return LocalisationStatus::ReadFailure;
            }

            return LocalisationStatus::ReadFailure;
        }


        // Section assignment.

        /// Records one required section while rejecting duplicate declarations.
        [[nodiscard]] static LocalisationStatus AssignSection(
            Edpl::SectionType Type,
            const SectionDescriptor& Value,
            PackLayout& Layout
        ) noexcept {
            SectionDescriptor* Target = nullptr;

            switch (Type) {
                case Edpl::SectionType::LanguageMetadata:
                    Target = &Layout.LanguageMetadata;
                    break;

                case Edpl::SectionType::LanguageDisplayNames:
                    Target = &Layout.LanguageDisplayNames;
                    break;

                case Edpl::SectionType::GeneralStrings:
                    Target = &Layout.GeneralStrings;
                    break;

                case Edpl::SectionType::TypeSchema:
                    Target = &Layout.TypeSchema;
                    break;

                case Edpl::SectionType::Utf8Payload:
                    Target = &Layout.Utf8Payload;
                    break;
            }

            if (Target == nullptr) {
                return LocalisationStatus::Success;
            }

            if (Target->IsPresent) {
                return LocalisationStatus::InvalidDataset;
            }

            *Target = Value;
            Target->IsPresent = true;

            return LocalisationStatus::Success;
        }


        // Envelope parsing.

        /// Reads and validates the current-format preamble and required section directory.
        [[nodiscard]] LocalisationStatus ReadLayout(
            const PackResource& Resource,
            PackLayout& Layout
        ) const noexcept {
            const auto Size = PackSource_->Size(Resource);

            switch (Size.Status) {
                case PackSizeStatus::Success:
                    break;

                case PackSizeStatus::ResourceUnavailable:
                    return LocalisationStatus::LanguagePackUnavailable;

                case PackSizeStatus::ProviderUnavailable:
                    return LocalisationStatus::ProviderUnavailable;

                case PackSizeStatus::ReadFailure:
                    return LocalisationStatus::ReadFailure;
            }

            if (
                Size.SizeBytes < Edpl::FixedPreambleBytes ||
                Size.SizeBytes > std::numeric_limits<std::uint32_t>::max()
            ) {
                return LocalisationStatus::InvalidDataset;
            }

            std::array<
                std::uint8_t,
                Edpl::FixedPreambleBytes
            > Preamble{};

            const auto PreambleStatus = ReadExact(
                Resource,
                0U,
                Preamble.data(),
                Preamble.size()
            );

            if (PreambleStatus != LocalisationStatus::Success) {
                return PreambleStatus;
            }

            if (
                Preamble[0] != static_cast<std::uint8_t>('E') ||
                Preamble[1] != static_cast<std::uint8_t>('D') ||
                Preamble[2] != static_cast<std::uint8_t>('P') ||
                Preamble[3] != static_cast<std::uint8_t>('L')
            ) {
                return LocalisationStatus::InvalidDataset;
            }

            if (
                Preamble[4] != TContract::FormatMajor ||
                Preamble[5] != TContract::FormatMinor
            ) {
                return LocalisationStatus::UnsupportedFormatVersion;
            }

            const std::uint16_t HeaderSize = ReadUInt16(
                Preamble.data() + 6U
            );
            const std::uint8_t SectionCount = Preamble[8U];

            if (
                Preamble[9U] != 0U ||
                Preamble[13U] != 0U
            ) {
                return LocalisationStatus::InvalidDataset;
            }

            if (
                Preamble[10U] != TContract::DomainIdentifierBytes ||
                Preamble[11U] != TContract::SubDomainIdentifierBytes ||
                Preamble[12U] != TContract::StringIdentifierBytes
            ) {
                return LocalisationStatus::IncompatibleLanguagePack;
            }

            const std::uint32_t PersistedFileSize = ReadUInt32(
                Preamble.data() + 14U
            );

            if (
                PersistedFileSize != Size.SizeBytes
            ) {
                return LocalisationStatus::InvalidDataset;
            }

            const std::uint64_t MinimumHeaderSize =
                Edpl::FixedPreambleBytes +
                (
                    static_cast<std::uint64_t>(SectionCount) *
                    Edpl::SectionDirectoryEntryBytes
                );

            if (
                SectionCount < Edpl::RequiredSectionCount ||
                HeaderSize < MinimumHeaderSize ||
                HeaderSize > Size.SizeBytes
            ) {
                return LocalisationStatus::InvalidDataset;
            }

            Layout = {
                Size.SizeBytes,
                HeaderSize,
                ReadUInt32(
                    Preamble.data() + Edpl::FileCrc32cOffset
                ),
                {0U, 0U, false},
                {0U, 0U, false},
                {0U, 0U, false},
                {0U, 0U, false},
                {0U, 0U, false}
            };

            std::array<
                std::uint8_t,
                Edpl::SectionDirectoryEntryBytes
            > Entry{};

            for (std::uint8_t Index = 0U; Index < SectionCount; ++Index) {
                const std::uint64_t EntryOffset =
                    Edpl::FixedPreambleBytes +
                    (
                        static_cast<std::uint64_t>(Index) *
                        Edpl::SectionDirectoryEntryBytes
                    );

                const auto EntryStatus = ReadExact(
                    Resource,
                    EntryOffset,
                    Entry.data(),
                    Entry.size()
                );

                if (EntryStatus != LocalisationStatus::Success) {
                    return EntryStatus;
                }

                if (
                    Entry[1U] != 0U ||
                    ReadUInt16(Entry.data() + 2U) != 0U
                ) {
                    return LocalisationStatus::InvalidDataset;
                }

                const std::uint32_t Offset = ReadUInt32(
                    Entry.data() + 4U
                );
                const std::uint32_t Length = ReadUInt32(
                    Entry.data() + 8U
                );

                if (
                    Offset < HeaderSize ||
                    !IsRelativeRangeValid(
                        Offset,
                        Length,
                        Size.SizeBytes
                    )
                ) {
                    return LocalisationStatus::InvalidDataset;
                }

                const auto Assignment = AssignSection(
                    static_cast<Edpl::SectionType>(Entry[0U]),
                    {
                        Offset,
                        Length,
                        true
                    },
                    Layout
                );

                if (Assignment != LocalisationStatus::Success) {
                    return Assignment;
                }
            }

            const SectionDescriptor RequiredSections[] = {
                Layout.LanguageMetadata,
                Layout.LanguageDisplayNames,
                Layout.GeneralStrings,
                Layout.TypeSchema,
                Layout.Utf8Payload
            };

            for (const auto& Section : RequiredSections) {
                if (!Section.IsPresent) {
                    return LocalisationStatus::InvalidDataset;
                }
            }

            for (std::size_t Left = 0U; Left < Edpl::RequiredSectionCount; ++Left) {
                for (
                    std::size_t Right = Left + 1U;
                    Right < Edpl::RequiredSectionCount;
                    ++Right
                ) {
                    if (DoRangesOverlap(
                        RequiredSections[Left],
                        RequiredSections[Right]
                    )) {
                        return LocalisationStatus::InvalidDataset;
                    }
                }
            }

            return LocalisationStatus::Success;
        }


        // Language metadata.

        /// Reads and validates language identity, parent, fingerprint and terminal metadata.
        [[nodiscard]] LocalisationStatus ReadLanguageMetadata(
            const PackResource& Resource,
            const PackLayout& Layout,
            LanguageIdentifierView ExpectedLanguage,
            LanguageMetadataValue& Metadata
        ) const noexcept {
            if (
                Layout.LanguageMetadata.Length < Edpl::LanguageMetadataFixedBytes
            ) {
                return LocalisationStatus::InvalidDataset;
            }

            std::array<
                std::uint8_t,
                Edpl::LanguageMetadataFixedBytes
            > Header{};

            const auto HeaderStatus = ReadExact(
                Resource,
                Layout.LanguageMetadata.Offset,
                Header.data(),
                Header.size()
            );

            if (HeaderStatus != LocalisationStatus::Success) {
                return HeaderStatus;
            }

            if (
                Header[0U] != Edpl::SectionVersion ||
                (Header[1U] & static_cast<std::uint8_t>(~Edpl::LanguageTerminalFlag)) != 0U
            ) {
                return LocalisationStatus::InvalidDataset;
            }

            const std::uint8_t LanguageLength = Header[2U];
            const std::uint8_t ParentLength = Header[3U];
            const bool IsTerminal =
                (Header[1U] & Edpl::LanguageTerminalFlag) != 0U;

            if (
                LanguageLength == 0U ||
                LanguageLength > TContract::MaximumSupportedLanguageIdentifierBytes ||
                ParentLength > TContract::MaximumSupportedLanguageIdentifierBytes ||
                (IsTerminal && ParentLength != 0U) ||
                (!IsTerminal && ParentLength == 0U)
            ) {
                return LocalisationStatus::InvalidDataset;
            }

            const std::uint64_t ExpectedSectionLength =
                Edpl::LanguageMetadataFixedBytes +
                static_cast<std::uint64_t>(LanguageLength) +
                static_cast<std::uint64_t>(ParentLength);

            if (Layout.LanguageMetadata.Length != ExpectedSectionLength) {
                return LocalisationStatus::InvalidDataset;
            }

            if (
                ByteOperations_->CompareBytes(
                    Header.data() + 4U,
                    TContract::ContractFamilyFingerprint.data(),
                    16U
                ) != ESPressio::Memory::ByteComparison::Equal
            ) {
                return LocalisationStatus::IncompatibleLanguagePack;
            }

            Metadata = {};
            Metadata.IsTerminal = IsTerminal;
            Metadata.LanguageLength = LanguageLength;
            Metadata.ParentLength = ParentLength;

            const auto LanguageRead = ReadExact(
                Resource,
                static_cast<std::uint64_t>(Layout.LanguageMetadata.Offset) +
                    Edpl::LanguageMetadataFixedBytes,
                reinterpret_cast<std::uint8_t*>(
                    Metadata.LanguageBytes.data()
                ),
                LanguageLength
            );

            if (LanguageRead != LocalisationStatus::Success) {
                return LanguageRead;
            }

            const auto LanguageValidation = LanguageIdentifierView::Validate(
                Metadata.LanguageBytes.data(),
                LanguageLength
            );

            if (!LanguageValidation.IsValuePresent) {
                return LocalisationStatus::InvalidDataset;
            }

            if (
                LanguageValidation.Value.Length() != ExpectedLanguage.Length() ||
                ByteOperations_->CompareBytes(
                    LanguageValidation.Value.Data(),
                    ExpectedLanguage.Data(),
                    ExpectedLanguage.Length()
                ) != ESPressio::Memory::ByteComparison::Equal
            ) {
                return LocalisationStatus::IncompatibleLanguagePack;
            }

            if (ParentLength != 0U) {
                const auto ParentRead = ReadExact(
                    Resource,
                    static_cast<std::uint64_t>(Layout.LanguageMetadata.Offset) +
                        Edpl::LanguageMetadataFixedBytes +
                        LanguageLength,
                    reinterpret_cast<std::uint8_t*>(
                        Metadata.ParentBytes.data()
                    ),
                    ParentLength
                );

                if (ParentRead != LocalisationStatus::Success) {
                    return ParentRead;
                }

                const auto ParentValidation = LanguageIdentifierView::Validate(
                    Metadata.ParentBytes.data(),
                    ParentLength
                );

                if (!ParentValidation.IsValuePresent) {
                    return LocalisationStatus::InvalidDataset;
                }

                if (
                    ParentValidation.Value.IsEqualTo(
                        LanguageValidation.Value
                    )
                ) {
                    return LocalisationStatus::InvalidDataset;
                }
            }

            return LocalisationStatus::Success;
        }


        // Payload metadata.

        /// Reads and validates the shared UTF-8 Payload section header.
        [[nodiscard]] LocalisationStatus ReadPayloadDescriptor(
            const PackResource& Resource,
            const PackLayout& Layout,
            PayloadDescriptor& Payload
        ) const noexcept {
            if (Layout.Utf8Payload.Length < Edpl::PayloadHeaderBytes) {
                return LocalisationStatus::InvalidDataset;
            }

            std::array<
                std::uint8_t,
                Edpl::PayloadHeaderBytes
            > Header{};

            const auto Status = ReadExact(
                Resource,
                Layout.Utf8Payload.Offset,
                Header.data(),
                Header.size()
            );

            if (Status != LocalisationStatus::Success) {
                return Status;
            }

            if (
                Header[0U] != Edpl::SectionVersion ||
                Header[1U] != 0U ||
                ReadUInt16(Header.data() + 2U) != 0U
            ) {
                return LocalisationStatus::InvalidDataset;
            }

            const std::uint32_t PayloadLength = ReadUInt32(
                Header.data() + 4U
            );

            if (
                Layout.Utf8Payload.Length !=
                    Edpl::PayloadHeaderBytes +
                    static_cast<std::uint64_t>(PayloadLength)
            ) {
                return LocalisationStatus::InvalidDataset;
            }

            Payload = {
                static_cast<std::uint64_t>(Layout.Utf8Payload.Offset) +
                    Edpl::PayloadHeaderBytes,
                PayloadLength
            };

            return LocalisationStatus::Success;
        }

        /// Indicates whether one payload reference fits the shared UTF-8 payload.
        [[nodiscard]] static constexpr bool IsPayloadReferenceValid(
            const PayloadDescriptor& Payload,
            std::uint32_t Offset,
            std::uint32_t Length
        ) noexcept {
            return IsRelativeRangeValid(
                Offset,
                Length,
                Payload.PayloadLength
            );
        }


        // General-string header.

        /// Reads and validates the fixed General Strings section header and table bounds.
        [[nodiscard]] LocalisationStatus ReadGeneralStringsHeader(
            const PackResource& Resource,
            const PackLayout& Layout,
            GeneralStringsHeader& HeaderValue
        ) const noexcept {
            if (Layout.GeneralStrings.Length < Edpl::GeneralStringsHeaderBytes) {
                return LocalisationStatus::InvalidDataset;
            }

            std::array<
                std::uint8_t,
                Edpl::GeneralStringsHeaderBytes
            > Header{};

            const auto Status = ReadExact(
                Resource,
                Layout.GeneralStrings.Offset,
                Header.data(),
                Header.size()
            );

            if (Status != LocalisationStatus::Success) {
                return Status;
            }

            if (
                Header[0U] != Edpl::SectionVersion ||
                Header[1U] != 0U ||
                ReadUInt16(Header.data() + 2U) != 0U
            ) {
                return LocalisationStatus::InvalidDataset;
            }

            HeaderValue = {
                ReadUInt32(Header.data() + 4U),
                ReadUInt32(Header.data() + 8U),
                ReadUInt32(Header.data() + 12U),
                ReadUInt32(Header.data() + 16U),
                ReadUInt32(Header.data() + 20U),
                ReadUInt32(Header.data() + 24U)
            };

            const std::uint64_t DomainEntrySize =
                TContract::DomainIdentifierBytes + 8U;
            const std::uint64_t SubDomainEntrySize =
                TContract::SubDomainIdentifierBytes + 8U;
            const std::uint64_t StringEntrySize =
                TContract::StringIdentifierBytes + 8U;

            if (
                !IsRelativeRangeValid(
                    HeaderValue.DomainTableOffset,
                    static_cast<std::uint64_t>(HeaderValue.DomainCount) * DomainEntrySize,
                    Layout.GeneralStrings.Length
                ) ||
                !IsRelativeRangeValid(
                    HeaderValue.SubDomainTableOffset,
                    static_cast<std::uint64_t>(HeaderValue.SubDomainCount) * SubDomainEntrySize,
                    Layout.GeneralStrings.Length
                ) ||
                !IsRelativeRangeValid(
                    HeaderValue.StringTableOffset,
                    static_cast<std::uint64_t>(HeaderValue.StringCount) * StringEntrySize,
                    Layout.GeneralStrings.Length
                )
            ) {
                return LocalisationStatus::InvalidDataset;
            }

            return LocalisationStatus::Success;
        }


        // General-string table lookup.

        /// Locates one Domain record and returns its SubDomain range.
        [[nodiscard]] LocalisationStatus FindDomain(
            const PackResource& Resource,
            const PackLayout& Layout,
            const GeneralStringsHeader& Header,
            const typename Identifiers::TypeIdentifier& Target,
            bool& Found,
            std::uint32_t& FirstSubDomain,
            std::uint32_t& SubDomainCount
        ) const noexcept {
            const std::uint64_t EntrySize =
                TContract::DomainIdentifierBytes + 8U;
            std::uint32_t Lower = 0U;
            std::uint32_t Upper = Header.DomainCount;
            std::array<std::uint8_t, 12U> Record{};

            while (Lower < Upper) {
                const std::uint32_t Middle =
                    Lower + ((Upper - Lower) / 2U);
                const std::uint64_t Offset =
                    static_cast<std::uint64_t>(Layout.GeneralStrings.Offset) +
                    Header.DomainTableOffset +
                    (
                        static_cast<std::uint64_t>(Middle) *
                        EntrySize
                    );

                const auto Status = ReadExact(
                    Resource,
                    Offset,
                    Record.data(),
                    static_cast<std::size_t>(EntrySize)
                );

                if (Status != LocalisationStatus::Success) {
                    return Status;
                }

                const std::uint64_t Value = ReadUnsigned(
                    Record.data(),
                    TContract::DomainIdentifierBytes
                );

                if (Value < Target) {
                    Lower = Middle + 1U;
                    continue;
                }

                if (Value > Target) {
                    Upper = Middle;
                    continue;
                }

                Found = true;
                FirstSubDomain = ReadUInt32(
                    Record.data() + TContract::DomainIdentifierBytes
                );
                SubDomainCount = ReadUInt32(
                    Record.data() + TContract::DomainIdentifierBytes + 4U
                );

                if (
                    static_cast<std::uint64_t>(FirstSubDomain) +
                    SubDomainCount >
                    Header.SubDomainCount
                ) {
                    return LocalisationStatus::InvalidDataset;
                }

                return LocalisationStatus::Success;
            }

            Found = false;
            FirstSubDomain = 0U;
            SubDomainCount = 0U;
            return LocalisationStatus::Success;
        }

        /// Locates one SubDomain record within its owning Domain range.
        [[nodiscard]] LocalisationStatus FindSubDomain(
            const PackResource& Resource,
            const PackLayout& Layout,
            const GeneralStringsHeader& Header,
            std::uint32_t FirstSubDomain,
            std::uint32_t SubDomainCount,
            std::uint64_t Target,
            bool& Found,
            std::uint32_t& FirstString,
            std::uint32_t& StringCount
        ) const noexcept {
            const std::uint64_t EntrySize =
                TContract::SubDomainIdentifierBytes + 8U;
            std::uint32_t Lower = 0U;
            std::uint32_t Upper = SubDomainCount;
            std::array<std::uint8_t, 12U> Record{};

            while (Lower < Upper) {
                const std::uint32_t RelativeMiddle =
                    Lower + ((Upper - Lower) / 2U);
                const std::uint32_t Middle =
                    FirstSubDomain + RelativeMiddle;
                const std::uint64_t Offset =
                    static_cast<std::uint64_t>(Layout.GeneralStrings.Offset) +
                    Header.SubDomainTableOffset +
                    (
                        static_cast<std::uint64_t>(Middle) *
                        EntrySize
                    );

                const auto Status = ReadExact(
                    Resource,
                    Offset,
                    Record.data(),
                    static_cast<std::size_t>(EntrySize)
                );

                if (Status != LocalisationStatus::Success) {
                    return Status;
                }

                const std::uint64_t Value = ReadUnsigned(
                    Record.data(),
                    TContract::SubDomainIdentifierBytes
                );

                if (Value < Target) {
                    Lower = RelativeMiddle + 1U;
                    continue;
                }

                if (Value > Target) {
                    Upper = RelativeMiddle;
                    continue;
                }

                Found = true;
                FirstString = ReadUInt32(
                    Record.data() + TContract::SubDomainIdentifierBytes
                );
                StringCount = ReadUInt32(
                    Record.data() + TContract::SubDomainIdentifierBytes + 4U
                );

                if (
                    static_cast<std::uint64_t>(FirstString) +
                    StringCount >
                    Header.StringCount
                ) {
                    return LocalisationStatus::InvalidDataset;
                }

                return LocalisationStatus::Success;
            }

            Found = false;
            FirstString = 0U;
            StringCount = 0U;
            return LocalisationStatus::Success;
        }

        /// Locates one String record within its owning SubDomain range.
        [[nodiscard]] RepresentationLookupResult FindString(
            const PackResource& Resource,
            const PackLayout& Layout,
            const GeneralStringsHeader& Header,
            const PayloadDescriptor& Payload,
            std::uint32_t FirstString,
            std::uint32_t StringCount,
            std::uint64_t Target
        ) const noexcept {
            const std::uint64_t EntrySize =
                TContract::StringIdentifierBytes + 8U;
            std::uint32_t Lower = 0U;
            std::uint32_t Upper = StringCount;
            std::array<std::uint8_t, 12U> Record{};

            while (Lower < Upper) {
                const std::uint32_t RelativeMiddle =
                    Lower + ((Upper - Lower) / 2U);
                const std::uint32_t Middle =
                    FirstString + RelativeMiddle;
                const std::uint64_t Offset =
                    static_cast<std::uint64_t>(Layout.GeneralStrings.Offset) +
                    Header.StringTableOffset +
                    (
                        static_cast<std::uint64_t>(Middle) *
                        EntrySize
                    );

                const auto Status = ReadExact(
                    Resource,
                    Offset,
                    Record.data(),
                    static_cast<std::size_t>(EntrySize)
                );

                if (Status != LocalisationStatus::Success) {
                    return {
                        Status,
                        {
                            RepresentationState::Absent,
                            0U,
                            0U,
                            false
                        }
                    };
                }

                const std::uint64_t Value = ReadUnsigned(
                    Record.data(),
                    TContract::StringIdentifierBytes
                );

                if (Value < Target) {
                    Lower = RelativeMiddle + 1U;
                    continue;
                }

                if (Value > Target) {
                    Upper = RelativeMiddle;
                    continue;
                }

                const std::uint32_t PayloadOffset = ReadUInt32(
                    Record.data() + TContract::StringIdentifierBytes
                );
                const std::uint32_t PayloadLength = ReadUInt32(
                    Record.data() + TContract::StringIdentifierBytes + 4U
                );

                if (!IsPayloadReferenceValid(
                    Payload,
                    PayloadOffset,
                    PayloadLength
                )) {
                    return {
                        LocalisationStatus::InvalidDataset,
                        {
                            RepresentationState::Absent,
                            0U,
                            0U,
                            false
                        }
                    };
                }

                return {
                    LocalisationStatus::Success,
                    {
                        PayloadLength == 0U
                            ? RepresentationState::PresentEmpty
                            : RepresentationState::PresentValue,
                        PayloadLength == 0U ? 0U : PayloadOffset,
                        PayloadLength,
                        true
                    }
                };
            }

            return {
                LocalisationStatus::Success,
                {
                    RepresentationState::Absent,
                    0U,
                    0U,
                    false
                }
            };
        }


        // Type Schema header.

        /// Reads and validates the fixed Type Schema section header and table bounds.
        [[nodiscard]] LocalisationStatus ReadTypeSchemaHeader(
            const PackResource& Resource,
            const PackLayout& Layout,
            TypeSchemaHeader& HeaderValue
        ) const noexcept {
            if (Layout.TypeSchema.Length < Edpl::TypeSchemaHeaderBytes) {
                return LocalisationStatus::InvalidDataset;
            }

            std::array<
                std::uint8_t,
                Edpl::TypeSchemaHeaderBytes
            > Header{};

            const auto Status = ReadExact(
                Resource,
                Layout.TypeSchema.Offset,
                Header.data(),
                Header.size()
            );

            if (Status != LocalisationStatus::Success) {
                return Status;
            }

            if (
                Header[0U] != Edpl::SectionVersion ||
                Header[1U] != 0U ||
                Header[2U] != TContract::TypeIdentifierBytes ||
                Header[3U] != TContract::FieldIdentifierBytes
            ) {
                return
                    (
                        Header[0U] == Edpl::SectionVersion &&
                        Header[1U] == 0U
                    )
                    ? LocalisationStatus::IncompatibleLanguagePack
                    : LocalisationStatus::InvalidDataset;
            }

            HeaderValue = {
                ReadUInt32(Header.data() + 4U),
                ReadUInt32(Header.data() + 8U),
                ReadUInt32(Header.data() + 12U),
                ReadUInt32(Header.data() + 16U)
            };

            const std::uint64_t TypeEntrySize =
                TContract::TypeIdentifierBytes + 28U;
            const std::uint64_t FieldEntrySize =
                TContract::FieldIdentifierBytes + 20U;

            if (
                !IsRelativeRangeValid(
                    HeaderValue.TypeTableOffset,
                    static_cast<std::uint64_t>(HeaderValue.TypeCount) * TypeEntrySize,
                    Layout.TypeSchema.Length
                ) ||
                !IsRelativeRangeValid(
                    HeaderValue.FieldTableOffset,
                    static_cast<std::uint64_t>(HeaderValue.FieldCount) * FieldEntrySize,
                    Layout.TypeSchema.Length
                )
            ) {
                return LocalisationStatus::InvalidDataset;
            }

            return LocalisationStatus::Success;
        }

        /// Locates one Type record by numeric TypeIdentifier.
        [[nodiscard]] LocalisationStatus FindType(
            const PackResource& Resource,
            const PackLayout& Layout,
            const TypeSchemaHeader& Header,
            std::uint64_t Target,
            bool& Found,
            TypeRecordValue& Value
        ) const noexcept {
            const std::uint64_t EntrySize =
                TContract::TypeIdentifierBytes + 28U;
            std::uint32_t Lower = 0U;
            std::uint32_t Upper = Header.TypeCount;
            std::array<std::uint8_t, TContract::TypeIdentifierBytes + 28U> Record{};

            while (Lower < Upper) {
                const std::uint32_t Middle =
                    Lower + ((Upper - Lower) / 2U);
                const std::uint64_t Offset =
                    static_cast<std::uint64_t>(Layout.TypeSchema.Offset) +
                    Header.TypeTableOffset +
                    (
                        static_cast<std::uint64_t>(Middle) *
                        EntrySize
                    );

                const auto Status = ReadExact(
                    Resource,
                    Offset,
                    Record.data(),
                    static_cast<std::size_t>(EntrySize)
                );

                if (Status != LocalisationStatus::Success) {
                    return Status;
                }

                const auto Comparison = ByteOperations_->CompareBytes(
                    Record.data(),
                    Target.Bytes().data(),
                    TContract::TypeIdentifierBytes
                );

                if (Comparison == ESPressio::Memory::ByteComparison::Less) {
                    Lower = Middle + 1U;
                    continue;
                }

                if (Comparison == ESPressio::Memory::ByteComparison::Greater) {
                    Upper = Middle;
                    continue;
                }

                const std::size_t Base = TContract::TypeIdentifierBytes;
                const std::uint8_t Flags = Record[Base];

                if (
                    (Flags & static_cast<std::uint8_t>(
                        ~(Edpl::NamePresentFlag | Edpl::DescriptionPresentFlag)
                    )) != 0U ||
                    Record[Base + 1U] != 0U ||
                    Record[Base + 2U] != 0U ||
                    Record[Base + 3U] != 0U
                ) {
                    return LocalisationStatus::InvalidDataset;
                }

                Value = {
                    Flags,
                    ReadUInt32(Record.data() + Base + 4U),
                    ReadUInt32(Record.data() + Base + 8U),
                    ReadUInt32(Record.data() + Base + 12U),
                    ReadUInt32(Record.data() + Base + 16U),
                    ReadUInt32(Record.data() + Base + 20U),
                    ReadUInt32(Record.data() + Base + 24U)
                };

                if (
                    static_cast<std::uint64_t>(Value.FirstFieldIndex) +
                    Value.FieldCount >
                    Header.FieldCount
                ) {
                    return LocalisationStatus::InvalidDataset;
                }

                Found = true;
                return LocalisationStatus::Success;
            }

            Found = false;
            Value = {};
            return LocalisationStatus::Success;
        }

        /// Locates one Field record inside its owning Type's contiguous field range.
        [[nodiscard]] LocalisationStatus FindField(
            const PackResource& Resource,
            const PackLayout& Layout,
            const TypeSchemaHeader& Header,
            const TypeRecordValue& TypeValue,
            std::uint64_t Target,
            bool& Found,
            FieldRecordValue& Value
        ) const noexcept {
            const std::uint64_t EntrySize =
                TContract::FieldIdentifierBytes + 20U;
            std::uint32_t Lower = 0U;
            std::uint32_t Upper = TypeValue.FieldCount;
            std::array<std::uint8_t, 24U> Record{};

            while (Lower < Upper) {
                const std::uint32_t RelativeMiddle =
                    Lower + ((Upper - Lower) / 2U);
                const std::uint32_t Middle =
                    TypeValue.FirstFieldIndex + RelativeMiddle;
                const std::uint64_t Offset =
                    static_cast<std::uint64_t>(Layout.TypeSchema.Offset) +
                    Header.FieldTableOffset +
                    (
                        static_cast<std::uint64_t>(Middle) *
                        EntrySize
                    );

                const auto Status = ReadExact(
                    Resource,
                    Offset,
                    Record.data(),
                    static_cast<std::size_t>(EntrySize)
                );

                if (Status != LocalisationStatus::Success) {
                    return Status;
                }

                const std::uint64_t Identifier = ReadUnsigned(
                    Record.data(),
                    TContract::FieldIdentifierBytes
                );

                if (Identifier < Target) {
                    Lower = RelativeMiddle + 1U;
                    continue;
                }

                if (Identifier > Target) {
                    Upper = RelativeMiddle;
                    continue;
                }

                const std::size_t Base = TContract::FieldIdentifierBytes;
                const std::uint8_t Flags = Record[Base];

                if (
                    (Flags & static_cast<std::uint8_t>(
                        ~(Edpl::NamePresentFlag | Edpl::DescriptionPresentFlag)
                    )) != 0U ||
                    Record[Base + 1U] != 0U ||
                    Record[Base + 2U] != 0U ||
                    Record[Base + 3U] != 0U
                ) {
                    return LocalisationStatus::InvalidDataset;
                }

                Value = {
                    Flags,
                    ReadUInt32(Record.data() + Base + 4U),
                    ReadUInt32(Record.data() + Base + 8U),
                    ReadUInt32(Record.data() + Base + 12U),
                    ReadUInt32(Record.data() + Base + 16U)
                };

                Found = true;
                return LocalisationStatus::Success;
            }

            Found = false;
            Value = {};
            return LocalisationStatus::Success;
        }

        /// Converts one present/absent Type or Field representation into a lookup result.
        [[nodiscard]] static RepresentationLookupResult SelectRepresentation(
            bool EntityPresent,
            std::uint8_t Flags,
            std::uint32_t NameOffset,
            std::uint32_t NameLength,
            std::uint32_t DescriptionOffset,
            std::uint32_t DescriptionLength,
            PresentationRepresentation Representation,
            const PayloadDescriptor& Payload
        ) noexcept {
            if (!EntityPresent) {
                return {
                    LocalisationStatus::Success,
                    {
                        RepresentationState::Absent,
                        0U,
                        0U,
                        false
                    }
                };
            }

            const bool IsName =
                Representation == PresentationRepresentation::Name;
            const std::uint8_t PresenceFlag = IsName
                ? Edpl::NamePresentFlag
                : Edpl::DescriptionPresentFlag;
            const bool IsPresent =
                (Flags & PresenceFlag) != 0U;
            const std::uint32_t Offset = IsName
                ? NameOffset
                : DescriptionOffset;
            const std::uint32_t Length = IsName
                ? NameLength
                : DescriptionLength;

            if (!IsPresent) {
                if (
                    Offset != 0U ||
                    Length != 0U
                ) {
                    return {
                        LocalisationStatus::InvalidDataset,
                        {
                            RepresentationState::Absent,
                            0U,
                            0U,
                            true
                        }
                    };
                }

                return {
                    LocalisationStatus::Success,
                    {
                        RepresentationState::Absent,
                        0U,
                        0U,
                        true
                    }
                };
            }

            if (!IsPayloadReferenceValid(
                Payload,
                Offset,
                Length
            )) {
                return {
                    LocalisationStatus::InvalidDataset,
                    {
                        RepresentationState::Absent,
                        0U,
                        0U,
                        true
                    }
                };
            }

            return {
                LocalisationStatus::Success,
                {
                    Length == 0U
                        ? RepresentationState::PresentEmpty
                        : RepresentationState::PresentValue,
                    Length == 0U ? 0U : Offset,
                    Length,
                    true
                }
            };
        }

        // Complete validation.

        /// Advances one byte through the reflected CRC32C state.
        [[nodiscard]] static constexpr std::uint32_t AdvanceCrc32c(
            std::uint32_t Crc,
            std::uint8_t Byte
        ) noexcept {
            Crc ^= Byte;

            for (std::uint8_t Bit = 0U; Bit < 8U; ++Bit) {
                Crc = (Crc & 1U) != 0U
                    ? static_cast<std::uint32_t>(
                        (Crc >> 1U) ^ 0x82F63B78U
                    )
                    : static_cast<std::uint32_t>(Crc >> 1U);
            }

            return Crc;
        }

        /// Verifies the whole-file CRC32C while treating the persisted CRC field as zero.
        [[nodiscard]] LocalisationStatus ValidateCrc32c(
            const PackResource& Resource,
            const PackLayout& Layout
        ) const noexcept {
            std::array<std::uint8_t, 64U> Buffer{};
            std::uint64_t Offset = 0U;
            std::uint32_t Crc = 0xFFFFFFFFU;

            while (Offset < Layout.FileSizeBytes) {
                const std::uint64_t Remaining =
                    Layout.FileSizeBytes - Offset;
                const std::size_t ByteCount = static_cast<std::size_t>(
                    Remaining < Buffer.size()
                        ? Remaining
                        : Buffer.size()
                );

                const auto Status = ReadExact(
                    Resource,
                    Offset,
                    Buffer.data(),
                    ByteCount
                );

                if (Status != LocalisationStatus::Success) {
                    return Status;
                }

                for (std::size_t Index = 0U; Index < ByteCount; ++Index) {
                    const std::uint64_t AbsoluteOffset =
                        Offset + Index;
                    const bool IsCrcField =
                        AbsoluteOffset >= Edpl::FileCrc32cOffset &&
                        AbsoluteOffset <
                            Edpl::FileCrc32cOffset +
                            Edpl::FileCrc32cBytes;

                    Crc = AdvanceCrc32c(
                        Crc,
                        IsCrcField
                            ? 0U
                            : Buffer[Index]
                    );
                }

                Offset += ByteCount;
            }

            Crc ^= 0xFFFFFFFFU;

            return Crc == Layout.FileCrc32c
                ? LocalisationStatus::Success
                : LocalisationStatus::InvalidDataset;
        }

        /// Verifies that one complete payload representation is valid UTF-8 without embedded NUL.
        [[nodiscard]] LocalisationStatus ValidateUtf8Payload(
            const PackResource& Resource,
            const PayloadDescriptor& Payload,
            std::uint32_t PayloadOffset,
            std::uint32_t PayloadLength
        ) const noexcept {
            if (!IsPayloadReferenceValid(
                Payload,
                PayloadOffset,
                PayloadLength
            )) {
                return LocalisationStatus::InvalidDataset;
            }

            std::array<std::uint8_t, 64U> Buffer{};
            std::uint32_t ReadOffset = 0U;
            std::uint32_t CodePoint = 0U;
            std::uint32_t MinimumCodePoint = 0U;
            std::uint8_t ContinuationsRemaining = 0U;

            while (ReadOffset < PayloadLength) {
                const std::uint32_t Remaining =
                    PayloadLength - ReadOffset;
                const std::size_t ByteCount = static_cast<std::size_t>(
                    Remaining < Buffer.size()
                        ? Remaining
                        : Buffer.size()
                );

                const auto Status = ReadExact(
                    Resource,
                    Payload.PayloadBytesOffset +
                        PayloadOffset +
                        ReadOffset,
                    Buffer.data(),
                    ByteCount
                );

                if (Status != LocalisationStatus::Success) {
                    return Status;
                }

                for (std::size_t Index = 0U; Index < ByteCount; ++Index) {
                    const std::uint8_t Byte = Buffer[Index];

                    if (ContinuationsRemaining == 0U) {
                        if (Byte == 0U) {
                            return LocalisationStatus::InvalidDataset;
                        }

                        if (Byte <= 0x7FU) {
                            continue;
                        }

                        if ((Byte & 0xE0U) == 0xC0U) {
                            CodePoint = Byte & 0x1FU;
                            MinimumCodePoint = 0x80U;
                            ContinuationsRemaining = 1U;
                            continue;
                        }

                        if ((Byte & 0xF0U) == 0xE0U) {
                            CodePoint = Byte & 0x0FU;
                            MinimumCodePoint = 0x800U;
                            ContinuationsRemaining = 2U;
                            continue;
                        }

                        if ((Byte & 0xF8U) == 0xF0U) {
                            CodePoint = Byte & 0x07U;
                            MinimumCodePoint = 0x10000U;
                            ContinuationsRemaining = 3U;
                            continue;
                        }

                        return LocalisationStatus::InvalidDataset;
                    }

                    if ((Byte & 0xC0U) != 0x80U) {
                        return LocalisationStatus::InvalidDataset;
                    }

                    CodePoint =
                        (CodePoint << 6U) |
                        static_cast<std::uint32_t>(Byte & 0x3FU);
                    --ContinuationsRemaining;

                    if (ContinuationsRemaining != 0U) {
                        continue;
                    }

                    if (
                        CodePoint < MinimumCodePoint ||
                        CodePoint > 0x10FFFFU ||
                        (
                            CodePoint >= 0xD800U &&
                            CodePoint <= 0xDFFFU
                        )
                    ) {
                        return LocalisationStatus::InvalidDataset;
                    }
                }

                ReadOffset += static_cast<std::uint32_t>(ByteCount);
            }

            return ContinuationsRemaining == 0U
                ? LocalisationStatus::Success
                : LocalisationStatus::InvalidDataset;
        }

        /// Compares two canonical language tags lexicographically.
        [[nodiscard]] ESPressio::Memory::ByteComparison CompareText(
            const char* Left,
            std::size_t LeftLength,
            const char* Right,
            std::size_t RightLength
        ) const noexcept {
            const std::size_t CommonLength =
                LeftLength < RightLength
                    ? LeftLength
                    : RightLength;

            const auto CommonComparison = ByteOperations_->CompareBytes(
                Left,
                Right,
                CommonLength
            );

            if (
                CommonComparison !=
                ESPressio::Memory::ByteComparison::Equal
            ) {
                return CommonComparison;
            }

            if (LeftLength < RightLength) {
                return ESPressio::Memory::ByteComparison::Less;
            }

            if (LeftLength > RightLength) {
                return ESPressio::Memory::ByteComparison::Greater;
            }

            return ESPressio::Memory::ByteComparison::Equal;
        }

        /// Verifies that every section-directory range is pairwise non-overlapping.
        [[nodiscard]] LocalisationStatus ValidateCompleteSectionDirectory(
            const PackResource& Resource,
            const PackLayout& Layout
        ) const noexcept {
            const std::uint64_t DirectoryBytes =
                Layout.HeaderSizeBytes - Edpl::FixedPreambleBytes;

            if (
                DirectoryBytes % Edpl::SectionDirectoryEntryBytes != 0U
            ) {
                return LocalisationStatus::InvalidDataset;
            }

            const std::uint64_t SectionCount =
                DirectoryBytes / Edpl::SectionDirectoryEntryBytes;

            std::array<
                std::uint8_t,
                Edpl::SectionDirectoryEntryBytes
            > LeftBytes{};
            std::array<
                std::uint8_t,
                Edpl::SectionDirectoryEntryBytes
            > RightBytes{};

            for (std::uint64_t LeftIndex = 0U; LeftIndex < SectionCount; ++LeftIndex) {
                const auto LeftStatus = ReadExact(
                    Resource,
                    Edpl::FixedPreambleBytes +
                        LeftIndex * Edpl::SectionDirectoryEntryBytes,
                    LeftBytes.data(),
                    LeftBytes.size()
                );

                if (LeftStatus != LocalisationStatus::Success) {
                    return LeftStatus;
                }

                const SectionDescriptor Left{
                    ReadUInt32(LeftBytes.data() + 4U),
                    ReadUInt32(LeftBytes.data() + 8U),
                    true
                };

                for (
                    std::uint64_t RightIndex = LeftIndex + 1U;
                    RightIndex < SectionCount;
                    ++RightIndex
                ) {
                    const auto RightStatus = ReadExact(
                        Resource,
                        Edpl::FixedPreambleBytes +
                            RightIndex * Edpl::SectionDirectoryEntryBytes,
                        RightBytes.data(),
                        RightBytes.size()
                    );

                    if (RightStatus != LocalisationStatus::Success) {
                        return RightStatus;
                    }

                    const SectionDescriptor Right{
                        ReadUInt32(RightBytes.data() + 4U),
                        ReadUInt32(RightBytes.data() + 8U),
                        true
                    };

                    if (DoRangesOverlap(
                        Left,
                        Right
                    )) {
                        return LocalisationStatus::InvalidDataset;
                    }
                }
            }

            return LocalisationStatus::Success;
        }

        /// Verifies all language-display-name records and referenced UTF-8 payload.
        [[nodiscard]] LocalisationStatus ValidateLanguageDisplayNames(
            const PackResource& Resource,
            const PackLayout& Layout,
            const PayloadDescriptor& Payload
        ) const noexcept {
            if (
                Layout.LanguageDisplayNames.Length <
                Edpl::LanguageDisplayHeaderBytes
            ) {
                return LocalisationStatus::InvalidDataset;
            }

            std::array<
                std::uint8_t,
                Edpl::LanguageDisplayHeaderBytes
            > Header{};

            const auto HeaderStatus = ReadExact(
                Resource,
                Layout.LanguageDisplayNames.Offset,
                Header.data(),
                Header.size()
            );

            if (HeaderStatus != LocalisationStatus::Success) {
                return HeaderStatus;
            }

            if (
                Header[0U] != Edpl::SectionVersion ||
                Header[1U] != 0U
            ) {
                return LocalisationStatus::InvalidDataset;
            }

            const std::uint16_t EntryCount = ReadUInt16(
                Header.data() + 2U
            );
            const std::uint64_t ExpectedLength =
                Edpl::LanguageDisplayHeaderBytes +
                static_cast<std::uint64_t>(EntryCount) *
                    Edpl::LanguageDisplayEntryBytes;

            if (ExpectedLength != Layout.LanguageDisplayNames.Length) {
                return LocalisationStatus::InvalidDataset;
            }

            std::array<
                std::uint8_t,
                Edpl::LanguageDisplayEntryBytes
            > Entry{};
            std::array<
                char,
                TContract::MaximumSupportedLanguageIdentifierBytes
            > CurrentTarget{};
            std::array<
                char,
                TContract::MaximumSupportedLanguageIdentifierBytes
            > PreviousTarget{};
            std::size_t PreviousLength = 0U;
            bool HasPrevious = false;

            for (std::uint16_t Index = 0U; Index < EntryCount; ++Index) {
                const auto EntryStatus = ReadExact(
                    Resource,
                    static_cast<std::uint64_t>(Layout.LanguageDisplayNames.Offset) +
                        Edpl::LanguageDisplayHeaderBytes +
                        static_cast<std::uint64_t>(Index) *
                            Edpl::LanguageDisplayEntryBytes,
                    Entry.data(),
                    Entry.size()
                );

                if (EntryStatus != LocalisationStatus::Success) {
                    return EntryStatus;
                }

                if (
                    Entry[5U] != 0U ||
                    Entry[6U] != 0U ||
                    Entry[7U] != 0U
                ) {
                    return LocalisationStatus::InvalidDataset;
                }

                const std::uint32_t TargetOffset = ReadUInt32(
                    Entry.data()
                );
                const std::size_t TargetLength = Entry[4U];
                const std::uint32_t DisplayOffset = ReadUInt32(
                    Entry.data() + 8U
                );
                const std::uint32_t DisplayLength = ReadUInt32(
                    Entry.data() + 12U
                );

                if (
                    TargetLength == 0U ||
                    TargetLength > CurrentTarget.size() ||
                    !IsPayloadReferenceValid(
                        Payload,
                        TargetOffset,
                        static_cast<std::uint32_t>(TargetLength)
                    ) ||
                    !IsPayloadReferenceValid(
                        Payload,
                        DisplayOffset,
                        DisplayLength
                    )
                ) {
                    return LocalisationStatus::InvalidDataset;
                }

                const auto TargetStatus = ReadExact(
                    Resource,
                    Payload.PayloadBytesOffset + TargetOffset,
                    reinterpret_cast<std::uint8_t*>(CurrentTarget.data()),
                    TargetLength
                );

                if (TargetStatus != LocalisationStatus::Success) {
                    return TargetStatus;
                }

                const auto TargetValidation = LanguageIdentifierView::Validate(
                    CurrentTarget.data(),
                    TargetLength
                );

                if (!TargetValidation.IsValuePresent) {
                    return LocalisationStatus::InvalidDataset;
                }

                if (
                    HasPrevious &&
                    CompareText(
                        PreviousTarget.data(),
                        PreviousLength,
                        CurrentTarget.data(),
                        TargetLength
                    ) != ESPressio::Memory::ByteComparison::Less
                ) {
                    return LocalisationStatus::InvalidDataset;
                }

                const auto DisplayStatus = ValidateUtf8Payload(
                    Resource,
                    Payload,
                    DisplayOffset,
                    DisplayLength
                );

                if (DisplayStatus != LocalisationStatus::Success) {
                    return DisplayStatus;
                }

                ByteOperations_->CopyBytes(
                    PreviousTarget.data(),
                    CurrentTarget.data(),
                    TargetLength
                );
                PreviousLength = TargetLength;
                HasPrevious = true;
            }

            return LocalisationStatus::Success;
        }

        /// Verifies every General Strings table record, range and UTF-8 payload reference.
        [[nodiscard]] LocalisationStatus ValidateGeneralStrings(
            const PackResource& Resource,
            const PackLayout& Layout,
            const PayloadDescriptor& Payload
        ) const noexcept {
            GeneralStringsHeader Header{};

            const auto HeaderStatus = ReadGeneralStringsHeader(
                Resource,
                Layout,
                Header
            );

            if (HeaderStatus != LocalisationStatus::Success) {
                return HeaderStatus;
            }

            const std::uint64_t DomainEntrySize =
                TContract::DomainIdentifierBytes + 8U;
            const std::uint64_t SubDomainEntrySize =
                TContract::SubDomainIdentifierBytes + 8U;
            const std::uint64_t StringEntrySize =
                TContract::StringIdentifierBytes + 8U;
            std::array<std::uint8_t, 12U> Record{};
            std::uint32_t ExpectedSubDomainIndex = 0U;
            std::uint32_t ExpectedStringIndex = 0U;
            std::uint64_t PreviousDomainId = 0U;
            bool HasPreviousDomain = false;

            for (std::uint32_t DomainIndex = 0U; DomainIndex < Header.DomainCount; ++DomainIndex) {
                const auto DomainStatus = ReadExact(
                    Resource,
                    static_cast<std::uint64_t>(Layout.GeneralStrings.Offset) +
                        Header.DomainTableOffset +
                        static_cast<std::uint64_t>(DomainIndex) *
                            DomainEntrySize,
                    Record.data(),
                    static_cast<std::size_t>(DomainEntrySize)
                );

                if (DomainStatus != LocalisationStatus::Success) {
                    return DomainStatus;
                }

                const std::uint64_t DomainId = ReadUnsigned(
                    Record.data(),
                    TContract::DomainIdentifierBytes
                );
                const std::uint32_t FirstSubDomain = ReadUInt32(
                    Record.data() + TContract::DomainIdentifierBytes
                );
                const std::uint32_t SubDomainCount = ReadUInt32(
                    Record.data() + TContract::DomainIdentifierBytes + 4U
                );

                if (
                    (HasPreviousDomain && DomainId <= PreviousDomainId) ||
                    FirstSubDomain != ExpectedSubDomainIndex ||
                    static_cast<std::uint64_t>(FirstSubDomain) +
                        SubDomainCount >
                        Header.SubDomainCount
                ) {
                    return LocalisationStatus::InvalidDataset;
                }

                PreviousDomainId = DomainId;
                HasPreviousDomain = true;
                std::uint64_t PreviousSubDomainId = 0U;
                bool HasPreviousSubDomain = false;

                for (std::uint32_t RelativeSubDomain = 0U; RelativeSubDomain < SubDomainCount; ++RelativeSubDomain) {
                    const std::uint32_t SubDomainIndex =
                        FirstSubDomain + RelativeSubDomain;
                    const auto SubDomainStatus = ReadExact(
                        Resource,
                        static_cast<std::uint64_t>(Layout.GeneralStrings.Offset) +
                            Header.SubDomainTableOffset +
                            static_cast<std::uint64_t>(SubDomainIndex) *
                                SubDomainEntrySize,
                        Record.data(),
                        static_cast<std::size_t>(SubDomainEntrySize)
                    );

                    if (SubDomainStatus != LocalisationStatus::Success) {
                        return SubDomainStatus;
                    }

                    const std::uint64_t SubDomainId = ReadUnsigned(
                        Record.data(),
                        TContract::SubDomainIdentifierBytes
                    );
                    const std::uint32_t FirstString = ReadUInt32(
                        Record.data() + TContract::SubDomainIdentifierBytes
                    );
                    const std::uint32_t StringCount = ReadUInt32(
                        Record.data() + TContract::SubDomainIdentifierBytes + 4U
                    );

                    if (
                        (HasPreviousSubDomain && SubDomainId <= PreviousSubDomainId) ||
                        FirstString != ExpectedStringIndex ||
                        static_cast<std::uint64_t>(FirstString) +
                            StringCount >
                            Header.StringCount
                    ) {
                        return LocalisationStatus::InvalidDataset;
                    }

                    PreviousSubDomainId = SubDomainId;
                    HasPreviousSubDomain = true;
                    std::uint64_t PreviousStringId = 0U;
                    bool HasPreviousString = false;

                    for (std::uint32_t RelativeString = 0U; RelativeString < StringCount; ++RelativeString) {
                        const std::uint32_t StringIndex =
                            FirstString + RelativeString;
                        const auto StringStatus = ReadExact(
                            Resource,
                            static_cast<std::uint64_t>(Layout.GeneralStrings.Offset) +
                                Header.StringTableOffset +
                                static_cast<std::uint64_t>(StringIndex) *
                                    StringEntrySize,
                            Record.data(),
                            static_cast<std::size_t>(StringEntrySize)
                        );

                        if (StringStatus != LocalisationStatus::Success) {
                            return StringStatus;
                        }

                        const std::uint64_t StringId = ReadUnsigned(
                            Record.data(),
                            TContract::StringIdentifierBytes
                        );
                        const std::uint32_t PayloadOffset = ReadUInt32(
                            Record.data() + TContract::StringIdentifierBytes
                        );
                        const std::uint32_t PayloadLength = ReadUInt32(
                            Record.data() + TContract::StringIdentifierBytes + 4U
                        );

                        if (
                            (HasPreviousString && StringId <= PreviousStringId) ||
                            !IsPayloadReferenceValid(
                                Payload,
                                PayloadOffset,
                                PayloadLength
                            )
                        ) {
                            return LocalisationStatus::InvalidDataset;
                        }

                        PreviousStringId = StringId;
                        HasPreviousString = true;

                        const auto PayloadStatus = ValidateUtf8Payload(
                            Resource,
                            Payload,
                            PayloadOffset,
                            PayloadLength
                        );

                        if (PayloadStatus != LocalisationStatus::Success) {
                            return PayloadStatus;
                        }

                        ++ExpectedStringIndex;
                    }
                }

                ExpectedSubDomainIndex += SubDomainCount;
            }

            return
                ExpectedSubDomainIndex == Header.SubDomainCount &&
                ExpectedStringIndex == Header.StringCount
                ? LocalisationStatus::Success
                : LocalisationStatus::InvalidDataset;
        }

        /// Verifies one Type/Field representation payload pair.
        [[nodiscard]] LocalisationStatus ValidatePresentationPair(
            const PackResource& Resource,
            const PayloadDescriptor& Payload,
            std::uint8_t Flags,
            std::uint8_t PresenceFlag,
            std::uint32_t Offset,
            std::uint32_t Length
        ) const noexcept {
            const bool IsPresent = (Flags & PresenceFlag) != 0U;

            if (!IsPresent) {
                return
                    Offset == 0U &&
                    Length == 0U
                    ? LocalisationStatus::Success
                    : LocalisationStatus::InvalidDataset;
            }

            return ValidateUtf8Payload(
                Resource,
                Payload,
                Offset,
                Length
            );
        }

        /// Verifies every Type Schema record, field range and presentation payload reference.
        [[nodiscard]] LocalisationStatus ValidateTypeSchema(
            const PackResource& Resource,
            const PackLayout& Layout,
            const PayloadDescriptor& Payload
        ) const noexcept {
            TypeSchemaHeader Header{};

            const auto HeaderStatus = ReadTypeSchemaHeader(
                Resource,
                Layout,
                Header
            );

            if (HeaderStatus != LocalisationStatus::Success) {
                return HeaderStatus;
            }

            const std::uint64_t TypeEntrySize =
                TContract::TypeIdentifierBytes + 28U;
            const std::uint64_t FieldEntrySize =
                TContract::FieldIdentifierBytes + 20U;
            std::array<std::uint8_t, TContract::TypeIdentifierBytes + 28U> Record{};
            std::array<std::uint8_t, TContract::TypeIdentifierBytes> PreviousTypeId{};
            bool HasPreviousType = false;
            std::uint32_t ExpectedFieldIndex = 0U;

            for (std::uint32_t TypeIndex = 0U; TypeIndex < Header.TypeCount; ++TypeIndex) {
                const auto TypeStatus = ReadExact(
                    Resource,
                    static_cast<std::uint64_t>(Layout.TypeSchema.Offset) +
                        Header.TypeTableOffset +
                        static_cast<std::uint64_t>(TypeIndex) *
                            TypeEntrySize,
                    Record.data(),
                    static_cast<std::size_t>(TypeEntrySize)
                );

                if (TypeStatus != LocalisationStatus::Success) {
                    return TypeStatus;
                }

                const std::size_t Base = TContract::TypeIdentifierBytes;
                const std::uint8_t Flags = Record[Base];
                const bool IsTypeOrderingInvalid =
                    HasPreviousType &&
                    ByteOperations_->CompareBytes(
                        PreviousTypeId.data(),
                        Record.data(),
                        TContract::TypeIdentifierBytes
                    ) != ESPressio::Memory::ByteComparison::Less;

                if (
                    IsTypeOrderingInvalid ||
                    (Flags & static_cast<std::uint8_t>(
                        ~(Edpl::NamePresentFlag | Edpl::DescriptionPresentFlag)
                    )) != 0U ||
                    Record[Base + 1U] != 0U ||
                    Record[Base + 2U] != 0U ||
                    Record[Base + 3U] != 0U
                ) {
                    return LocalisationStatus::InvalidDataset;
                }

                const std::uint32_t NameOffset = ReadUInt32(
                    Record.data() + Base + 4U
                );
                const std::uint32_t NameLength = ReadUInt32(
                    Record.data() + Base + 8U
                );
                const std::uint32_t DescriptionOffset = ReadUInt32(
                    Record.data() + Base + 12U
                );
                const std::uint32_t DescriptionLength = ReadUInt32(
                    Record.data() + Base + 16U
                );
                const std::uint32_t FirstField = ReadUInt32(
                    Record.data() + Base + 20U
                );
                const std::uint32_t FieldCount = ReadUInt32(
                    Record.data() + Base + 24U
                );

                if (
                    FirstField != ExpectedFieldIndex ||
                    static_cast<std::uint64_t>(FirstField) +
                        FieldCount >
                        Header.FieldCount
                ) {
                    return LocalisationStatus::InvalidDataset;
                }

                const auto NameStatus = ValidatePresentationPair(
                    Resource,
                    Payload,
                    Flags,
                    Edpl::NamePresentFlag,
                    NameOffset,
                    NameLength
                );

                if (NameStatus != LocalisationStatus::Success) {
                    return NameStatus;
                }

                const auto DescriptionStatus = ValidatePresentationPair(
                    Resource,
                    Payload,
                    Flags,
                    Edpl::DescriptionPresentFlag,
                    DescriptionOffset,
                    DescriptionLength
                );

                if (DescriptionStatus != LocalisationStatus::Success) {
                    return DescriptionStatus;
                }

                ByteOperations_->CopyBytes(
                    PreviousTypeId.data(),
                    Record.data(),
                    TContract::TypeIdentifierBytes
                );
                HasPreviousType = true;
                std::uint64_t PreviousFieldId = 0U;
                bool HasPreviousField = false;

                for (std::uint32_t RelativeField = 0U; RelativeField < FieldCount; ++RelativeField) {
                    const std::uint32_t FieldIndex =
                        FirstField + RelativeField;
                    const auto FieldStatus = ReadExact(
                        Resource,
                        static_cast<std::uint64_t>(Layout.TypeSchema.Offset) +
                            Header.FieldTableOffset +
                            static_cast<std::uint64_t>(FieldIndex) *
                                FieldEntrySize,
                        Record.data(),
                        static_cast<std::size_t>(FieldEntrySize)
                    );

                    if (FieldStatus != LocalisationStatus::Success) {
                        return FieldStatus;
                    }

                    const std::uint64_t FieldId = ReadUnsigned(
                        Record.data(),
                        TContract::FieldIdentifierBytes
                    );
                    const std::size_t FieldBase =
                        TContract::FieldIdentifierBytes;
                    const std::uint8_t FieldFlags =
                        Record[FieldBase];

                    if (
                        (HasPreviousField && FieldId <= PreviousFieldId) ||
                        (FieldFlags & static_cast<std::uint8_t>(
                            ~(Edpl::NamePresentFlag | Edpl::DescriptionPresentFlag)
                        )) != 0U ||
                        Record[FieldBase + 1U] != 0U ||
                        Record[FieldBase + 2U] != 0U ||
                        Record[FieldBase + 3U] != 0U
                    ) {
                        return LocalisationStatus::InvalidDataset;
                    }

                    const auto FieldNameStatus = ValidatePresentationPair(
                        Resource,
                        Payload,
                        FieldFlags,
                        Edpl::NamePresentFlag,
                        ReadUInt32(Record.data() + FieldBase + 4U),
                        ReadUInt32(Record.data() + FieldBase + 8U)
                    );

                    if (FieldNameStatus != LocalisationStatus::Success) {
                        return FieldNameStatus;
                    }

                    const auto FieldDescriptionStatus = ValidatePresentationPair(
                        Resource,
                        Payload,
                        FieldFlags,
                        Edpl::DescriptionPresentFlag,
                        ReadUInt32(Record.data() + FieldBase + 12U),
                        ReadUInt32(Record.data() + FieldBase + 16U)
                    );

                    if (FieldDescriptionStatus != LocalisationStatus::Success) {
                        return FieldDescriptionStatus;
                    }

                    PreviousFieldId = FieldId;
                    HasPreviousField = true;
                    ++ExpectedFieldIndex;
                }
            }

            return ExpectedFieldIndex == Header.FieldCount
                ? LocalisationStatus::Success
                : LocalisationStatus::InvalidDataset;
        }


    public:

        /// Exposes owned language metadata for format-independent fallback traversal.
        using LanguageMetadata = LanguageMetadataValue;


        // Construction.

        /// Binds the selected Pack Source and raw byte-operations provider.
        EdplReader(
            const TPackSource& PackSourceValue,
            const TByteOperationsProvider& ByteOperations
        ) noexcept :
            PackSource_(&PackSourceValue),
            ByteOperations_(&ByteOperations) {}


        // Pack inspection.

        /// Reads current pack layout and language metadata without scanning unrelated payload.
        [[nodiscard]] LocalisationStatus InspectPack(
            const PackResource& Resource,
            LanguageIdentifierView ExpectedLanguage,
            PackLayout& Layout,
            LanguageMetadata& Metadata,
            PayloadDescriptor& Payload
        ) const noexcept {
            const auto LayoutStatus = ReadLayout(
                Resource,
                Layout
            );

            if (LayoutStatus != LocalisationStatus::Success) {
                return LayoutStatus;
            }

            const auto MetadataStatus = ReadLanguageMetadata(
                Resource,
                Layout,
                ExpectedLanguage,
                Metadata
            );

            if (MetadataStatus != LocalisationStatus::Success) {
                return MetadataStatus;
            }

            return ReadPayloadDescriptor(
                Resource,
                Layout,
                Payload
            );
        }


        /// Performs complete current-format structural, integrity, ordering, and UTF-8 validation.
        [[nodiscard]] LocalisationStatus ValidateCompletePack(
            const PackResource& Resource,
            LanguageIdentifierView ExpectedLanguage
        ) const noexcept {
            PackLayout Layout{};
            LanguageMetadata Metadata{};
            PayloadDescriptor Payload{};

            const auto InspectStatus = InspectPack(
                Resource,
                ExpectedLanguage,
                Layout,
                Metadata,
                Payload
            );

            if (InspectStatus != LocalisationStatus::Success) {
                return InspectStatus;
            }

            const auto DirectoryStatus = ValidateCompleteSectionDirectory(
                Resource,
                Layout
            );

            if (DirectoryStatus != LocalisationStatus::Success) {
                return DirectoryStatus;
            }

            const auto CrcStatus = ValidateCrc32c(
                Resource,
                Layout
            );

            if (CrcStatus != LocalisationStatus::Success) {
                return CrcStatus;
            }

            const auto DisplayStatus = ValidateLanguageDisplayNames(
                Resource,
                Layout,
                Payload
            );

            if (DisplayStatus != LocalisationStatus::Success) {
                return DisplayStatus;
            }

            const auto GeneralStatus = ValidateGeneralStrings(
                Resource,
                Layout,
                Payload
            );

            if (GeneralStatus != LocalisationStatus::Success) {
                return GeneralStatus;
            }

            return ValidateTypeSchema(
                Resource,
                Layout,
                Payload
            );
        }


        // Metadata identity access.

        /// Returns a view into Metadata's owned canonical language bytes.
        [[nodiscard]] static LanguageIdentifierView LanguageView(
            const LanguageMetadata& Metadata
        ) noexcept {
            return LanguageIdentifierView::Validate(
                Metadata.LanguageBytes.data(),
                Metadata.LanguageLength
            ).Value;
        }

        /// Returns a view into Metadata's owned parent bytes; caller uses only when ParentLength is non-zero.
        [[nodiscard]] static LanguageIdentifierView ParentView(
            const LanguageMetadata& Metadata
        ) noexcept {
            return LanguageIdentifierView::Validate(
                Metadata.ParentBytes.data(),
                Metadata.ParentLength
            ).Value;
        }


        // General strings.

        /// Locates one general-string representation without copying payload bytes.
        [[nodiscard]] RepresentationLookupResult LookupGeneralString(
            const PackResource& Resource,
            const PackLayout& Layout,
            const PayloadDescriptor& Payload,
            const typename Identifiers::GeneralStringIdentifier& Identifier
        ) const noexcept {
            GeneralStringsHeader Header{};

            const auto HeaderStatus = ReadGeneralStringsHeader(
                Resource,
                Layout,
                Header
            );

            if (HeaderStatus != LocalisationStatus::Success) {
                return {
                    HeaderStatus,
                    {
                        RepresentationState::Absent,
                        0U,
                        0U,
                        false
                    }
                };
            }

            bool DomainFound = false;
            std::uint32_t FirstSubDomain = 0U;
            std::uint32_t SubDomainCount = 0U;

            const auto DomainStatus = FindDomain(
                Resource,
                Layout,
                Header,
                static_cast<std::uint64_t>(Identifier.Domain.Value()),
                DomainFound,
                FirstSubDomain,
                SubDomainCount
            );

            if (DomainStatus != LocalisationStatus::Success) {
                return {
                    DomainStatus,
                    {
                        RepresentationState::Absent,
                        0U,
                        0U,
                        false
                    }
                };
            }

            if (!DomainFound) {
                return {
                    LocalisationStatus::Success,
                    {
                        RepresentationState::Absent,
                        0U,
                        0U,
                        false
                    }
                };
            }

            bool SubDomainFound = false;
            std::uint32_t FirstString = 0U;
            std::uint32_t StringCount = 0U;

            const auto SubDomainStatus = FindSubDomain(
                Resource,
                Layout,
                Header,
                FirstSubDomain,
                SubDomainCount,
                static_cast<std::uint64_t>(Identifier.SubDomain.Value()),
                SubDomainFound,
                FirstString,
                StringCount
            );

            if (SubDomainStatus != LocalisationStatus::Success) {
                return {
                    SubDomainStatus,
                    {
                        RepresentationState::Absent,
                        0U,
                        0U,
                        false
                    }
                };
            }

            if (!SubDomainFound) {
                return {
                    LocalisationStatus::Success,
                    {
                        RepresentationState::Absent,
                        0U,
                        0U,
                        false
                    }
                };
            }

            return FindString(
                Resource,
                Layout,
                Header,
                Payload,
                FirstString,
                StringCount,
                static_cast<std::uint64_t>(Identifier.String.Value())
            );
        }


        // Language display names.

        /// Locates one localised language display-name representation.
        [[nodiscard]] RepresentationLookupResult LookupLanguageDisplayName(
            const PackResource& Resource,
            const PackLayout& Layout,
            const PayloadDescriptor& Payload,
            LanguageIdentifierView TargetLanguage
        ) const noexcept {
            if (
                Layout.LanguageDisplayNames.Length <
                Edpl::LanguageDisplayHeaderBytes
            ) {
                return {
                    LocalisationStatus::InvalidDataset,
                    {
                        RepresentationState::Absent,
                        0U,
                        0U,
                        false
                    }
                };
            }

            std::array<
                std::uint8_t,
                Edpl::LanguageDisplayHeaderBytes
            > Header{};

            const auto HeaderStatus = ReadExact(
                Resource,
                Layout.LanguageDisplayNames.Offset,
                Header.data(),
                Header.size()
            );

            if (HeaderStatus != LocalisationStatus::Success) {
                return {
                    HeaderStatus,
                    {
                        RepresentationState::Absent,
                        0U,
                        0U,
                        false
                    }
                };
            }

            if (
                Header[0U] != Edpl::SectionVersion ||
                Header[1U] != 0U
            ) {
                return {
                    LocalisationStatus::InvalidDataset,
                    {
                        RepresentationState::Absent,
                        0U,
                        0U,
                        false
                    }
                };
            }

            const std::uint16_t EntryCount = ReadUInt16(
                Header.data() + 2U
            );
            const std::uint64_t RequiredLength =
                Edpl::LanguageDisplayHeaderBytes +
                (
                    static_cast<std::uint64_t>(EntryCount) *
                    Edpl::LanguageDisplayEntryBytes
                );

            if (RequiredLength > Layout.LanguageDisplayNames.Length) {
                return {
                    LocalisationStatus::InvalidDataset,
                    {
                        RepresentationState::Absent,
                        0U,
                        0U,
                        false
                    }
                };
            }

            std::array<
                std::uint8_t,
                Edpl::LanguageDisplayEntryBytes
            > Entry{};
            std::array<
                std::uint8_t,
                TContract::MaximumSupportedLanguageIdentifierBytes
            > TargetBytes{};

            for (std::uint16_t Index = 0U; Index < EntryCount; ++Index) {
                const std::uint64_t EntryOffset =
                    static_cast<std::uint64_t>(Layout.LanguageDisplayNames.Offset) +
                    Edpl::LanguageDisplayHeaderBytes +
                    (
                        static_cast<std::uint64_t>(Index) *
                        Edpl::LanguageDisplayEntryBytes
                    );

                const auto EntryStatus = ReadExact(
                    Resource,
                    EntryOffset,
                    Entry.data(),
                    Entry.size()
                );

                if (EntryStatus != LocalisationStatus::Success) {
                    return {
                        EntryStatus,
                        {
                            RepresentationState::Absent,
                            0U,
                            0U,
                            false
                        }
                    };
                }

                if (
                    Entry[5U] != 0U ||
                    Entry[6U] != 0U ||
                    Entry[7U] != 0U
                ) {
                    return {
                        LocalisationStatus::InvalidDataset,
                        {
                            RepresentationState::Absent,
                            0U,
                            0U,
                            false
                        }
                    };
                }

                const std::uint32_t TargetOffset = ReadUInt32(
                    Entry.data()
                );
                const std::uint8_t TargetLength = Entry[4U];
                const std::uint32_t DisplayOffset = ReadUInt32(
                    Entry.data() + 8U
                );
                const std::uint32_t DisplayLength = ReadUInt32(
                    Entry.data() + 12U
                );

                if (
                    TargetLength == 0U ||
                    TargetLength > TContract::MaximumSupportedLanguageIdentifierBytes ||
                    !IsPayloadReferenceValid(
                        Payload,
                        TargetOffset,
                        TargetLength
                    ) ||
                    !IsPayloadReferenceValid(
                        Payload,
                        DisplayOffset,
                        DisplayLength
                    )
                ) {
                    return {
                        LocalisationStatus::InvalidDataset,
                        {
                            RepresentationState::Absent,
                            0U,
                            0U,
                            false
                        }
                    };
                }

                const auto TargetRead = ReadExact(
                    Resource,
                    Payload.PayloadBytesOffset + TargetOffset,
                    TargetBytes.data(),
                    TargetLength
                );

                if (TargetRead != LocalisationStatus::Success) {
                    return {
                        TargetRead,
                        {
                            RepresentationState::Absent,
                            0U,
                            0U,
                            false
                        }
                    };
                }

                const auto TargetValidation = LanguageIdentifierView::Validate(
                    reinterpret_cast<const char*>(TargetBytes.data()),
                    TargetLength
                );

                if (!TargetValidation.IsValuePresent) {
                    return {
                        LocalisationStatus::InvalidDataset,
                        {
                            RepresentationState::Absent,
                            0U,
                            0U,
                            false
                        }
                    };
                }

                if (
                    TargetLength != TargetLanguage.Length() ||
                    ByteOperations_->CompareBytes(
                        TargetBytes.data(),
                        TargetLanguage.Data(),
                        TargetLength
                    ) != ESPressio::Memory::ByteComparison::Equal
                ) {
                    continue;
                }

                return {
                    LocalisationStatus::Success,
                    {
                        DisplayLength == 0U
                            ? RepresentationState::PresentEmpty
                            : RepresentationState::PresentValue,
                        DisplayLength == 0U ? 0U : DisplayOffset,
                        DisplayLength,
                        true
                    }
                };
            }

            return {
                LocalisationStatus::Success,
                {
                    RepresentationState::Absent,
                    0U,
                    0U,
                    false
                }
            };
        }


        // Type presentation.

        /// Locates one Type Name or Description representation.
        [[nodiscard]] RepresentationLookupResult LookupTypeRepresentation(
            const PackResource& Resource,
            const PackLayout& Layout,
            const PayloadDescriptor& Payload,
            const typename Identifiers::TypeIdentifier& Type,
            PresentationRepresentation Representation
        ) const noexcept {
            TypeSchemaHeader Header{};

            const auto HeaderStatus = ReadTypeSchemaHeader(
                Resource,
                Layout,
                Header
            );

            if (HeaderStatus != LocalisationStatus::Success) {
                return {
                    HeaderStatus,
                    {
                        RepresentationState::Absent,
                        0U,
                        0U,
                        false
                    }
                };
            }

            bool Found = false;
            TypeRecordValue Value{};

            const auto TypeStatus = FindType(
                Resource,
                Layout,
                Header,
                Type,
                Found,
                Value
            );

            if (TypeStatus != LocalisationStatus::Success) {
                return {
                    TypeStatus,
                    {
                        RepresentationState::Absent,
                        0U,
                        0U,
                        false
                    }
                };
            }

            return SelectRepresentation(
                Found,
                Value.Flags,
                Value.NameOffset,
                Value.NameLength,
                Value.DescriptionOffset,
                Value.DescriptionLength,
                Representation,
                Payload
            );
        }

        /// Locates one Field Name or Description representation.
        [[nodiscard]] RepresentationLookupResult LookupFieldRepresentation(
            const PackResource& Resource,
            const PackLayout& Layout,
            const PayloadDescriptor& Payload,
            const typename Identifiers::FieldPresentationIdentifier& Field,
            PresentationRepresentation Representation
        ) const noexcept {
            TypeSchemaHeader Header{};

            const auto HeaderStatus = ReadTypeSchemaHeader(
                Resource,
                Layout,
                Header
            );

            if (HeaderStatus != LocalisationStatus::Success) {
                return {
                    HeaderStatus,
                    {
                        RepresentationState::Absent,
                        0U,
                        0U,
                        false
                    }
                };
            }

            bool TypeFound = false;
            TypeRecordValue TypeValue{};

            const auto TypeStatus = FindType(
                Resource,
                Layout,
                Header,
                Field.Type,
                TypeFound,
                TypeValue
            );

            if (TypeStatus != LocalisationStatus::Success) {
                return {
                    TypeStatus,
                    {
                        RepresentationState::Absent,
                        0U,
                        0U,
                        false
                    }
                };
            }

            if (!TypeFound) {
                return {
                    LocalisationStatus::Success,
                    {
                        RepresentationState::Absent,
                        0U,
                        0U,
                        false
                    }
                };
            }

            bool FieldFound = false;
            FieldRecordValue FieldValue{};

            const auto FieldStatus = FindField(
                Resource,
                Layout,
                Header,
                TypeValue,
                static_cast<std::uint64_t>(Field.Field.Value()),
                FieldFound,
                FieldValue
            );

            if (FieldStatus != LocalisationStatus::Success) {
                return {
                    FieldStatus,
                    {
                        RepresentationState::Absent,
                        0U,
                        0U,
                        false
                    }
                };
            }

            return SelectRepresentation(
                FieldFound,
                FieldValue.Flags,
                FieldValue.NameOffset,
                FieldValue.NameLength,
                FieldValue.DescriptionOffset,
                FieldValue.DescriptionLength,
                Representation,
                Payload
            );
        }


        // Payload transfer.

        /// Reads representation payload bytes selected by a previous lookup.
        [[nodiscard]] LocalisationStatus ReadPayload(
            const PackResource& Resource,
            const PayloadDescriptor& Payload,
            const RepresentationLocation& Representation,
            std::uint8_t* Destination,
            std::size_t ByteCount
        ) const noexcept {
            if (
                Representation.State != RepresentationState::PresentValue ||
                ByteCount > Representation.PayloadLength ||
                !IsPayloadReferenceValid(
                    Payload,
                    Representation.PayloadOffset,
                    static_cast<std::uint32_t>(ByteCount)
                )
            ) {
                return LocalisationStatus::InvalidDataset;
            }

            return ReadExact(
                Resource,
                Payload.PayloadBytesOffset + Representation.PayloadOffset,
                Destination,
                ByteCount
            );
        }

    };

} // ESPressio::Localisation::Detail
