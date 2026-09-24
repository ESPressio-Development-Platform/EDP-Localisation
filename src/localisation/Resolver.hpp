#pragma once

#include <ESPressio_Memory.hpp>
#include <array>
#include <cstddef>
#include <cstdint>
#include <optional>


#include "LanguageIdentifierView.hpp"
#include "LocalisationContext.hpp"
#include "LocalisationTypes.hpp"
#include "PackSourceContract.hpp"
#include "detail/EdplReader.hpp"

namespace ESPressio::Localisation {

    /// Stateless V1 Localisation resolver over one selected Pack Source and generated ContractFamily.
    ///
    /// @tparam TPackSource Concrete provider satisfying the Localisation Pack Source contract.
    /// @tparam TByteOperationsProvider Concrete provider satisfying EDP-Memory ByteOperations.
    /// @tparam TContract Generated Localisation ContractFamily descriptor.
    template<
        class TPackSource,
        class TByteOperationsProvider,
        class TContract
    >
    class Resolver final {
    private:

        static_assert(
            PackSourceProvider<TPackSource>,
            "Resolver requires a conforming Localisation Pack Source"
        );

        static_assert(
            sizeof(
                ESPressio::Memory::Detail::ByteOperationsProviderTraits<
                    TByteOperationsProvider
                >
            ) > 0U,
            "Resolver requires an EDP-Memory ByteOperations provider"
        );

        static_assert(
            TContract::SupportedLanguageCount > 0U,
            "Generated Localisation contract must contain at least one supported language"
        );


        /// Provider-associated immutable pack resource type.
        using PackResource = typename TPackSource::PackResource;

        /// Current-format EDPL reader bound to this Resolver specialization.
        using Reader = Detail::EdplReader<
            TPackSource,
            TByteOperationsProvider,
            TContract
        >;


        /// Internal contract-derived identifier vocabulary.
        using IdentifierVocabulary = ContractIdentifiers<TContract>;


        /// Internal opaque runtime handle value exposed through the public LanguageHandle alias.
        class LanguageHandleValue final {
        private:

            // Stable supplying resource.

            /// Provider-associated resource whose language supplied the representation.
            PackResource Resource_;

            /// Constructs a successful handle from one stable PackResource.
            explicit LanguageHandleValue(
                const PackResource& Resource
            ) noexcept :
                Resource_(Resource) {}

            /// Grants this Resolver specialization handle-construction access.
            friend class Resolver<
                TPackSource,
                TByteOperationsProvider,
                TContract
            >;

        public:

            /// Copies one runtime language handle.
            LanguageHandleValue(const LanguageHandleValue&) noexcept = default;

            /// Replaces one runtime language handle.
            LanguageHandleValue& operator=(const LanguageHandleValue&) noexcept = default;

            /// Destroys the wrapped provider-associated resource.
            ~LanguageHandleValue() = default;

        };


        /// Internal result value exposed through the public ResolveResult alias.
        struct ResolveResultValue final {

            /// Mutually exclusive resolution outcome.
            LocalisationStatus Status;

            /// Orthogonal facts describing one successful resolution.
            LocalisationFacts Facts;

            /// Number of UTF-8 payload bytes written, excluding an optional NUL terminator.
            std::size_t BytesWritten;

            /// Complete UTF-8 payload byte count required, excluding an optional NUL terminator.
            std::size_t RequiredBytes;

            /// Supplying language handle present only when Status is Success.
            std::optional<LanguageHandleValue> ResolvedLanguage;

        };


        /// Result of analysing the largest complete UTF-8 prefix in one bounded byte range.
        struct Utf8PrefixAnalysis final {

            /// Indicates whether every fully observed code point is valid UTF-8.
            bool IsValid;

            /// Largest complete UTF-8 prefix in the inspected byte range.
            std::size_t CompleteBytes;

        };


        // Bound providers.

        /// Pack Source supplying current ContractFamily language-pack bytes.
        const TPackSource* PackSource_;

        /// Bootstrap-owned raw byte operations provider.
        const TByteOperationsProvider* ByteOperations_;

        /// Current-release EDPL reader bound to the same providers.
        Reader Reader_;


