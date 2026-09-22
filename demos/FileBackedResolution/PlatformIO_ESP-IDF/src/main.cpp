#include <cerrno>
#include <cstddef>
#include <cstdint>
#include <cstdio>

#include <sdkconfig.h>

#include <esp_err.h>
#include <esp_vfs_fat.h>
#include <wear_levelling.h>

#include <ESPressio_Localisation_Persistence.hpp>
#include <ESPressio_Persistence_ESP_IDF.hpp>
#include <memory/ByteOperationsProvider.hpp>

#include "DemoGenerated.hpp"

#if !defined(CONFIG_FATFS_LFN_STACK) || !defined(CONFIG_FATFS_MAX_LFN)
#error "FileBackedResolution requires stack-backed FAT long-filename support; regenerate sdkconfig from sdkconfig.defaults"
#else
static_assert(
    CONFIG_FATFS_MAX_LFN >= 63,
    "FileBackedResolution requires CONFIG_FATFS_MAX_LFN >= 63 to satisfy its VFS binding profile"
);
#endif

#if !defined(CONFIG_VFS_SUPPORT_IO) || !defined(CONFIG_VFS_SUPPORT_DIR)
#error "FileBackedResolution requires ESP-IDF VFS I/O and directory-operation support"
#endif

namespace Demo {

    /// Mutually exclusive outcome of mounting the dedicated FAT filesystem.
    enum class FileSystemMountStatus : std::uint8_t {
        Succeeded = 0U,
        ProviderFailure = 1U
    };

    /// Mutually exclusive outcome of unmounting the dedicated FAT filesystem.
    enum class FileSystemUnmountStatus : std::uint8_t {
        Succeeded = 0U,
        NotMounted = 1U,
        ProviderFailure = 2U
    };

    /// Mutually exclusive outcome of the complete file-backed demonstration.
    enum class DemoStatus : std::uint8_t {
        Succeeded = 0U,
        FileSystemMountFailed = 1U,
        StorageNotReady = 2U,
        PackPreparationFailed = 3U,
        ResolutionFailed = 4U,
        LanguageMaterialisationFailed = 5U,
        ExpectedFallbackMissing = 6U,
        FileSystemUnmountFailed = 7U
    };


    namespace Framework = ESPressio::System::CompositionFramework;

    /// VFS base path used by the dedicated demonstration filesystem.
    constexpr char BasePath[] = "/edploc";

