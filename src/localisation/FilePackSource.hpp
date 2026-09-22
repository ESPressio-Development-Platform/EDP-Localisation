#pragma once

#include <array>
#include <cstddef>
#include <cstdint>
#include <optional>
#include <type_traits>

#include <ESPressio_Persistence.hpp>
#include <memory/ByteOperationsContract.hpp>
#include <memory/MemoryTypes.hpp>

#include "LanguageIdentifierView.hpp"
#include "LocalisationComposition.hpp"
#include "LocalisationTypes.hpp"
#include "PackSourceContract.hpp"

namespace ESPressio::Localisation {

    /// Compile-time provider-relative directory used by FilePackSource.
    ///
    /// An empty value selects the FileStorage provider root. Non-empty values must satisfy
    /// EDP-Persistence FilePathView canonical path rules and must not end in a separator.
    ///
    /// @tparam TExtent String-literal extent including the terminating NUL byte.
    template<std::size_t TExtent>
    struct StaticPackDirectory final {

        static_assert(
            TExtent > 0U,
            "StaticPackDirectory requires a string-literal extent including its terminator"
        );


        /// Provider-relative directory bytes excluding the source literal terminator.
        std::array<char, TExtent - 1U> Bytes{};


        /// Copies one compile-time string literal into this structural value.
        consteval StaticPackDirectory(
            const char (&Value)[TExtent]
        ) noexcept {
            for (std::size_t Index = 0U; Index + 1U < TExtent; ++Index) {
                Bytes[Index] = Value[Index];
            }
        }


        /// Returns the exact provider-relative directory byte count.
        [[nodiscard]] constexpr std::size_t Size() const noexcept {
            return Bytes.size();
        }

    };


    /// Deduces StaticPackDirectory extent from one string literal.
    ///
    /// @tparam TExtent String-literal extent including the terminating NUL byte.
    template<std::size_t TExtent>
    StaticPackDirectory(const char (&)[TExtent]) -> StaticPackDirectory<TExtent>;


    /// Consolidated external FileStorage Requirement used by FilePackSource.
    ///
    /// @tparam TPackDirectory Compile-time provider-relative pack directory.
    /// @tparam TLocalisationContract Generated Localisation ContractFamily descriptor.
    /// @tparam TStorageSelectionConstraints Additional FileStorage qualification constraints/Attributes.
    template<
        StaticPackDirectory TPackDirectory,
        class TLocalisationContract,
        class... TStorageSelectionConstraints
    >
    using FileStorageRequirement = Framework::Requirement<
        ESPressio::Persistence::FileStorage,
        Framework::RequirementScope::ExternalDomain,
        Framework::ExactlyProviders<1U>,
        Framework::AtLeast<
            ESPressio::Persistence::FileInvocationConcurrency,
            ESPressio::Persistence::InvocationConcurrency::ConcurrentReads
        >,
        Framework::AtLeast<
            ESPressio::Persistence::MaximumPathBytes,
            TPackDirectory.Size() +
                (TPackDirectory.Size() == 0U ? 0U : 1U) +
                TLocalisationContract::MaximumSupportedLanguageIdentifierBytes +
                7U
        >,
        TStorageSelectionConstraints...
    >;