        // Argument validation.

        /// Indicates whether Mode is one of the two public V1 text-output modes.
        [[nodiscard]] static constexpr bool IsOutputModeValid(
            TextOutputMode Mode
        ) noexcept {
            return
                Mode == TextOutputMode::RawUtf8 ||
                Mode == TextOutputMode::NullTerminatedUtf8;
        }

        /// Indicates whether Destination satisfies its caller-owned pointer/capacity invariant.
        [[nodiscard]] static constexpr bool IsDestinationValid(
            WritableTextView Destination
        ) noexcept {
            return
                Destination.Capacity == 0U ||
                Destination.Data != nullptr;
        }


        // Result construction.

        /// Creates one normalized failed Resolve result.
        [[nodiscard]] static ResolveResultValue MakeFailure(
            LocalisationStatus Status
        ) noexcept {
            return {
                Status,
                LocalisationFacts{},
                0U,
                0U,
                std::nullopt
            };
        }

        /// Maps a Pack Source locate failure into Localisation semantics.
        [[nodiscard]] static constexpr LocalisationStatus MapLocateStatus(
            PackLocateStatus Status
        ) noexcept {
            switch (Status) {
                case PackLocateStatus::Success:
                    return LocalisationStatus::Success;

                case PackLocateStatus::PackUnavailable:
                    return LocalisationStatus::LanguagePackUnavailable;

                case PackLocateStatus::ProviderUnavailable:
                    return LocalisationStatus::ProviderUnavailable;

                case PackLocateStatus::ReadFailure:
                    return LocalisationStatus::ReadFailure;

                case PackLocateStatus::InvalidArgument:
                    return LocalisationStatus::InvalidArgument;
            }

            return LocalisationStatus::ReadFailure;
        }

        /// Maps the Resolver's broader LocalisationStatus into validation-specific vocabulary.
        [[nodiscard]] static constexpr ValidationStatus MapValidationStatus(
            LocalisationStatus Status
        ) noexcept {
            switch (Status) {
                case LocalisationStatus::Success:
                    return ValidationStatus::Success;

                case LocalisationStatus::LanguagePackUnavailable:
                    return ValidationStatus::LanguagePackUnavailable;

                case LocalisationStatus::ProviderUnavailable:
                    return ValidationStatus::ProviderUnavailable;

                case LocalisationStatus::UnsupportedFormatVersion:
                case LocalisationStatus::IncompatibleLanguagePack:
                    return ValidationStatus::IncompatibleLanguagePack;

                case LocalisationStatus::InvalidDataset:
                case LocalisationStatus::NoStringFoundForIdentifier:
                    return ValidationStatus::InvalidDataset;

                case LocalisationStatus::ReadFailure:
                    return ValidationStatus::ReadFailure;

                case LocalisationStatus::InvalidArgument:
                    return ValidationStatus::InvalidArgument;
            }

            return ValidationStatus::ReadFailure;
        }


        // UTF-8 materialisation.

