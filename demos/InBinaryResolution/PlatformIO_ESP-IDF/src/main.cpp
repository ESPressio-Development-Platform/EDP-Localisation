#include <cstdio>
#include <cstddef>

#include <ESPressio_Localisation.hpp>
#include <memory/ByteOperationsProvider.hpp>

#include "DemoGenerated.hpp"

namespace Demo {

    using ByteOperations =
        ESPressio::Platform::Portable::Memory::ByteOperationsProvider;

    using PackSource =
        ESPressio::Localisation::InBinaryPackSource<ByteOperations>;

    using Resolver = ESPressio::Localisation::Resolver<
        PackSource,
        ByteOperations,
        DemoGenerated::Contract
    >;


    [[nodiscard]] int Run() noexcept {
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
            return 1;
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
            return 2;
        }

        std::printf("Resolved text: %s\n", Text);
        std::printf("Supplying language: %s\n", SupplyingLanguage);
        std::printf(
            "Fallback used: %s\n",
            Result.Facts.IsSet(
                ESPressio::Localisation::LocalisationFact::LanguageFallbackUsed
            ) ? "yes" : "no"
        );

        return 0;
    }

} // Demo


extern "C" void app_main() {
    const int Result = Demo::Run();

    if (Result != 0) {
        std::printf("InBinaryResolution failed: %d\n", Result);
    }
}