    /// Generic file-backed Localisation Pack Source over one qualified EDP-Persistence FileStorage provider.
    ///
    /// @tparam TPersistenceComposition Persistence Composition from which one FileStorage provider is selected.
    /// @tparam TPackDirectory Compile-time provider-relative pack directory; empty selects provider root.
    /// @tparam TLocalisationContract Generated Localisation ContractFamily descriptor.
    /// @tparam TByteOperationsProvider Concrete EDP-Memory ByteOperations provider supplied by Bootstrap.
    /// @tparam TStorageSelectionConstraints Additional FileStorage Requirement constraints, including optional Attributes.
    template<
        class TPersistenceComposition,
        StaticPackDirectory TPackDirectory,
        class TLocalisationContract,
        class TByteOperationsProvider,
        class... TStorageSelectionConstraints
    >
    class FilePackSource final : public Framework::Provider<
        Domain,
        Framework::Provides<
            Framework::Offer<PackSource>
        >,
        Framework::Contract<
            ExternalByteOperationsRequirement,
            FileStorageRequirement<
                TPackDirectory,
                TLocalisationContract,
                TStorageSelectionConstraints...
            >
        >
    > {
    private:

        static_assert(
            TLocalisationContract::MaximumSupportedLanguageIdentifierBytes > 0U &&
            TLocalisationContract::MaximumSupportedLanguageIdentifierBytes <= 255U,
            "FilePackSource requires a generated maximum language-identifier length in the range 1..255"
        );

        static_assert(
            sizeof(
                ESPressio::Memory::Detail::ByteOperationsProviderTraits<
                    TByteOperationsProvider
                >
            ) > 0U,
            "FilePackSource requires an EDP-Memory ByteOperations provider"
        );


        /// Persisted pack filename suffix.
        inline static constexpr std::array<char, 7U> PackExtension_ = {
            '.',
            'e',
            'd',
            'p',
            'l',
            'o',
            'c'
        };

        /// Complete maximum canonical file-path byte count for this ContractFamily.
        inline static constexpr std::size_t RequiredMaximumPackPathBytes_ =
            TPackDirectory.Size() +
            (TPackDirectory.Size() == 0U ? 0U : 1U) +
            TLocalisationContract::MaximumSupportedLanguageIdentifierBytes +
            PackExtension_.size();

        /// FileStorage Requirement used both for provider Contract declaration and unique selection.
        using StorageRequirement = FileStorageRequirement<
            TPackDirectory,
            TLocalisationContract,
            TStorageSelectionConstraints...
        >;

        static_assert(
            StorageRequirement::Cardinality::Minimum == 1U &&
            StorageRequirement::Cardinality::Maximum == 1U,
            "FilePackSource requires exactly one qualified FileStorage provider"
        );

        static_assert(
            TPersistenceComposition::template SatisfiesRequirement<StorageRequirement>,
            "Persistence Composition does not satisfy FilePackSource FileStorage cardinality/qualification"
        );

        /// Unique FileStorage provider selected explicitly from the supplied Persistence Composition.
        using StorageProvider =
            typename TPersistenceComposition::template Select<
                StorageRequirement,
                Framework::SelectUnique
            >;

        static_assert(
            ESPressio::Persistence::FileStorageProvider<StorageProvider>,
            "Selected Persistence provider does not satisfy the complete EDP-Persistence FileStorage contract"
        );


        /// Stable file-backed locator returned by successful Locate.
        class Resource final {
        private:

            // Owned canonical language identity.

            /// Exact canonical language byte count.
            std::uint8_t LanguageLength_;

            /// Inline canonical language bytes sized from the generated ContractFamily.
            std::array<
                char,
                TLocalisationContract::MaximumSupportedLanguageIdentifierBytes
            > LanguageBytes_;


            /// Copies one validated canonical language identity into stable inline storage.
            Resource(
                LanguageIdentifierView Language,
                const TByteOperationsProvider& ByteOperations
            ) noexcept :
                LanguageLength_(Language.Length()),
                LanguageBytes_{} {
                ByteOperations.CopyBytes(
                    LanguageBytes_.data(),
                    Language.Data(),
                    Language.Length()
                );
            }

            /// Grants the owning source resource construction and identity access.
            friend class FilePackSource<
                TPersistenceComposition,
                TPackDirectory,
                TLocalisationContract,
                TByteOperationsProvider,
                TStorageSelectionConstraints...
            >;

        public:

            /// Copies one stable file-backed resource locator.
            Resource(const Resource&) noexcept = default;

            /// Replaces one stable file-backed resource locator.
            Resource& operator=(const Resource&) noexcept = default;

        };


        // Bound Bootstrap-owned providers.

        /// Selected FileStorage provider instance.
        const StorageProvider* Storage_;

        /// Raw byte operations provider used for stable identity/path copying.
        const TByteOperationsProvider* ByteOperations_;


        // Compile-time directory validation.

        /// Indicates whether the configured provider-relative directory satisfies Persistence path rules.
        [[nodiscard]] static consteval bool IsPackDirectoryValid() noexcept {
            if constexpr (TPackDirectory.Size() == 0U) {
                return true;
            } else {
                const auto Result = ESPressio::Persistence::FilePathView::Validate(
                    TPackDirectory.Bytes.data(),
                    TPackDirectory.Size()
                );

                return Result.IsValuePresent;
            }
        }

        static_assert(
            IsPackDirectoryValid(),
            "FilePackSource pack directory must be empty (provider root) or a canonical provider-relative Persistence path"
        );


        // Canonical pack path construction.

        /// Builds one provider-relative canonical pack path into caller-owned operation scratch.
        [[nodiscard]] std::size_t BuildPackPath(
            LanguageIdentifierView Language,
            std::array<char, RequiredMaximumPackPathBytes_>& Path
        ) const noexcept {
            std::size_t Offset = 0U;

            if constexpr (TPackDirectory.Size() != 0U) {
                ByteOperations_->CopyBytes(
                    Path.data(),
                    TPackDirectory.Bytes.data(),
                    TPackDirectory.Size()
                );
                Offset += TPackDirectory.Size();
                Path[Offset] = '/';
                ++Offset;
            }

            ByteOperations_->CopyBytes(
                Path.data() + Offset,
                Language.Data(),
                Language.Length()
            );
            Offset += Language.Length();

            ByteOperations_->CopyBytes(
                Path.data() + Offset,
                PackExtension_.data(),
                PackExtension_.size()
            );
            Offset += PackExtension_.size();

            return Offset;
        }

        /// Reconstructs and validates one pack path from a stable resource.
        [[nodiscard]] ESPressio::Persistence::FilePathView::ValidationResult BuildResourcePath(
            const Resource& ResourceValue,
            std::array<char, RequiredMaximumPackPathBytes_>& Path
        ) const noexcept {
            const auto Language = LanguageIdentifierView::Validate(
                ResourceValue.LanguageBytes_.data(),
                ResourceValue.LanguageLength_
            );

            if (!Language.IsValuePresent) {
                return ESPressio::Persistence::FilePathView::Validate(
                    nullptr,
                    0U
                );
            }

            const std::size_t PathSize = BuildPackPath(
                Language.Value,
                Path
            );

            return ESPressio::Persistence::FilePathView::Validate(
                Path.data(),
                PathSize
            );
        }


        // Persistence result mapping.

        /// Maps FileSizeStatus into Locate semantics.
        [[nodiscard]] static constexpr PackLocateStatus MapLocateStatus(
            ESPressio::Persistence::FileSizeStatus Status
        ) noexcept {
            using StatusType = ESPressio::Persistence::FileSizeStatus;

            switch (Status) {
                case StatusType::Succeeded:
                    return PackLocateStatus::Success;

                case StatusType::NotFound:
                    return PackLocateStatus::PackUnavailable;

                case StatusType::NotReady:
                case StatusType::MediaUnavailable:
                case StatusType::Busy:
                    return PackLocateStatus::ProviderUnavailable;

                case StatusType::PathTooLong:
                case StatusType::PathNotRepresentable:
                case StatusType::CorruptData:
                case StatusType::IoFailure:
                case StatusType::ProviderFailure:
                    return PackLocateStatus::ReadFailure;
            }

            return PackLocateStatus::ReadFailure;
        }

        /// Maps FileSizeStatus into observation-after-Locate semantics.
        [[nodiscard]] static constexpr PackSizeStatus MapSizeStatus(
            ESPressio::Persistence::FileSizeStatus Status
        ) noexcept {
            using StatusType = ESPressio::Persistence::FileSizeStatus;

            switch (Status) {
                case StatusType::Succeeded:
                    return PackSizeStatus::Success;

                case StatusType::NotFound:
                    return PackSizeStatus::ResourceUnavailable;

                case StatusType::NotReady:
                case StatusType::MediaUnavailable:
                case StatusType::Busy:
                    return PackSizeStatus::ProviderUnavailable;

                case StatusType::PathTooLong:
                case StatusType::PathNotRepresentable:
                case StatusType::CorruptData:
                case StatusType::IoFailure:
                case StatusType::ProviderFailure:
                    return PackSizeStatus::ReadFailure;
            }

            return PackSizeStatus::ReadFailure;
        }

        /// Maps a non-successful FileReadStatus into Pack Source semantics.
        [[nodiscard]] static constexpr PackReadStatus MapReadFailureStatus(
            ESPressio::Persistence::FileReadStatus Status
        ) noexcept {
            using StatusType = ESPressio::Persistence::FileReadStatus;

            switch (Status) {
                case StatusType::Succeeded:
                    return PackReadStatus::Success;

                case StatusType::NotFound:
                    return PackReadStatus::ResourceUnavailable;

                case StatusType::NotReady:
                case StatusType::MediaUnavailable:
                case StatusType::Busy:
                    return PackReadStatus::ProviderUnavailable;

                case StatusType::InvalidOffset:
                    return PackReadStatus::OutOfRange;

                case StatusType::PathTooLong:
                case StatusType::PathNotRepresentable:
                case StatusType::CorruptData:
                case StatusType::IoFailure:
                case StatusType::ProviderFailure:
                    return PackReadStatus::ReadFailure;
            }

            return PackReadStatus::ReadFailure;
        }

    public:

        /// Provider-associated stable resource type consumed by Pack Source operations.
        using PackResource = Resource;

        /// Exposes the consolidated cross-domain Persistence Requirement for Architecture validation/testing.
        using PersistenceRequirement = StorageRequirement;

        /// Maximum canonical pack path bytes required by this FilePackSource specialization.
        inline static constexpr std::size_t RequiredMaximumPackPathBytes =
            RequiredMaximumPackPathBytes_;


        // Construction.

        /// Binds the compile-time selected FileStorage instance and Bootstrap-owned ByteOperations provider.
        FilePackSource(
            const StorageProvider& Storage,
            const TByteOperationsProvider& ByteOperations
        ) noexcept :
            Storage_(&Storage),
            ByteOperations_(&ByteOperations) {}


        // Pack Source operations.

        /// Locates one currently addressable language pack and returns a stable resource identity.
        [[nodiscard]] PackLocateResult<PackResource> Locate(
            LanguageIdentifierView Language
        ) const noexcept {
            const auto LanguageValidation = LanguageIdentifierView::Validate(
                Language.Data(),
                Language.Length()
            );

            if (!LanguageValidation.IsValuePresent) {
                return {
                    PackLocateStatus::InvalidArgument,
                    std::nullopt
                };
            }

            std::array<char, RequiredMaximumPackPathBytes_> Path{};
            const std::size_t PathSize = BuildPackPath(
                Language,
                Path
            );
            const auto ValidatedPath = ESPressio::Persistence::FilePathView::Validate(
                Path.data(),
                PathSize
            );

            if (!ValidatedPath.IsValuePresent) {
                return {
                    PackLocateStatus::ReadFailure,
                    std::nullopt
                };
            }

            const auto Result = Storage_->GetFileSize(
                ValidatedPath.Value
            );
            const auto Status = MapLocateStatus(
                Result.Status
            );

            if (Status != PackLocateStatus::Success) {
                return {
                    Status,
                    std::nullopt
                };
            }

            return {
                PackLocateStatus::Success,
                PackResource(
                    Language,
                    *ByteOperations_
                )
            };
        }

        /// Performs a fresh FileStorage size observation for one previously located resource.
        [[nodiscard]] PackSizeResult Size(
            const PackResource& ResourceValue
        ) const noexcept {
            std::array<char, RequiredMaximumPackPathBytes_> Path{};
            const auto ValidatedPath = BuildResourcePath(
                ResourceValue,
                Path
            );

            if (!ValidatedPath.IsValuePresent) {
                return {
                    PackSizeStatus::ReadFailure,
                    0U
                };
            }

            const auto Result = Storage_->GetFileSize(
                ValidatedPath.Value
            );
            const auto Status = MapSizeStatus(
                Result.Status
            );

            return {
                Status,
                Status == PackSizeStatus::Success
                    ? Result.Size.RawValue
                    : 0U
            };
        }

        /// Performs one exact bounded read over the selected FileStorage resource.
        [[nodiscard]] PackReadResult Read(
            const PackResource& ResourceValue,
            std::uint64_t Offset,
            WritableByteView Destination
        ) const noexcept {
            if (
                Destination.Size != 0U &&
                Destination.Data == nullptr
            ) {
                return {
                    PackReadStatus::ReadFailure,
                    0U
                };
            }

            std::array<char, RequiredMaximumPackPathBytes_> Path{};
            const auto ValidatedPath = BuildResourcePath(
                ResourceValue,
                Path
            );

            if (!ValidatedPath.IsValuePresent) {
                return {
                    PackReadStatus::ReadFailure,
                    0U
                };
            }

            const auto Result = Storage_->ReadFileAt(
                ValidatedPath.Value,
                ESPressio::Persistence::StorageOffset{
                    Offset
                },
                {
                    Destination.Data,
                    Destination.Size
                }
            );

            if (
                Result.Status != ESPressio::Persistence::FileReadStatus::Succeeded
            ) {
                return {
                    MapReadFailureStatus(
                        Result.Status
                    ),
                    0U
                };
            }

            if (Result.BytesTransferred != Destination.Size) {
                return {
                    PackReadStatus::OutOfRange,
                    Result.BytesTransferred
                };
            }

            return {
                PackReadStatus::Success,
                Result.BytesTransferred
            };
        }

        /// Returns the stable canonical language identity owned by ResourceValue without touching Persistence.
        [[nodiscard]] LanguageIdentifierView LanguageIdentity(
            const PackResource& ResourceValue
        ) const noexcept {
            return LanguageIdentifierView::Validate(
                ResourceValue.LanguageBytes_.data(),
                ResourceValue.LanguageLength_
            ).Value;
        }

    };

} // ESPressio::Localisation
