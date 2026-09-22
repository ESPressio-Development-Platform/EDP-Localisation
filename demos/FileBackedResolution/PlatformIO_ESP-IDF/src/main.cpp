#include <cstddef>
#include <cstdio>

#include <esp_err.h>
#include <esp_vfs_fat.h>
#include <wear_levelling.h>

#include <ESPressio_Localisation_Persistence.hpp>
#include <ESPressio_Persistence_ESP_IDF.hpp>
#include <memory/ByteOperationsProvider.hpp>

#include "DemoGenerated.hpp"

namespace Demo {

    namespace Framework = ESPressio::System::CompositionFramework;

    constexpr char BasePath[] = "/edploc";
    constexpr char PartitionLabel[] = "storage";

    using ByteOperations =
        ESPressio::Platform::Portable::Memory::ByteOperationsProvider;

    struct DemoBinding final {};


    using FileProfile =
        ESPressio::Persistence::EspIdf::VfsBindingProfile<
            ESPressio::Persistence::RetentionLevel::Restart,
            ESPressio::Persistence::TextCaseSensitivity::CaseInsensitive,
            ESPressio::Persistence::MediaRemovability::Fixed,
            127U,
            63U,
            4096ULL,
            ESPressio::Persistence::InvocationConcurrency::ConcurrentReads
        >;

    using FileStorage =
        ESPressio::Persistence::EspIdf::VfsFileStorage<
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
        "ESP-IDF VfsFileStorage must satisfy FileStorage"
    );

    static_assert(
        ESPressio::Localisation::PackSourceProvider<PackSource>,
        "The real ESP-IDF FileStorage binding must satisfy FilePackSource"
    );


    wl_handle_t WearLevellingHandle = WL_INVALID_HANDLE;


    [[nodiscard]] bool MountFileSystem() noexcept {
        esp_vfs_fat_mount_config_t Configuration{};
        Configuration.format_if_mount_failed = true;
        Configuration.max_files = 4U;
        Configuration.allocation_unit_size = 4096U;

        return esp_vfs_fat_spiflash_mount_rw_wl(
            BasePath,
            PartitionLabel,
            &Configuration,
            &WearLevellingHandle
        ) == ESP_OK;
    }


    void UnmountFileSystem() noexcept {
        if (WearLevellingHandle == WL_INVALID_HANDLE) {
            return;
        }

        static_cast<void>(
            esp_vfs_fat_spiflash_unmount_rw_wl(
                BasePath,
                WearLevellingHandle
            )
        );

        WearLevellingHandle = WL_INVALID_HANDLE;
    }


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
            BasePath,
            Bytes
        );

        if (!Storage.IsFileStorageReady()) {
            return 11;
        }

        if (!PreparePackFiles(Storage)) {
            return 12;
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
            return 13;
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
            return 14;
        }

        return Result.Facts.IsSet(
            ESPressio::Localisation::LocalisationFact::LanguageFallbackUsed
        ) ? 0 : 15;
    }


    [[nodiscard]] int Run(
        char* Text,
        std::size_t TextCapacity,
        char* SupplyingLanguage,
        std::size_t SupplyingLanguageCapacity
    ) noexcept {
        if (!MountFileSystem()) {
            return 10;
        }

        const int Result = ResolveFromFileSystem(
            Text,
            TextCapacity,
            SupplyingLanguage,
            SupplyingLanguageCapacity
        );

        UnmountFileSystem();
        return Result;
    }

} // namespace Demo


extern "C" void app_main() {
    char Text[32U]{};
    char SupplyingLanguage[8U]{};

    const int Result = Demo::Run(
        Text,
        sizeof(Text),
        SupplyingLanguage,
        sizeof(SupplyingLanguage)
    );

    if (Result != 0) {
        std::printf(
            "FileBackedResolution failed: %d\n",
            Result
        );
        return;
    }

    std::printf(
        "Resolved through real VfsFileStorage + FilePackSource: %s\n",
        Text
    );
    std::printf(
        "Supplying language: %s\n",
        SupplyingLanguage
    );
}
