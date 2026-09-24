#include <Arduino.h>

#include <cstddef>

#include <ESPressio_Localisation.hpp>
#include <ESPressio_Platform_Portable_ByteOperations.hpp>

#include "DemoGenerated.hpp"

namespace Demo {

    namespace Framework = ESPressio::System::CompositionFramework;


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

    using MemoryComposition = Framework::Composition<
        ESPressio::Memory::Domain,
        ByteOperations
    >;

    using LocalisationComposition = Framework::Composition<
        ESPressio::Localisation::Domain,
        PackSource
    >;

    using ApplicationArchitecture = Framework::Architecture<
        MemoryComposition,
        LocalisationComposition
    >;


    using ResolverContractValidation =
        ApplicationArchitecture::ValidateContract<
            Resolver::CompositionContract
        >;

    using SelectedPackSource = ApplicationArchitecture::Select<
        ESPressio::Localisation::PackSourceRequirement,
        Framework::SelectUnique
    >;


    static_assert(
        ApplicationArchitecture::IsValid,
        "Demo Architecture must satisfy consolidated Localisation dependencies"
    );

    static_assert(
        ResolverContractValidation::IsValid,
        "Application Architecture must satisfy Resolver's standalone consumer Contract"
    );


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

        Serial.print("Resolved text: ");
        Serial.println(Text);
        Serial.print("Supplying language: ");
        Serial.println(SupplyingLanguage);
        Serial.print("Fallback used: ");
        Serial.println(
            Result.Facts.IsSet(
                ESPressio::Localisation::LocalisationFact::LanguageFallbackUsed
            ) ? "yes" : "no"
        );

        return DemoStatus::Succeeded;
    }

} // Demo


/// Executes the Arduino startup path for the in-binary resolution demonstration.
void setup() {
    Serial.begin(115200);

    const auto Result = Demo::Run();

    if (Result != Demo::DemoStatus::Succeeded) {
        Serial.print("InBinaryResolution failed: ");
        Serial.println(
            static_cast<unsigned>(Result)
        );
    }
}


/// Provides the intentionally idle Arduino loop for this one-shot demonstration.
void loop() {
}
