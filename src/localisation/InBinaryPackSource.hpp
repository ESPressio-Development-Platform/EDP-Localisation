#pragma once

#include <cstddef>
#include <cstdint>

#include <memory/ByteOperationsContract.hpp>
#include <memory/MemoryTypes.hpp>

#include "LanguageIdentifierView.hpp"
#include "LocalisationComposition.hpp"
#include "LocalisationTypes.hpp"
#include "PackSourceContract.hpp"

namespace ESPressio::Localisation {

    /// Immutable in-binary pack descriptor owned by generated/application storage.
    struct InBinaryPackDescriptor final {

        /// Canonical language identity represented by this pack.
        LanguageIdentifierView Language;

        /// First immutable pack byte.
        const std::uint8_t* Data;

        /// Number of immutable pack bytes.
        std::size_t SizeBytes;

    };


    /// Pack Source exposing immutable generated EDPL bytes compiled into the application.
    ///
    /// @tparam TByteOperationsProvider Concrete provider satisfying EDP-Memory ByteOperations.
    template<class TByteOperationsProvider>
    class InBinaryPackSource final : public Framework::Provider<
        Domain,
        Framework::Provides<
            Framework::Offer<PackSource>
        >,
        Framework::Contract<
            ExternalByteOperationsRequirement
        >
    > {
    private:

        static_assert(
            sizeof(
                ESPressio::Memory::Detail::ByteOperationsProviderTraits<
                    TByteOperationsProvider
                >
            ) > 0U,
            "InBinaryPackSource requires an EDP-Memory ByteOperations provider"
        );


        /// Stable provider-associated locator returned after successful Locate.
        class Resource final {
        private:

            // Stable descriptor identity.

            /// Immutable descriptor represented by this resource.
            const InBinaryPackDescriptor* Descriptor_;

            /// Constructs a resource for one provider-owned descriptor table entry.
            explicit constexpr Resource(
                const InBinaryPackDescriptor* Descriptor
            ) noexcept :
                Descriptor_(Descriptor) {}

            /// Grants the owning Pack Source access to resource construction.
            friend class InBinaryPackSource<TByteOperationsProvider>;

        public:

            /// Copies a stable in-binary resource locator.
            constexpr Resource(const Resource&) noexcept = default;

            /// Replaces a stable in-binary resource locator.
            constexpr Resource& operator=(const Resource&) noexcept = default;

            /// Destroys the stable descriptor locator.
            ~Resource() = default;

        };


        // Bound immutable source state.

        /// First immutable pack descriptor.
        const InBinaryPackDescriptor* Descriptors_;

        /// Number of immutable pack descriptors.
        std::size_t DescriptorCount_;

        /// Bootstrap-owned raw byte operations provider.
        const TByteOperationsProvider* ByteOperations_;


        // Descriptor comparison.

        /// Indicates whether Descriptor represents Language exactly.
        [[nodiscard]] bool IsLanguageMatch(
            const InBinaryPackDescriptor& Descriptor,
            LanguageIdentifierView Language
        ) const noexcept {
            if (Descriptor.Language.Length() != Language.Length()) { return false; }

            if (Language.Length() == 0U) { return false; }

            return ByteOperations_->CompareBytes(
                Descriptor.Language.Data(),
                Language.Data(),
                Language.Length()
            ) == ESPressio::Memory::ByteComparison::Equal;
        }

    public:

        /// Provider-associated resource type consumed by Pack Source operations.
        using PackResource = Resource;


        // Construction.

        /// Binds one immutable descriptor table and the Bootstrap-owned ByteOperations provider.
        InBinaryPackSource(
            const InBinaryPackDescriptor* Descriptors,
            std::size_t DescriptorCount,
            const TByteOperationsProvider& ByteOperations
        ) noexcept :
            Descriptors_(Descriptors),
            DescriptorCount_(DescriptorCount),
            ByteOperations_(&ByteOperations) {}


        // Pack Source operations.

        /// Locates one immutable pack by canonical language identity.
        [[nodiscard]] PackLocateResult<PackResource> Locate(
            LanguageIdentifierView Language
        ) const noexcept {
            const auto Validation = LanguageIdentifierView::Validate(
                Language.Data(),
                Language.Length()
            );

            if (!Validation.IsValuePresent) {
                return {
                    PackLocateStatus::InvalidArgument,
                    std::nullopt
                };
            }

            if (
                DescriptorCount_ != 0U &&
                Descriptors_ == nullptr
            ) {
                return {
                    PackLocateStatus::ReadFailure,
                    std::nullopt
                };
            }

            for (std::size_t Index = 0U; Index < DescriptorCount_; ++Index) {
                if (!IsLanguageMatch(
                    Descriptors_[Index],
                    Language
                )) {
                    continue;
                }

                return {
                    PackLocateStatus::Success,
                    PackResource(
                        &Descriptors_[Index]
                    )
                };
            }

            return {
                PackLocateStatus::PackUnavailable,
                std::nullopt
            };
        }

        /// Reports the complete immutable pack size.
        [[nodiscard]] PackSizeResult Size(
            const PackResource& ResourceValue
        ) const noexcept {
            return {
                PackSizeStatus::Success,
                static_cast<std::uint64_t>(
                    ResourceValue.Descriptor_->SizeBytes
                )
            };
        }

        /// Copies exactly the requested immutable pack byte range.
        [[nodiscard]] PackReadResult Read(
            const PackResource& ResourceValue,
            std::uint64_t Offset,
            WritableByteView Destination
        ) const noexcept {
            const auto& Descriptor = *ResourceValue.Descriptor_;
            const std::uint64_t SizeBytes = static_cast<std::uint64_t>(
                Descriptor.SizeBytes
            );

            if (Offset > SizeBytes) {
                return {
                    PackReadStatus::OutOfRange,
                    0U
                };
            }

            const std::uint64_t Remaining = SizeBytes - Offset;

            if (
                static_cast<std::uint64_t>(Destination.Size) > Remaining
            ) {
                return {
                    PackReadStatus::OutOfRange,
                    0U
                };
            }

            if (Destination.Size == 0U) {
                return {
                    PackReadStatus::Success,
                    0U
                };
            }

            if (
                Destination.Data == nullptr ||
                Descriptor.Data == nullptr
            ) {
                return {
                    PackReadStatus::ReadFailure,
                    0U
                };
            }

            const auto AddressableOffset = static_cast<std::size_t>(Offset);

            ByteOperations_->CopyBytes(
                Destination.Data,
                Descriptor.Data + AddressableOffset,
                Destination.Size
            );

            return {
                PackReadStatus::Success,
                Destination.Size
            };
        }

        /// Returns the canonical language identity represented by ResourceValue.
        [[nodiscard]] LanguageIdentifierView LanguageIdentity(
            const PackResource& ResourceValue
        ) const noexcept {
            return ResourceValue.Descriptor_->Language;
        }

    };

} // ESPressio::Localisation