        /// Analyses an already-read UTF-8 prefix and optionally tolerates one incomplete trailing code point.
        [[nodiscard]] static constexpr Utf8PrefixAnalysis AnalyseUtf8Prefix(
            const char* Data,
            std::size_t Size,
            bool AllowIncompleteTrailingCodePoint
        ) noexcept {
            if (
                Size != 0U &&
                Data == nullptr
            ) {
                return {
                    false,
                    0U
                };
            }

            std::size_t Index = 0U;
            std::size_t CompleteBytes = 0U;

            while (Index < Size) {
                const std::size_t CodePointStart = Index;
                const std::uint8_t First =
                    static_cast<std::uint8_t>(Data[Index]);

                if (First == 0U) {
                    return {
                        false,
                        CompleteBytes
                    };
                }

                if (First <= 0x7FU) {
                    ++Index;
                    CompleteBytes = Index;
                    continue;
                }

                std::size_t Length = 0U;
                std::uint32_t CodePoint = 0U;
                std::uint32_t Minimum = 0U;

                if ((First & 0xE0U) == 0xC0U) {
                    Length = 2U;
                    CodePoint = First & 0x1FU;
                    Minimum = 0x80U;
                } else if ((First & 0xF0U) == 0xE0U) {
                    Length = 3U;
                    CodePoint = First & 0x0FU;
                    Minimum = 0x800U;
                } else if ((First & 0xF8U) == 0xF0U) {
                    Length = 4U;
                    CodePoint = First & 0x07U;
                    Minimum = 0x10000U;
                } else {
                    return {
                        false,
                        CompleteBytes
                    };
                }

                if (CodePointStart + Length > Size) {
                    return AllowIncompleteTrailingCodePoint
                        ? Utf8PrefixAnalysis{
                            true,
                            CodePointStart
                        }
                        : Utf8PrefixAnalysis{
                            false,
                            CompleteBytes
                        };
                }

                for (std::size_t Offset = 1U; Offset < Length; ++Offset) {
                    const std::uint8_t Byte = static_cast<std::uint8_t>(
                        Data[CodePointStart + Offset]
                    );

                    if ((Byte & 0xC0U) != 0x80U) {
                        return {
                            false,
                            CompleteBytes
                        };
                    }

                    CodePoint =
                        (CodePoint << 6U) |
                        static_cast<std::uint32_t>(Byte & 0x3FU);
                }

                if (
                    CodePoint < Minimum ||
                    CodePoint > 0x10FFFFU ||
                    (
                        CodePoint >= 0xD800U &&
                        CodePoint <= 0xDFFFU
                    )
                ) {
                    return {
                        false,
                        CompleteBytes
                    };
                }

                Index += Length;
                CompleteBytes = Index;
            }

            return {
                true,
                CompleteBytes
            };
        }

        /// Copies one selected representation into caller-owned text storage.
        [[nodiscard]] ResolveResultValue MaterialiseRepresentation(
            const PackResource& Resource,
            const Detail::PayloadDescriptor& Payload,
            const Detail::RepresentationLocation& Representation,
            LanguageIdentifierView RequestedLanguage,
            WritableTextView Destination,
            TextOutputMode OutputMode
        ) const noexcept {
            const std::size_t RequiredBytes =
                Representation.PayloadLength;
            const std::size_t PayloadCapacity =
                OutputMode == TextOutputMode::NullTerminatedUtf8
                    ? (
                        Destination.Capacity == 0U
                            ? 0U
                            : Destination.Capacity - 1U
                    )
                    : Destination.Capacity;
            const bool IsBufferTooSmall =
                RequiredBytes > PayloadCapacity ||
                (
                    OutputMode == TextOutputMode::NullTerminatedUtf8 &&
                    Destination.Capacity == 0U
                );
            const std::size_t RequestedReadBytes =
                RequiredBytes < PayloadCapacity
                    ? RequiredBytes
                    : PayloadCapacity;

            std::size_t BytesWritten = 0U;

            if (
                Representation.State == Detail::RepresentationState::PresentValue &&
                RequestedReadBytes != 0U
            ) {
                const auto ReadStatus = Reader_.ReadPayload(
                    Resource,
                    Payload,
                    Representation,
                    reinterpret_cast<std::uint8_t*>(
                        Destination.Data
                    ),
                    RequestedReadBytes
                );

                if (ReadStatus != LocalisationStatus::Success) {
                    return MakeFailure(ReadStatus);
                }

                const auto Analysis = AnalyseUtf8Prefix(
                    Destination.Data,
                    RequestedReadBytes,
                    IsBufferTooSmall
                );

                if (!Analysis.IsValid) {
                    return MakeFailure(
                        LocalisationStatus::InvalidDataset
                    );
                }

                BytesWritten = Analysis.CompleteBytes;
            }

            if (
                OutputMode == TextOutputMode::NullTerminatedUtf8 &&
                Destination.Capacity != 0U
            ) {
                Destination.Data[BytesWritten] = '\0';
            }

            LocalisationFacts Facts;

            if (IsBufferTooSmall) {
                Facts.Set(
                    LocalisationFact::BufferTooSmall
                );
            }

            const auto SupplyingLanguage =
                PackSource_->LanguageIdentity(Resource);

            if (!SupplyingLanguage.IsEqualTo(RequestedLanguage)) {
                Facts.Set(
                    LocalisationFact::LanguageFallbackUsed
                );
            }

            return {
                LocalisationStatus::Success,
                Facts,
                BytesWritten,
                RequiredBytes,
                LanguageHandleValue(Resource)
            };
        }


