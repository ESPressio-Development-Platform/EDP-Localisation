#include <Arduino.h>
#include <LittleFS.h>

#include <cstddef>

#include <ESPressio_Localisation_Persistence.hpp>
#include <ESPressio_Persistence_Arduino.hpp>
#include <ESPressio_Platform_Portable.hpp>

#include "DemoGenerated.hpp"

namespace Demo {

    namespace Framework = ESPressio::System::CompositionFramework;

    using ByteOperations =
        ESPressio::Platform::Portable::Memory::ByteOperationsProvider;

    struct DemoBinding final {};


    using FileProfile =
        ESPressio::Persistence::Arduino::FileSystemBindingProfile<
            ESPressio::Persistence::RetentionLevel::Restart,
            ESPressio::Persistence::TextCaseSensitivity::CaseSensitive,
            ESPressio::Persistence::MediaRemovability::Fixed,
            127U,
            63U,
            4096ULL,
            ESPressio::Persistence::InvocationConcurrency::ConcurrentReads
        >;

    using FileStorage =
        ESPressio::Persistence::Arduino::FileSystemStorage<
            DemoBinding,
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
        DemoGenerated::Contract,
        ByteOperations
    >;

    using Resolver = ESPressio::Localisation::Resolver<
        PackSource,
        ByteOperations,
        DemoGenerated::Contract
    >;


    static_assert(
        ESPressio::Persistence::FileStorageProvider<FileStorage>,
        "Arduino FileSystemStorage must satisfy FileStorage"
    );

    static_assert(
        ESPressio::Localisation::PackSourceProvider<PackSource>,
        "The real Arduino FileStorage binding must satisfy FilePackSource"
    );


    [[nodiscard]] bool PreparePackFiles(
        FileStorage& Storage
    ) noexcept {
        constexpr auto Directory =
            ESPressio::Persistence::FilePathView::Validate("localisation");
        constexpr auto EnglishPath =
            ESPressio::Persistence::FilePathView::Validate(
                "localisation/en-GB.edploc"
            );
        constexpr auto GermanPath =
            ESPressio::Persistence::FilePathView::Validate(
                "localisation/de.edploc"
            );

        static_assert(
            Directory.Status ==
                ESPressio::Persistence::FilePathValidationStatus::Succeeded
        );
        static_assert(
            EnglishPath.Status ==
                ESPressio::Persistence::FilePathValidationStatus::Succeeded
        );
        static_assert(
            GermanPath.Status ==
                ESPressio::Persistence::FilePathValidationStatus::Succeeded
        );

        const auto DirectoryStatus = Storage.CreateDirectory(
            Directory.Value
        );

        if (
            DirectoryStatus !=
                ESPressio::Persistence::DirectoryCreateStatus::Succeeded &&
            DirectoryStatus !=
                ESPressio::Persistence::DirectoryCreateStatus::AlreadyExists
        ) {
            return false;
        }

        if (
            Storage.ReplaceFile(
                EnglishPath.Value,
                {
                    DemoGenerated::EnglishPack,
                    sizeof(DemoGenerated::EnglishPack)
                }
            ) != ESPressio::Persistence::FileReplaceStatus::Succeeded
        ) {
            return false;
        }

        return Storage.ReplaceFile(
            GermanPath.Value,
            {
                DemoGenerated::GermanPack,
                sizeof(DemoGenerated::GermanPack)
            }
        ) == ESPressio::Persistence::FileReplaceStatus::Succeeded;
    }


    [[nodiscard]] int ResolveFromFileSystem(
        char* Text,
        std::size_t TextCapacity,
        char* SupplyingLanguage,
        std::size_t SupplyingLanguageCapacity
    ) noexcept {
        ByteOperations Bytes;
        FileStorage Storage(
            LittleFS,
            Bytes
        );

        if (!PreparePackFiles(Storage)) {
            return 11;
        }

        PackSource Source(
            Storage,
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

        const auto Result = Localisation.ResolveString(
            Context,
            Greeting,
            {
                Text,
                TextCapacity
            },
            ESPressio::Localisation::TextOutputMode::NullTerminatedUtf8
        );

        if (
            Result.Status !=
                ESPressio::Localisation::LocalisationStatus::Success ||
            !Result.ResolvedLanguage.has_value()
        ) {
            return 12;
        }

        const auto LanguageResult = Localisation.ResolveLanguageIdentity(
            *Result.ResolvedLanguage,
            {
                SupplyingLanguage,
                SupplyingLanguageCapacity
            },
            ESPressio::Localisation::TextOutputMode::NullTerminatedUtf8
        );

        if (
            LanguageResult.Status !=
                ESPressio::Localisation::TextMaterialisationStatus::Success
        ) {
            return 13;
        }

        return Result.Facts.IsSet(
            ESPressio::Localisation::LocalisationFact::LanguageFallbackUsed
        ) ? 0 : 14;
    }


    [[nodiscard]] int Run(
        char* Text,
        std::size_t TextCapacity,
        char* SupplyingLanguage,
        std::size_t SupplyingLanguageCapacity
    ) noexcept {
        if (!LittleFS.begin(true)) {
            return 10;
        }

        const int Result = ResolveFromFileSystem(
            Text,
            TextCapacity,
            SupplyingLanguage,
            SupplyingLanguageCapacity
        );

        LittleFS.end();
        return Result;
    }

} // namespace Demo


void setup() {
    Serial.begin(115200);

    char Text[32U]{};
    char SupplyingLanguage[8U]{};
    const int Result = Demo::Run(
        Text,
        sizeof(Text),
        SupplyingLanguage,
        sizeof(SupplyingLanguage)
    );

    if (Result != 0) {
        Serial.print("FileBackedResolution failed: ");
        Serial.println(Result);
        return;
    }

    Serial.print("Resolved through real FileSystemStorage + FilePackSource: ");
    Serial.println(Text);
    Serial.print("Supplying language: ");
    Serial.println(SupplyingLanguage);
}


void loop() {
}
