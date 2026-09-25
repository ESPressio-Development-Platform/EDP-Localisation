#include <Arduino.h>
#include <LittleFS.h>

#include <cstddef>
#include <cstdint>

#include <ESPressio_Localisation_Persistence.hpp>
#include <ESPressio_Persistence_Arduino.hpp>
#include <memory/ByteOperationsProvider.hpp>

#include "generated/GeneratedLocalisationContract.hpp"
#include "generated/GeneratedLocalisationIdentifiers.hpp"

namespace Tutorial {

    /// Mutually exclusive outcome of one tutorial runtime/display operation.
    enum class TutorialStatus : std::uint8_t {
        Succeeded = 0U,
        ResolutionFailed = 1U,
        SupplyingLanguageUnavailable = 2U,
        LanguageMaterialisationFailed = 3U
    };


    namespace Framework = ESPressio::System::CompositionFramework;
    namespace Identifiers =
        Example::Localisation::LocalisationIdentifiers;

    using Contract =
        Example::Localisation::GeneratedLocalisationContract;


    /// Demonstration application Type whose concrete Fields are mapped into the Localisation schema.
    struct TemperatureReading final {

        /// Stable universal identity shared with the Localisation schema inventory.
        static constexpr ESPressio::System::TypeIdentifier Identifier{
            ESPressio::System::TypeIdentifier::Storage{
                0x01U,
                0x23U,
                0x45U,
                0x67U,
                0x89U,
                0xABU,
                0xCDU,
                0xEFU
            }
        };


        // Application data Fields.

        /// Current temperature value represented by Field zero.
        float Temperature = 0.0F;

        /// Capture time represented by Field one.
        std::uint64_t RecordedAt = 0U;


        // Canonical Field bindings.

        /// Compile-time identity binding for Temperature.
        using TemperatureField = ESPressio::System::FieldBinding<
            &TemperatureReading::Temperature,
            0U
        >;

        /// Compile-time identity binding for RecordedAt.
        using RecordedAtField = ESPressio::System::FieldBinding<
            &TemperatureReading::RecordedAt,
            1U
        >;

        /// Complete enumerable schema for this Type.
        using Fields = ESPressio::System::FieldSet<
            TemperatureField,
            RecordedAtField
        >;

    };


    static_assert(
        ESPressio::System::SchemaType<TemperatureReading>,
        "TemperatureReading must satisfy the platform schema contract"
    );

    static_assert(
        ESPressio::System::TypeIdentifierOf<TemperatureReading> ==
            Identifiers::Types::TemperatureReading::Type,
        "C++ Type identity must match the Localisation schema inventory"
    );

    static_assert(
        ESPressio::System::FieldIdentifierOf<
            TemperatureReading::TemperatureField
        > ==
            Identifiers::Types::TemperatureReading::Fields::Temperature.Field,
        "Temperature Field identity must match the Localisation schema inventory"
    );

    static_assert(
        ESPressio::System::FieldIdentifierOf<
            TemperatureReading::RecordedAtField
        > ==
            Identifiers::Types::TemperatureReading::Fields::RecordedAt.Field,
        "RecordedAt Field identity must match the Localisation schema inventory"
    );


    using ByteOperations =
        ESPressio::Platform::Portable::Memory::ByteOperationsProvider;

    /// Compile-time identity distinguishing this tutorial filesystem binding.
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