        // Terminal validation.

        /// Performs the touched-data validation required to establish the caller's terminal contract boundary.
        [[nodiscard]] LocalisationStatus ValidateTerminalForResolve(
            const LocalisationContext& Context
        ) const noexcept {
            const auto Located = PackSource_->Locate(
                Context.TerminalLanguage
            );

            if (Located.Status != PackLocateStatus::Success) {
                return MapLocateStatus(
                    Located.Status
                );
            }

            if (!Located.Resource.has_value()) {
                return LocalisationStatus::ReadFailure;
            }

            Detail::PackLayout Layout{};
            typename Reader::LanguageMetadata Metadata{};
            Detail::PayloadDescriptor Payload{};

            const auto Status = Reader_.InspectPack(
                *Located.Resource,
                Context.TerminalLanguage,
                Layout,
                Metadata,
                Payload
            );

            if (Status != LocalisationStatus::Success) {
                return Status;
            }

            return
                Metadata.IsTerminal &&
                Metadata.ParentLength == 0U
                ? LocalisationStatus::Success
                : LocalisationStatus::IncompatibleLanguagePack;
        }


        // Generic fallback resolution.

        /// Resolves one representation surface through the explicit parent chain.
        ///
        /// @tparam TLookup Callable accepting Resource, Layout and Payload and returning RepresentationLookupResult.
        template<class TLookup>
        [[nodiscard]] ResolveResultValue ResolveRepresentation(
            const LocalisationContext& Context,
            WritableTextView Destination,
            TextOutputMode OutputMode,
            bool RequireTerminalNameWhenEntityExists,
            TLookup&& Lookup
        ) const noexcept {
            const auto ContextValidation = ValidateLocalisationContext(
                Context
            );

            if (
                ContextValidation.Status !=
                LocalisationContextValidationStatus::Succeeded ||
                !IsDestinationValid(Destination) ||
                !IsOutputModeValid(OutputMode)
            ) {
                return MakeFailure(
                    LocalisationStatus::InvalidArgument
                );
            }

            const auto TerminalStatus = ValidateTerminalForResolve(
                Context
            );

            if (TerminalStatus != LocalisationStatus::Success) {
                return MakeFailure(TerminalStatus);
            }

            std::array<
                char,
                TContract::MaximumSupportedLanguageIdentifierBytes
            > ParentScratch{};
            LanguageIdentifierView CurrentLanguage =
                Context.RequestedLanguage;

            for (
                std::size_t Traversal = 0U;
                Traversal < TContract::SupportedLanguageCount;
                ++Traversal
            ) {
                const auto Located = PackSource_->Locate(
                    CurrentLanguage
                );

                if (Located.Status != PackLocateStatus::Success) {
                    return MakeFailure(
                        MapLocateStatus(
                            Located.Status
                        )
                    );
                }

                if (!Located.Resource.has_value()) {
                    return MakeFailure(
                        LocalisationStatus::ReadFailure
                    );
                }

                const auto& Resource = *Located.Resource;
                Detail::PackLayout Layout{};
                typename Reader::LanguageMetadata Metadata{};
                Detail::PayloadDescriptor Payload{};

                const auto InspectStatus = Reader_.InspectPack(
                    Resource,
                    CurrentLanguage,
                    Layout,
                    Metadata,
                    Payload
                );

                if (InspectStatus != LocalisationStatus::Success) {
                    return MakeFailure(InspectStatus);
                }

                const bool IsTerminalLanguage =
                    CurrentLanguage.IsEqualTo(
                        Context.TerminalLanguage
                    );

                if (
                    IsTerminalLanguage &&
                    (
                        !Metadata.IsTerminal ||
                        Metadata.ParentLength != 0U
                    )
                ) {
                    return MakeFailure(
                        LocalisationStatus::IncompatibleLanguagePack
                    );
                }

                if (
                    !IsTerminalLanguage &&
                    Metadata.IsTerminal
                ) {
                    return MakeFailure(
                        LocalisationStatus::IncompatibleLanguagePack
                    );
                }

                const auto LookupResult = Lookup(
                    Resource,
                    Layout,
                    Payload
                );

                if (
                    LookupResult.Status !=
                    LocalisationStatus::Success
                ) {
                    return MakeFailure(
                        LookupResult.Status
                    );
                }

                if (!LookupResult.Representation.has_value()) {
                    return MakeFailure(
                        LocalisationStatus::InvalidDataset
                    );
                }

                const auto& Representation =
                    *LookupResult.Representation;

                if (
                    Representation.State !=
                    Detail::RepresentationState::Absent
                ) {
                    return MaterialiseRepresentation(
                        Resource,
                        Payload,
                        Representation,
                        Context.RequestedLanguage,
                        Destination,
                        OutputMode
                    );
                }

                if (IsTerminalLanguage) {
                    if (
                        RequireTerminalNameWhenEntityExists &&
                        Representation.IsEntityPresent
                    ) {
                        return MakeFailure(
                            LocalisationStatus::InvalidDataset
                        );
                    }

                    return MakeFailure(
                        LocalisationStatus::NoStringFoundForIdentifier
                    );
                }

                if (Metadata.ParentLength == 0U) {
                    return MakeFailure(
                        LocalisationStatus::InvalidDataset
                    );
                }

                ByteOperations_->CopyBytes(
                    ParentScratch.data(),
                    Metadata.ParentBytes.data(),
                    Metadata.ParentLength
                );

                const auto ParentValidation =
                    LanguageIdentifierView::Validate(
                        ParentScratch.data(),
                        Metadata.ParentLength
                    );

                if (!ParentValidation.IsValuePresent) {
                    return MakeFailure(
                        LocalisationStatus::InvalidDataset
                    );
                }

                CurrentLanguage = ParentValidation.Value;
            }

            return MakeFailure(
                LocalisationStatus::InvalidDataset
            );
        }


