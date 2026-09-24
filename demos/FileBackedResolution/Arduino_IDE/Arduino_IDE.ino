#include <Arduino.h>
#include <LittleFS.h>

#include <cstddef>
#include <cstdint>

#include <ESPressio_Localisation_Persistence.hpp>
#include <ESPressio_Persistence_Arduino.hpp>
#include <ESPressio_Platform_Portable.hpp>

#include "DemoGenerated.hpp"

namespace Demo {

    /// Mutually exclusive outcome of the file-backed resolution demonstration.
    enum class DemoStatus : std::uint8_t {
        Succeeded = 0U,
        FileSystemMountFailed = 1U,
        PackDirectoryPreparationFailed = 2U,
        EnglishPackWriteFailed = 3U,
        GermanPackWriteFailed = 4U,
        ResolutionFailed = 5U,
        LanguageMaterialisationFailed = 6U,
        ExpectedFallbackMissing = 7U
    };


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


    static_assert(
        ESPressio::Persistence::FileStorageProvider<FileStorage>,
        "Arduino FileSystemStorage must satisfy FileStorage"
    );

    static_assert(
        ESPressio::Localisation::PackSourceProvider<PackSource>,
        "The real Arduino FileStorage binding must satisfy FilePackSource"
    );


    /// Creates the pack directory and writes both generated EDPL fixture files.
    [[nodiscard]] DemoStatus PreparePackFiles(
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
            return DemoStatus::PackDirectoryPreparationFailed;
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
            return DemoStatus::EnglishPackWriteFailed;
        }

        if (
            Storage.ReplaceFile(
                GermanPath.Value,
                {
                    DemoGenerated::GermanPack,
                    sizeof(DemoGenerated::GermanPack)
                }
            ) != ESPressio::Persistence::FileReplaceStatus::Succeeded
        ) {
            return DemoStatus::GermanPackWriteFailed;
        }

        return DemoStatus::Succeeded;
    }


    /// Resolves the fixture string through the real Arduino Persistence provider.
    [[nodiscard]] DemoStatus ResolveFromFileSystem(
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

        const auto PreparationStatus = PreparePackFiles(
            Storage
        );

        if (PreparationStatus != DemoStatus::Succeeded) {
            return PreparationStatus;
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
            return DemoStatus::ResolutionFailed;
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
            return DemoStatus::LanguageMaterialisationFailed;
        }

        return Result.Facts.IsSet(
            ESPressio::Localisation::LocalisationFact::LanguageFallbackUsed
        )
            ? DemoStatus::Succeeded
            : DemoStatus::ExpectedFallbackMissing;
    }


    /// Mounts LittleFS, executes the file-backed resolution path, then unmounts it.
    [[nodiscard]] DemoStatus Run(
        char* Text,
        std::size_t TextCapacity,
        char* SupplyingLanguage,
        std::size_t SupplyingLanguageCapacity
    ) noexcept {
        if (!LittleFS.begin(true)) {
            return DemoStatus::FileSystemMountFailed;
        }

        const auto Result = ResolveFromFileSystem(
            Text,
            TextCapacity,
            SupplyingLanguage,
            SupplyingLanguageCapacity
        );

        LittleFS.end();
        return Result;
    }

} // Demo


/// Executes the Arduino startup path for the file-backed resolution demonstration.
void setup() {
    Serial.begin(115200);

    char Text[32U]{};
    char SupplyingLanguage[8U]{};
    const auto Result = Demo::Run(
        Text,
        sizeof(Text),
        SupplyingLanguage,
        sizeof(SupplyingLanguage)
    );

    if (Result != Demo::DemoStatus::Succeeded) {
        Serial.print("FileBackedResolution failed: ");
        Serial.println(
            static_cast<unsigned>(Result)
        );
        return;
    }

    Serial.print("Resolved through real FileSystemStorage + FilePackSource: ");
    Serial.println(Text);
    Serial.print("Supplying language: ");
    Serial.println(SupplyingLanguage);
}


/// Provides the intentionally idle Arduino loop for this one-shot demonstration.
void loop() {
}
