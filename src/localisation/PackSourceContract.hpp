#pragma once

#include <concepts>
#include <cstddef>
#include <cstdint>
#include <optional>
#include <type_traits>
#include <utility>

#include "LanguageIdentifierView.hpp"
#include "LocalisationComposition.hpp"
#include "LocalisationTypes.hpp"

namespace ESPressio::Localisation {

    enum class PackLocateStatus : std::uint8_t {
        Success = 0U,
        PackUnavailable = 1U,
        ProviderUnavailable = 2U,
        ReadFailure = 3U,
        InvalidArgument = 4U
    };

    enum class PackSizeStatus : std::uint8_t {
        Success = 0U,
        ResourceUnavailable = 1U,
        ProviderUnavailable = 2U,
        ReadFailure = 3U
    };

    enum class PackReadStatus : std::uint8_t {
        Success = 0U,
        ResourceUnavailable = 1U,
        ProviderUnavailable = 2U,
        OutOfRange = 3U,
        ReadFailure = 4U
    };


    /// Locate result containing a resource only on success.
    ///
    /// @tparam TPackResource Provider-associated stable resource type.
    template<class TPackResource>
    struct PackLocateResult final {

        /// Locate outcome.
        PackLocateStatus Status;

        /// Located resource when Status is Success.
        std::optional<TPackResource> Resource;

    };


    /// Current size of one previously located pack resource.
    struct PackSizeResult final {

        /// Size-query outcome.
        PackSizeStatus Status;

        /// Current complete resource size on Success; zero otherwise.
        std::uint64_t SizeBytes;

    };


    /// Exact-read result for one bounded pack range.
    struct PackReadResult final {

        /// Read outcome.
        PackReadStatus Status;

        /// Number of destination bytes written before return.
        std::size_t BytesRead;

    };


    /// Validates the public operation shape required from a Localisation Pack Source.
    ///
    /// @tparam TPackSource Candidate provider type.
    template<class TPackSource>
    concept PackSourceProvider =
        requires(
            const TPackSource& Source,
            LanguageIdentifierView Language,
            const typename TPackSource::PackResource& Resource,
            std::uint64_t Offset,
            WritableByteView Destination
        ) {
            typename TPackSource::PackResource;

            requires TPackSource::CompositionCapabilities::template Contains<PackSource>;

            {
                Source.Locate(Language)
            } -> std::same_as<
                PackLocateResult<typename TPackSource::PackResource>
            >;

            {
                Source.Size(Resource)
            } -> std::same_as<PackSizeResult>;

            {
                Source.Read(
                    Resource,
                    Offset,
                    Destination
                )
            } -> std::same_as<PackReadResult>;

            {
                Source.LanguageIdentity(Resource)
            } -> std::same_as<LanguageIdentifierView>;
        };


    /// Emits focused compile-time diagnostics for one candidate Pack Source provider.
    ///
    /// @tparam TPackSource Candidate provider type.
    template<class TPackSource>
    constexpr void ValidatePackSourceProvider() noexcept {
        static_assert(
            PackSourceProvider<TPackSource>,
            "Localisation Pack Source provider does not satisfy the required public contract"
        );

        static_assert(
            std::is_copy_constructible_v<typename TPackSource::PackResource>,
            "PackResource must be copy constructible"
        );

        static_assert(
            std::is_copy_assignable_v<typename TPackSource::PackResource>,
            "PackResource must be copy assignable"
        );
    }

} // ESPressio::Localisation