        // Validation traversal.

        /// Fully validates one already-located language pack and returns its metadata for chain traversal.
        [[nodiscard]] ValidationStatus ValidatePackForContext(
            LanguageIdentifierView Language,
            typename Reader::LanguageMetadata& Metadata
        ) const noexcept {
            const auto Located = PackSource_->Locate(
                Language
            );

            if (Located.Status != PackLocateStatus::Success) {
                return MapValidationStatus(
                    MapLocateStatus(
                        Located.Status
                    )
                );
            }

            if (!Located.Resource.has_value()) {
                return ValidationStatus::ReadFailure;
            }

            const auto Validation = Reader_.ValidateCompletePack(
                *Located.Resource,
                Language
            );

            if (Validation != LocalisationStatus::Success) {
                return MapValidationStatus(Validation);
            }

            Detail::PackLayout Layout{};
            Detail::PayloadDescriptor Payload{};

            return MapValidationStatus(
                Reader_.InspectPack(
                    *Located.Resource,
                    Language,
                    Layout,
                    Metadata,
                    Payload
                )
            );
        }

    public:

        /// Contract-derived identifier vocabulary used by this Resolver specialization.
        using Identifiers = IdentifierVocabulary;

        /// Strong general-string lookup identity.
        using GeneralStringIdentifier =
            typename Identifiers::GeneralStringIdentifier;

        /// Strong globally unique Type identity.
        using TypeIdentifier =
            typename Identifiers::TypeIdentifier;

        /// Strong Type-local Field presentation identity.
        using FieldPresentationIdentifier =
            typename Identifiers::FieldPresentationIdentifier;

        /// Consolidated compile-time dependency contract of one Resolver consumer.
        using CompositionContract = ResolverContract;

        /// Opaque runtime handle identifying the pack that supplied a successful representation.
        using LanguageHandle = LanguageHandleValue;

        /// Result returned by every Localisation representation lookup.
        using ResolveResult = ResolveResultValue;


