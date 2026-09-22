#include <cstdio>
#include <cstddef>

#include <ESPressio_Localisation.hpp>
#include <memory/ByteOperationsProvider.hpp>

#include "DemoGenerated.hpp"

namespace Demo {

    /// Mutually exclusive outcome of the in-binary resolution demonstration.
    enum class DemoStatus : std::uint8_t {
        Succeeded = 0U,
        ResolutionFailed = 1U,
        LanguageMaterialisationFailed = 2U
    };


    using ByteOperations =
        ESPressio::Platform::Portable::Memory::ByteOperationsProvider;

    using PackSource =
        ESPressio::Localisation::InBinaryPackSource<ByteOperations>;

    using Resolver = ESPressio::Localisation::Resolver<
        PackSource,
        ByteOperations,
        DemoGenerated::Contract
    >;


    /// Executes the complete in-binary resolution demonstration.
    [[nodiscard]] DemoStatus Run() noexcept {
        ByteOperations Bytes;
        PackSource Source(
            DemoGenerated::Descriptors,
            sizeof(DemoGenerated::Descriptors) /
                sizeof(DemoGenerated::Descriptors[0]),
            Bytes
        );
        Resolver Localisation(
            Source,
            Bytes
        );

        const ESPressio::Localisation::LocalisationContext Context{
            DemoGenerated::GermanValidation.Value,
            DemoGenerated::EnglishValidation.Value
        };

        const Resolver::GeneralStringIdentifier Greeting{
            Resolver::Identifiers::DomainIdentifier{1U},
            Resolver::Identifiers::SubDomainIdentifier{0U},
            Resolver::Identifiers::StringIdentifierValue{3U}
        };

        char Text[32U]{};
        const auto Result = Localisation.ResolveString(
            Context,
            Greeting,
            {
                Text,
                sizeof(Text)
            },
            ESPressio::Localisation::TextOutputMode::NullTerminatedUtf8
        );

        if (
            Result.Status !=
                ESPressio::Localisation::LocalisationStatus::Success ||
            !Result.ResolvedLanguage.has_value()
        ) {
            return DemoStatus::ResolutionFailed;
        }

        char SupplyingLanguage[8U]{};
        const auto LanguageResult = Localisation.ResolveLanguageIdentity(
            *Result.ResolvedLanguage,
            {
                SupplyingLanguage,
                sizeof(SupplyingLanguage)
            },
            ESPressio::Localisation::TextOutputMode::NullTerminatedUtf8
        );

        if (
            LanguageResult.Status !=
                ESPressio::Localisation::TextMaterialisationStatus::Success
        ) {
            return DemoStatus::LanguageMaterialisationFailed;
        }

        std::printf(
            "Resolved text: %s\n",
            Text
        );
        std::printf(
            "Supplying language: %s\n",
            SupplyingLanguage
        );
        std::printf(
            "Fallback used: %s\n",
            Result.Facts.IsSet(
                ESPressio::Localisation::LocalisationFact::LanguageFallbackUsed
            ) ? "yes" : "no"
        );

        return DemoStatus::Succeeded;
    }

} // Demo


/// Executes the ESP-IDF entry point for the in-binary resolution demonstration.
extern "C" void app_main() {
    const auto Result = Demo::Run();

    if (Result != Demo::DemoStatus::Succeeded) {
        std::printf(
            "InBinaryResolution failed: %u\n",
            static_cast<unsigned>(Result)
        );
    }
}