    /// Partition label selected by the committed demonstration partition table.
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
        "ESP-IDF VfsFileStorage must satisfy FileStorage"
    );

    static_assert(
        ESPressio::Localisation::PackSourceProvider<PackSource>,
        "The real ESP-IDF FileStorage binding must satisfy FilePackSource"
    );


    /// Wear-levelling handle owned by the one-shot demonstration lifecycle.
    wl_handle_t WearLevellingHandle = WL_INVALID_HANDLE;


    /// Mounts the dedicated internal-flash FAT partition with wear levelling.
    [[nodiscard]] FileSystemMountStatus MountFileSystem() noexcept {
        esp_vfs_fat_mount_config_t Configuration{};
        Configuration.format_if_mount_failed = true;
        Configuration.max_files = 4U;
        Configuration.allocation_unit_size = 4096U;

        return esp_vfs_fat_spiflash_mount_rw_wl(
            BasePath,
            PartitionLabel,
            &Configuration,
            &WearLevellingHandle
        ) == ESP_OK
            ? FileSystemMountStatus::Succeeded
            : FileSystemMountStatus::ProviderFailure;
    }


    /// Unmounts the dedicated FAT partition and reports the provider outcome.
    [[nodiscard]] FileSystemUnmountStatus UnmountFileSystem() noexcept {
        if (WearLevellingHandle == WL_INVALID_HANDLE) {
            return FileSystemUnmountStatus::NotMounted;
        }

        const esp_err_t Result = esp_vfs_fat_spiflash_unmount_rw_wl(
            BasePath,
            WearLevellingHandle
        );

        if (Result != ESP_OK) {
            return FileSystemUnmountStatus::ProviderFailure;
        }

        WearLevellingHandle = WL_INVALID_HANDLE;
        return FileSystemUnmountStatus::Succeeded;
    }


    /// Stage at which pack preparation most recently completed or failed.
    enum class PackPreparationStage : std::uint8_t {
        Succeeded = 0U,
        CreateDirectory = 1U,
        WriteEnglishPack = 2U,
        WriteGermanPack = 3U
    };


    /// Structured diagnostic result of preparing persisted EDPL fixtures.
    struct PackPreparationResult final {

        /// Stage at which preparation completed or failed.
        PackPreparationStage Stage;

        /// Directory-creation result observed before file publication.
        ESPressio::Persistence::DirectoryCreateStatus DirectoryStatus;

        /// File-replacement result for the active file stage.
        ESPressio::Persistence::FileReplaceStatus FileStatus;

        /// Native errno captured for supplemental demonstration diagnostics.
        int NativeError;


        /// Indicates whether every preparation stage completed successfully.
        [[nodiscard]] bool IsSucceeded() const noexcept {
            return Stage == PackPreparationStage::Succeeded;
        }

    };


    /// Creates the pack directory and writes both generated EDPL fixture files.
    [[nodiscard]] PackPreparationResult PreparePackFiles(
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

        errno = 0;
        const auto DirectoryStatus = Storage.CreateDirectory(
            Directory.Value
        );
        const int DirectoryNativeError = errno;

        if (
            DirectoryStatus !=
                ESPressio::Persistence::DirectoryCreateStatus::Succeeded &&
            DirectoryStatus !=
                ESPressio::Persistence::DirectoryCreateStatus::AlreadyExists
        ) {
            return {
                PackPreparationStage::CreateDirectory,
                DirectoryStatus,
                ESPressio::Persistence::FileReplaceStatus::Succeeded,
                DirectoryNativeError
            };
        }

        const auto EnglishStatus = Storage.ReplaceFile(
            EnglishPath.Value,
            {
                DemoGenerated::EnglishPack,
                sizeof(DemoGenerated::EnglishPack)
            }
        );

        if (
            EnglishStatus !=
                ESPressio::Persistence::FileReplaceStatus::Succeeded
        ) {
            return {
                PackPreparationStage::WriteEnglishPack,
                DirectoryStatus,
                EnglishStatus,
                errno
            };
        }

        const auto GermanStatus = Storage.ReplaceFile(
            GermanPath.Value,
            {
                DemoGenerated::GermanPack,
                sizeof(DemoGenerated::GermanPack)
            }
        );

        if (
            GermanStatus !=
                ESPressio::Persistence::FileReplaceStatus::Succeeded
        ) {
            return {
                PackPreparationStage::WriteGermanPack,
                DirectoryStatus,
                GermanStatus,
                errno
            };
        }

        return {
            PackPreparationStage::Succeeded,
            DirectoryStatus,
            GermanStatus,
            0
        };
    }


    /// Resolves the fixture string through the real ESP-IDF Persistence provider.
    [[nodiscard]] DemoStatus ResolveFromFileSystem(
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
            return DemoStatus::StorageNotReady;
        }

        const auto Preparation = PreparePackFiles(Storage);

        if (!Preparation.IsSucceeded()) {
            std::printf(
                "Pack preparation failed: stage=%u directory_status=%u file_status=%u native_errno=%d\n",
                static_cast<unsigned>(Preparation.Stage),
                static_cast<unsigned>(Preparation.DirectoryStatus),
                static_cast<unsigned>(Preparation.FileStatus),
                Preparation.NativeError
            );
            return DemoStatus::PackPreparationFailed;
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


    /// Mounts the FAT volume, executes resolution, and verifies filesystem teardown.
    [[nodiscard]] DemoStatus Run(
        char* Text,
        std::size_t TextCapacity,
        char* SupplyingLanguage,
        std::size_t SupplyingLanguageCapacity
    ) noexcept {
        const auto MountStatus = MountFileSystem();

        if (MountStatus != FileSystemMountStatus::Succeeded) {
            return DemoStatus::FileSystemMountFailed;
        }

        const auto Result = ResolveFromFileSystem(
            Text,
            TextCapacity,
            SupplyingLanguage,
            SupplyingLanguageCapacity
        );

        const auto UnmountStatus = UnmountFileSystem();

        if (UnmountStatus != FileSystemUnmountStatus::Succeeded) {
            return DemoStatus::FileSystemUnmountFailed;
        }

        return Result;
    }

} // Demo


/// Executes the ESP-IDF entry point for the file-backed resolution demonstration.
extern "C" void app_main() {
    char Text[32U]{};
    char SupplyingLanguage[8U]{};

    const auto Result = Demo::Run(
        Text,
        sizeof(Text),
        SupplyingLanguage,
        sizeof(SupplyingLanguage)
    );

    if (Result != Demo::DemoStatus::Succeeded) {
        std::printf(
            "FileBackedResolution failed: %u\n",
            static_cast<unsigned>(Result)
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