        // Construction.

        /// Binds the stateless Resolver to Bootstrap-owned Pack Source and ByteOperations providers.
        Resolver(
            const TPackSource& PackSourceValue,
            const TByteOperationsProvider& ByteOperations
        ) noexcept :
            PackSource_(&PackSourceValue),
            ByteOperations_(&ByteOperations),
            Reader_(
                PackSourceValue,
                ByteOperations
            ) {}


        // General-string resolution.

        /// Resolves one Domain/SubDomain/String presentation value through explicit fallback.
        [[nodiscard]] ResolveResult ResolveString(
            const LocalisationContext& Context,
            const GeneralStringIdentifier& Identifier,
            WritableTextView Destination,
            TextOutputMode OutputMode
        ) const noexcept {
            return ResolveRepresentation(
                Context,
                Destination,
                OutputMode,
                false,
                [
                    this,
                    &Identifier
                ](
                    const PackResource& Resource,
                    const Detail::PackLayout& Layout,
                    const Detail::PayloadDescriptor& Payload
                ) noexcept {
                    return Reader_.LookupGeneralString(
                        Resource,
                        Layout,
                        Payload,
                        Identifier
                    );
                }
            );
        }


        // Language display-name resolution.

        /// Resolves the localised display name of TargetLanguage using Context's requested language.
        [[nodiscard]] ResolveResult ResolveLanguageDisplayName(
            const LocalisationContext& Context,
            LanguageIdentifierView TargetLanguage,
            WritableTextView Destination,
            TextOutputMode OutputMode
        ) const noexcept {
            const auto TargetValidation =
                LanguageIdentifierView::Validate(
                    TargetLanguage.Data(),
                    TargetLanguage.Length()
                );

            if (!TargetValidation.IsValuePresent) {
                return MakeFailure(
                    LocalisationStatus::InvalidArgument
                );
            }

            return ResolveRepresentation(
                Context,
                Destination,
                OutputMode,
                false,
                [
                    this,
                    TargetLanguage
                ](
                    const PackResource& Resource,
                    const Detail::PackLayout& Layout,
                    const Detail::PayloadDescriptor& Payload
                ) noexcept {
                    return Reader_.LookupLanguageDisplayName(
                        Resource,
                        Layout,
                        Payload,
                        TargetLanguage
                    );
                }
            );
        }


        // Type presentation resolution.

        /// Resolves one Type Name through explicit language fallback.
        [[nodiscard]] ResolveResult ResolveTypeName(
            const LocalisationContext& Context,
            TypeIdentifier Type,
            WritableTextView Destination,
            TextOutputMode OutputMode
        ) const noexcept {
            if constexpr (TContract::TypeIdentifierBytes != 0U) {
                if (!Type.IsValid()) {
                    return MakeFailure(
                        LocalisationStatus::InvalidArgument
                    );
                }
            }

            return ResolveRepresentation(
                Context,
                Destination,
                OutputMode,
                true,
                [
                    this,
                    Type
                ](
                    const PackResource& Resource,
                    const Detail::PackLayout& Layout,
                    const Detail::PayloadDescriptor& Payload
                ) noexcept {
                    return Reader_.LookupTypeRepresentation(
                        Resource,
                        Layout,
                        Payload,
                        Type,
                        Detail::PresentationRepresentation::Name
                    );
                }
            );
        }

        /// Resolves one optional Type Description through explicit language fallback.
        [[nodiscard]] ResolveResult ResolveTypeDescription(
            const LocalisationContext& Context,
            TypeIdentifier Type,
            WritableTextView Destination,
            TextOutputMode OutputMode
        ) const noexcept {
            if constexpr (TContract::TypeIdentifierBytes != 0U) {
                if (!Type.IsValid()) {
                    return MakeFailure(
                        LocalisationStatus::InvalidArgument
                    );
                }
            }

            return ResolveRepresentation(
                Context,
                Destination,
                OutputMode,
                false,
                [
                    this,
                    Type
                ](
                    const PackResource& Resource,
                    const Detail::PackLayout& Layout,
                    const Detail::PayloadDescriptor& Payload
                ) noexcept {
                    return Reader_.LookupTypeRepresentation(
                        Resource,
                        Layout,
                        Payload,
                        Type,
                        Detail::PresentationRepresentation::Description
                    );
                }
            );
        }


