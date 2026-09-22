#include <Arduino.h>
#include <LittleFS.h>

#include <cstddef>

#include <ESPressio_Localisation_Persistence.hpp>
#include <ESPressio_Persistence_Arduino.hpp>
#include <memory/ByteOperationsProvider.hpp>

#include "generated/GeneratedLocalisationContract.hpp"
#include "generated/GeneratedLocalisationIdentifiers.hpp"

namespace Tutorial {

    namespace Framework = ESPressio::System::CompositionFramework;
    namespace Identifiers =
        Example::Localisation::LocalisationIdentifiers;

    using Contract =
        Example::Localisation::GeneratedLocalisationContract;

    using ByteOperations =
        ESPressio::Platform::Portable::Memory::ByteOperationsProvider;

    struct TutorialBinding final {};


    using FileProfile =
        ESPressio::Persistence::Arduino::FileSystemBindingProfile<
            ESPressio::Persistence::RetentionLevel::Restart,
            ESPressio::Persistence::TextCaseSensitivity::CaseSensitive,
            ESPressio::Persistence::MediaRemovability::Fixed,
            127U,
            63U,
            16384ULL,
            ESPressio::Persistence::InvocationConcurrency::ConcurrentReads
        >;

    using FileStorage =
        ESPressio::Persistence::Arduino::FileSystemStorage<
            TutorialBinding,
            FileProfile,
            ByteOperations
        >;

    using PersistenceComposition = Framework::Composition<
        ESPressio::Persistence::Domain,
        FileStorage
    >;

    inline constexpr ESPressio::Localisation::StaticPackDirectory PackDirectory{
        "localisation"
    };

    using PackSource = ESPressio::Localisation::FilePackSource<
        PersistenceComposition,
        PackDirectory,
        Contract,
        ByteOperations
    >;

    using Resolver = ESPressio::Localisation::Resolver<
        PackSource,
        ByteOperations,
        Contract
    >;


    inline constexpr auto AustrianGerman =
        ESPressio::Localisation::LanguageIdentifierView::Validate("de-AT");

    inline constexpr auto BritishEnglish =
        ESPressio::Localisation::LanguageIdentifierView::Validate("en-GB");

    static_assert(AustrianGerman.IsValuePresent);
    static_assert(BritishEnglish.IsValuePresent);


    void PrintResult(
        const char* Label,
        const Resolver& Localisation,
        const Resolver::ResolveResult& Result,
        const char* Text
    ) {
        Serial.print(Label);
        Serial.print(": ");

        if (
            Result.Status !=
                ESPressio::Localisation::LocalisationStatus::Success
        ) {
            Serial.print("<resolution failed: ");
            Serial.print(static_cast<unsigned>(Result.Status));
            Serial.println(">");
            return;
        }

        Serial.print(Text);

        if (!Result.ResolvedLanguage.has_value()) {
            Serial.println();
            return;
        }

        char Language[16U]{};
        const auto LanguageResult =
            Localisation.ResolveLanguageIdentity(
                *Result.ResolvedLanguage,
                {
                    Language,
                    sizeof(Language)
                },
                ESPressio::Localisation::TextOutputMode::NullTerminatedUtf8
            );

        if (
            LanguageResult.Status ==
                ESPressio::Localisation::TextMaterialisationStatus::Success
        ) {
            Serial.print("  [supplied by ");
            Serial.print(Language);
            Serial.print("]");
        }

        Serial.println();
    }


    void Run() {
        ByteOperations Bytes;
        FileStorage Storage(
            LittleFS,
            Bytes
        );
        PackSource Source(
            Storage,
            Bytes
        );
        Resolver Localisation(
            Source,
            Bytes
        );

        const ESPressio::Localisation::LocalisationContext Context{
            AustrianGerman.Value,
            BritishEnglish.Value
        };

        char Text[128U]{};

        auto Result = Localisation.ResolveString(
            Context,
            Identifiers::Application::Root::Greeting,
            {
                Text,
                sizeof(Text)
            },
            ESPressio::Localisation::TextOutputMode::NullTerminatedUtf8
        );
        PrintResult(
            "Greeting",
            Localisation,
            Result,
            Text
        );

        Result = Localisation.ResolveString(
            Context,
            Identifiers::Application::Root::Ready,
            {
                Text,
                sizeof(Text)
            },
            ESPressio::Localisation::TextOutputMode::NullTerminatedUtf8
        );
        PrintResult(
            "Ready",
            Localisation,
            Result,
            Text
        );

        Result = Localisation.ResolveTypeName(
            Context,
            Identifiers::Types::TemperatureReading::Type,
            {
                Text,
                sizeof(Text)
            },
            ESPressio::Localisation::TextOutputMode::NullTerminatedUtf8
        );
        PrintResult(
            "Type name",
            Localisation,
            Result,
            Text
        );

        Result = Localisation.ResolveTypeDescription(
            Context,
            Identifiers::Types::TemperatureReading::Type,
            {
                Text,
                sizeof(Text)
            },
            ESPressio::Localisation::TextOutputMode::NullTerminatedUtf8
        );
        PrintResult(
            "Type description",
            Localisation,
            Result,
            Text
        );

        Result = Localisation.ResolveFieldName(
            Context,
            Identifiers::Types::TemperatureReading::Fields::Temperature,
            {
                Text,
                sizeof(Text)
            },
            ESPressio::Localisation::TextOutputMode::NullTerminatedUtf8
        );
        PrintResult(
            "Temperature field",
            Localisation,
            Result,
            Text
        );

        Result = Localisation.ResolveFieldDescription(
            Context,
            Identifiers::Types::TemperatureReading::Fields::Temperature,
            {
                Text,
                sizeof(Text)
            },
            ESPressio::Localisation::TextOutputMode::NullTerminatedUtf8
        );
        PrintResult(
            "Temperature description",
            Localisation,
            Result,
            Text
        );

        Result = Localisation.ResolveFieldName(
            Context,
            Identifiers::Types::TemperatureReading::Fields::RecordedAt,
            {
                Text,
                sizeof(Text)
            },
            ESPressio::Localisation::TextOutputMode::NullTerminatedUtf8
        );
        PrintResult(
            "RecordedAt field",
            Localisation,
            Result,
            Text
        );
    }

} // namespace Tutorial


void setup() {
    Serial.begin(115200);

    if (!LittleFS.begin(true)) {
        Serial.println("LittleFS mount failed");
        return;
    }

    Tutorial::Run();
    LittleFS.end();
}


void loop() {
}