    /// Provider-relative directory containing generated tutorial language packs.
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
        PersistenceComposition,
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
        "Application Architecture must satisfy Memory, Persistence and Localisation requirements"
    );

    static_assert(
        ResolverContractValidation::IsValid,
        "Application Architecture must satisfy Resolver's standalone consumer Contract"
    );


    /// Canonical requested language used to exercise local German fallback.
    inline constexpr auto AustrianGerman =
        ESPressio::Localisation::LanguageIdentifierView::Validate("de-AT");

    /// Canonical terminal language used by the tutorial ContractFamily.
    inline constexpr auto BritishEnglish =
        ESPressio::Localisation::LanguageIdentifierView::Validate("en-GB");

    static_assert(AustrianGerman.IsValuePresent);
    static_assert(BritishEnglish.IsValuePresent);


    /// Prints one successful resolution and its actual supplying language.
    [[nodiscard]] TutorialStatus PrintResult(
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
            Serial.print(
                static_cast<unsigned>(Result.Status)
            );
            Serial.println(">");
            return TutorialStatus::ResolutionFailed;
        }

        Serial.print(Text);

        if (!Result.ResolvedLanguage.has_value()) {
            Serial.println();
            return TutorialStatus::SupplyingLanguageUnavailable;
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
            LanguageResult.Status !=
                ESPressio::Localisation::TextMaterialisationStatus::Success
        ) {
            Serial.println();
            return TutorialStatus::LanguageMaterialisationFailed;
        }

        Serial.print("  [supplied by ");
        Serial.print(Language);
        Serial.print("]");
        Serial.println();

        return TutorialStatus::Succeeded;
    }


    /// Executes every documented String/Type/Field lookup in the tutorial sequence.
    [[nodiscard]] TutorialStatus Run() {
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
        auto OutputStatus = PrintResult(
            "Greeting",
            Localisation,
            Result,
            Text
        );

        if (OutputStatus != TutorialStatus::Succeeded) {
            return OutputStatus;
        }

        Result = Localisation.ResolveString(
            Context,
            Identifiers::Application::Root::Ready,
            {
                Text,
                sizeof(Text)
            },
            ESPressio::Localisation::TextOutputMode::NullTerminatedUtf8
        );
        OutputStatus = PrintResult(
            "Ready",
            Localisation,
            Result,
            Text
        );

        if (OutputStatus != TutorialStatus::Succeeded) {
            return OutputStatus;
        }

        Result = Localisation.ResolveTypeName(
            Context,
            Identifiers::Types::TemperatureReading::Type,
            {
                Text,
                sizeof(Text)
            },
            ESPressio::Localisation::TextOutputMode::NullTerminatedUtf8
        );
        OutputStatus = PrintResult(
            "Type name",
            Localisation,
            Result,
            Text
        );

        if (OutputStatus != TutorialStatus::Succeeded) {
            return OutputStatus;
        }

        Result = Localisation.ResolveTypeDescription(
            Context,
            Identifiers::Types::TemperatureReading::Type,
            {
                Text,
                sizeof(Text)
            },
            ESPressio::Localisation::TextOutputMode::NullTerminatedUtf8
        );
        OutputStatus = PrintResult(
            "Type description",
            Localisation,
            Result,
            Text
        );

        if (OutputStatus != TutorialStatus::Succeeded) {
            return OutputStatus;
        }

        TutorialStatus FieldStatus = TutorialStatus::Succeeded;

        ESPressio::System::ForEachField<TemperatureReading>(
            [&]<class TField>() {
                if (FieldStatus != TutorialStatus::Succeeded) return;

                Serial.print("Field ");
                Serial.println(
                    static_cast<unsigned>(
                        TField::Identifier.Value()
                    )
                );

                const Resolver::Identifiers::FieldPresentationIdentifier FieldIdentifier{
                    ESPressio::System::TypeIdentifierOf<TemperatureReading>,
                    TField::Identifier
                };

                Result = Localisation.ResolveFieldName(
                    Context,
                    FieldIdentifier,
                    {
                        Text,
                        sizeof(Text)
                    },
                    ESPressio::Localisation::TextOutputMode::NullTerminatedUtf8
                );
                FieldStatus = PrintResult(
                    "  Name",
                    Localisation,
                    Result,
                    Text
                );

                if (FieldStatus != TutorialStatus::Succeeded) return;

                Result = Localisation.ResolveFieldDescription(
                    Context,
                    FieldIdentifier,
                    {
                        Text,
                        sizeof(Text)
                    },
                    ESPressio::Localisation::TextOutputMode::NullTerminatedUtf8
                );
                FieldStatus = PrintResult(
                    "  Description",
                    Localisation,
                    Result,
                    Text
                );
            }
        );

        if (FieldStatus != TutorialStatus::Succeeded) {
            return FieldStatus;
        }

        return TutorialStatus::Succeeded;
    }

} // Tutorial


/// Mounts LittleFS and executes the complete consumer tutorial once.
void setup() {
    Serial.begin(115200);

    if (!LittleFS.begin(true)) {
        Serial.println("LittleFS mount failed");
        return;
    }

    const auto Status = Tutorial::Run();
    LittleFS.end();

    if (Status != Tutorial::TutorialStatus::Succeeded) {
        Serial.print("Getting-started tutorial failed: ");
        Serial.println(
            static_cast<unsigned>(Status)
        );
    }
}


/// Provides the intentionally idle Arduino loop for this one-shot tutorial.
void loop() {
}