        // Field presentation resolution.

        /// Resolves one Field Name through explicit language fallback.
        [[nodiscard]] ResolveResult ResolveFieldName(
            const LocalisationContext& Context,
            const FieldPresentationIdentifier& Field,
            WritableTextView Destination,
            TextOutputMode OutputMode
        ) const noexcept {
            if constexpr (TContract::TypeIdentifierBytes != 0U) {
                if (!Field.Type.IsValid()) {
                    return MakeFailure(
                        LocalisationStatus::InvalidArgument
                    );
                }
            }

            return ResolveRepresentation(
                Context,
                Destination,
                OutputMode,
                true,
                [
                    this,
                    &Field
                ](
                    const PackResource& Resource,
                    const Detail::PackLayout& Layout,
                    const Detail::PayloadDescriptor& Payload
                ) noexcept {
                    return Reader_.LookupFieldRepresentation(
                        Resource,
                        Layout,
                        Payload,
                        Field,
                        Detail::PresentationRepresentation::Name
                    );
                }
            );
        }

        /// Resolves one optional Field Description through explicit language fallback.
        [[nodiscard]] ResolveResult ResolveFieldDescription(
            const LocalisationContext& Context,
            const FieldPresentationIdentifier& Field,
            WritableTextView Destination,
            TextOutputMode OutputMode
        ) const noexcept {
            if constexpr (TContract::TypeIdentifierBytes != 0U) {
                if (!Field.Type.IsValid()) {
                    return MakeFailure(
                        LocalisationStatus::InvalidArgument
                    );
                }
            }

            return ResolveRepresentation(
                Context,
                Destination,
                OutputMode,
                false,
                [
                    this,
                    &Field
                ](
                    const PackResource& Resource,
                    const Detail::PackLayout& Layout,
                    const Detail::PayloadDescriptor& Payload
                ) noexcept {
                    return Reader_.LookupFieldRepresentation(
                        Resource,
                        Layout,
                        Payload,
                        Field,
                        Detail::PresentationRepresentation::Description
                    );
                }
            );
        }


        // Resolved-language materialisation.

        /// Copies the canonical BCP47 identity represented by Language into caller-owned storage.
        [[nodiscard]] TextMaterialisationResult ResolveLanguageIdentity(
            const LanguageHandle& Language,
            WritableTextView Destination,
            TextOutputMode OutputMode
        ) const noexcept {
            if (
                !IsDestinationValid(Destination) ||
                !IsOutputModeValid(OutputMode)
            ) {
                return {
                    TextMaterialisationStatus::InvalidArgument,
                    TextMaterialisationFacts{},
                    0U,
                    0U
                };
            }

            const auto Identity = PackSource_->LanguageIdentity(
                Language.Resource_
            );
            const std::size_t RequiredBytes =
                Identity.Length();
            const std::size_t PayloadCapacity =
                OutputMode == TextOutputMode::NullTerminatedUtf8
                    ? (
                        Destination.Capacity == 0U
                            ? 0U
                            : Destination.Capacity - 1U
                    )
                    : Destination.Capacity;
            const std::size_t BytesWritten =
                RequiredBytes < PayloadCapacity
                    ? RequiredBytes
                    : PayloadCapacity;
            const bool IsBufferTooSmall =
                RequiredBytes > PayloadCapacity ||
                (
                    OutputMode == TextOutputMode::NullTerminatedUtf8 &&
                    Destination.Capacity == 0U
                );

            if (BytesWritten != 0U) {
                ByteOperations_->CopyBytes(
                    Destination.Data,
                    Identity.Data(),
                    BytesWritten
                );
            }

            if (
                OutputMode == TextOutputMode::NullTerminatedUtf8 &&
                Destination.Capacity != 0U
            ) {
                Destination.Data[BytesWritten] = '\0';
            }

            TextMaterialisationFacts Facts;

            if (IsBufferTooSmall) {
                Facts.Set(
                    TextMaterialisationFact::BufferTooSmall
                );
            }

            return {
                TextMaterialisationStatus::Success,
                Facts,
                BytesWritten,
                RequiredBytes
            };
        }


        // Explicit validation.

        /// Performs complete current-format validation of one language pack.
        [[nodiscard]] ValidationResult ValidateLanguagePack(
            LanguageIdentifierView Language
        ) const noexcept {
            const auto LanguageValidation =
                LanguageIdentifierView::Validate(
                    Language.Data(),
                    Language.Length()
                );

            if (!LanguageValidation.IsValuePresent) {
                return {
                    ValidationStatus::InvalidArgument
                };
            }

            const auto Located = PackSource_->Locate(
                Language
            );

            if (Located.Status != PackLocateStatus::Success) {
                return {
                    MapValidationStatus(
                        MapLocateStatus(
                            Located.Status
                        )
                    )
                };
            }

            if (!Located.Resource.has_value()) {
                return {
                    ValidationStatus::ReadFailure
                };
            }

            return {
                MapValidationStatus(
                    Reader_.ValidateCompletePack(
                        *Located.Resource,
                        Language
                    )
                )
            };
        }

        /// Fully validates the requested-to-terminal fallback chain supplied by Context.
        [[nodiscard]] ValidationResult ValidateContext(
            const LocalisationContext& Context
        ) const noexcept {
            const auto ContextValidation =
                ValidateLocalisationContext(
                    Context
                );

            if (
                ContextValidation.Status !=
                LocalisationContextValidationStatus::Succeeded
            ) {
                return {
                    ValidationStatus::InvalidArgument
                };
            }

            typename Reader::LanguageMetadata TerminalMetadata{};
            const auto TerminalStatus = ValidatePackForContext(
                Context.TerminalLanguage,
                TerminalMetadata
            );

            if (TerminalStatus != ValidationStatus::Success) {
                return {
                    TerminalStatus
                };
            }

            if (
                !TerminalMetadata.IsTerminal ||
                TerminalMetadata.ParentLength != 0U
            ) {
                return {
                    ValidationStatus::IncompatibleLanguagePack
                };
            }

            if (
                Context.RequestedLanguage.IsEqualTo(
                    Context.TerminalLanguage
                )
            ) {
                return {
                    ValidationStatus::Success
                };
            }

            std::array<
                char,
                TContract::MaximumSupportedLanguageIdentifierBytes
            > ParentScratch{};
            LanguageIdentifierView CurrentLanguage =
                Context.RequestedLanguage;

            for (
                std::size_t Traversal = 0U;
                Traversal < TContract::SupportedLanguageCount;
                ++Traversal
            ) {
                typename Reader::LanguageMetadata Metadata{};
                const auto Status = ValidatePackForContext(
                    CurrentLanguage,
                    Metadata
                );

                if (Status != ValidationStatus::Success) {
                    return {
                        Status
                    };
                }

                const bool IsTerminalLanguage =
                    CurrentLanguage.IsEqualTo(
                        Context.TerminalLanguage
                    );

                if (IsTerminalLanguage) {
                    return {
                        Metadata.IsTerminal &&
                        Metadata.ParentLength == 0U
                            ? ValidationStatus::Success
                            : ValidationStatus::IncompatibleLanguagePack
                    };
                }

                if (
                    Metadata.IsTerminal ||
                    Metadata.ParentLength == 0U
                ) {
                    return {
                        ValidationStatus::InvalidDataset
                    };
                }

                ByteOperations_->CopyBytes(
                    ParentScratch.data(),
                    Metadata.ParentBytes.data(),
                    Metadata.ParentLength
                );

                const auto ParentValidation =
                    LanguageIdentifierView::Validate(
                        ParentScratch.data(),
                        Metadata.ParentLength
                    );

                if (!ParentValidation.IsValuePresent) {
                    return {
                        ValidationStatus::InvalidDataset
                    };
                }

                CurrentLanguage = ParentValidation.Value;
            }

            return {
                ValidationStatus::InvalidDataset
            };
        }

    };

} // ESPressio::Localisation
