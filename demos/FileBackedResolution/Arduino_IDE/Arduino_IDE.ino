#include <Arduino.h>

#include <cstddef>
#include <cstdint>

#include <ESPressio_Localisation_Persistence.hpp>
#include <ESPressio_Platform_Portable.hpp>

#include "DemoGenerated.hpp"

namespace Demo {

    namespace Framework = ESPressio::System::CompositionFramework;

    using ByteOperations =
        ESPressio::Platform::Portable::Memory::ByteOperationsProvider;


    using FileOffer = Framework::Offer<
        ESPressio::Persistence::FileStorage,
        Framework::PropertyValue<
            ESPressio::Persistence::FileAccessMode,
            ESPressio::Persistence::AccessMode::ReadOnly
        >,
        Framework::PropertyValue<
            ESPressio::Persistence::FileRetention,
            ESPressio::Persistence::RetentionLevel::Session
        >,
        Framework::PropertyValue<
            ESPressio::Persistence::FileHierarchyMode,
            ESPressio::Persistence::FileHierarchy::Hierarchical
        >,
        Framework::PropertyValue<
            ESPressio::Persistence::FilePathCaseSensitivity,
            ESPressio::Persistence::TextCaseSensitivity::CaseSensitive
        >,
        Framework::PropertyValue<
            ESPressio::Persistence::FileMediaRemovability,
            ESPressio::Persistence::MediaRemovability::Fixed
        >,
        Framework::PropertyValue<
            ESPressio::Persistence::MaximumPathBytes,
            std::size_t{128U}
        >,
        Framework::PropertyValue<
            ESPressio::Persistence::MaximumPathSegmentBytes,
            std::size_t{64U}
        >,
        Framework::PropertyValue<
            ESPressio::Persistence::MaximumFileSize,
            ESPressio::Persistence::StorageSize{4096U}
        >,
        Framework::PropertyValue<
            ESPressio::Persistence::DirectoryMutationSupport,
            ESPressio::Persistence::Support::Unsupported
        >,
        Framework::PropertyValue<
            ESPressio::Persistence::DirectoryEnumerationSupport,
            ESPressio::Persistence::Support::Unsupported
        >,
        Framework::PropertyValue<
            ESPressio::Persistence::RenameSupport,
            ESPressio::Persistence::Support::Unsupported
        >,
        Framework::PropertyValue<
            ESPressio::Persistence::AppendSupport,
            ESPressio::Persistence::Support::Unsupported
        >,
        Framework::PropertyValue<
            ESPressio::Persistence::WriteFileAtSupport,
            ESPressio::Persistence::Support::Unsupported
        >,
        Framework::PropertyValue<
            ESPressio::Persistence::FileCapacityReportingSupport,
            ESPressio::Persistence::Support::Unsupported
        >,
        Framework::PropertyValue<
            ESPressio::Persistence::FileInvocationConcurrency,
            ESPressio::Persistence::InvocationConcurrency::ConcurrentReads
        >
    >;


    class DemoFileStorage final : public Framework::Provider<
        ESPressio::Persistence::Domain,
        Framework::Provides<FileOffer>,
        Framework::Requires<>,
        Framework::DependsOn<
            Framework::Need<ESPressio::Memory::ByteOperations>
        >
    > {
    private:

        struct LocatedFile final {

            const std::uint8_t* Data;
            std::size_t Size;
            bool IsPresent;

        };


        const ByteOperations* ByteOperations_;


        [[nodiscard]] bool IsPath(
            ESPressio::Persistence::FilePathView Path,
            const char* Expected,
            std::size_t ExpectedSize
        ) const noexcept {
            return
                Path.Size() == ExpectedSize &&
                ByteOperations_->CompareBytes(
                    Path.Data(),
                    Expected,
                    ExpectedSize
                ) == ESPressio::Memory::ByteComparison::Equal;
        }

        [[nodiscard]] LocatedFile LocateFile(
            ESPressio::Persistence::FilePathView Path
        ) const noexcept {
            constexpr char EnglishPath[] =
                "localisation/en-GB.edploc";
            constexpr char GermanPath[] =
                "localisation/de.edploc";

            if (IsPath(
                Path,
                EnglishPath,
                sizeof(EnglishPath) - 1U
            )) {
                return {
                    DemoGenerated::EnglishPack,
                    sizeof(DemoGenerated::EnglishPack),
                    true
                };
            }

            if (IsPath(
                Path,
                GermanPath,
                sizeof(GermanPath) - 1U
            )) {
                return {
                    DemoGenerated::GermanPack,
                    sizeof(DemoGenerated::GermanPack),
                    true
                };
            }

            return {
                nullptr,
                0U,
                false
            };
        }

    public:

        explicit DemoFileStorage(
            const ByteOperations& ByteOperationsValue
        ) noexcept :
            ByteOperations_(&ByteOperationsValue) {}


        [[nodiscard]] bool IsFileStorageReady() const noexcept {
            return true;
        }

        [[nodiscard]] ESPressio::Persistence::FileSizeResult GetFileSize(
            ESPressio::Persistence::FilePathView Path
        ) const noexcept {
            const auto File = LocateFile(Path);

            if (!File.IsPresent) {
                return {
                    ESPressio::Persistence::FileSizeStatus::NotFound,
                    ESPressio::Persistence::StorageSize{}
                };
            }

            return {
                ESPressio::Persistence::FileSizeStatus::Succeeded,
                ESPressio::Persistence::StorageSize{
                    File.Size
                }
            };
        }

        [[nodiscard]] ESPressio::Persistence::FileReadResult ReadFileAt(
            ESPressio::Persistence::FilePathView Path,
            ESPressio::Persistence::StorageOffset Offset,
            ESPressio::Persistence::DestinationBufferView Destination
        ) const noexcept {
            const auto File = LocateFile(Path);

            if (!File.IsPresent) {
                return {
                    ESPressio::Persistence::FileReadStatus::NotFound,
                    static_cast<std::uint8_t>(
                        ESPressio::Persistence::ReadFact::None
                    ),
                    0U,
                    ESPressio::Persistence::StorageSize{}
                };
            }

            if (Offset.RawValue > File.Size) {
                return {
                    ESPressio::Persistence::FileReadStatus::InvalidOffset,
                    static_cast<std::uint8_t>(
                        ESPressio::Persistence::ReadFact::None
                    ),
                    0U,
                    ESPressio::Persistence::StorageSize{}
                };
            }

            const std::size_t Available =
                File.Size -
                static_cast<std::size_t>(Offset.RawValue);
            const std::size_t Transfer =
                Destination.Capacity < Available
                    ? Destination.Capacity
                    : Available;

            if (
                Transfer != 0U &&
                Destination.Address == nullptr
            ) {
                return {
                    ESPressio::Persistence::FileReadStatus::ProviderFailure,
                    static_cast<std::uint8_t>(
                        ESPressio::Persistence::ReadFact::None
                    ),
                    0U,
                    ESPressio::Persistence::StorageSize{}
                };
            }

            if (Transfer != 0U) {
                ByteOperations_->CopyBytes(
                    Destination.Address,
                    File.Data + static_cast<std::size_t>(Offset.RawValue),
                    Transfer
                );
            }

            std::uint8_t Facts = static_cast<std::uint8_t>(
                ESPressio::Persistence::ReadFact::AvailableDataSizeIsKnown
            );

            if (Destination.Capacity < Available) {
                Facts |= static_cast<std::uint8_t>(
                    ESPressio::Persistence::ReadFact::WasTruncated
                );
            } else if (Destination.Capacity > Available) {
                Facts |= static_cast<std::uint8_t>(
                    ESPressio::Persistence::ReadFact::IsSmallerThanAvailableBuffer
                );
            }

            return {
                ESPressio::Persistence::FileReadStatus::Succeeded,
                Facts,
                Transfer,
                ESPressio::Persistence::StorageSize{
                    Available
                }
            };
        }

        [[nodiscard]] ESPressio::Persistence::FileReplaceStatus ReplaceFile(
            ESPressio::Persistence::FilePathView,
            ESPressio::Persistence::SourceBufferView
        ) noexcept {
            return ESPressio::Persistence::FileReplaceStatus::ReadOnly;
        }

        [[nodiscard]] ESPressio::Persistence::FileRemoveStatus RemoveFile(
            ESPressio::Persistence::FilePathView
        ) noexcept {
            return ESPressio::Persistence::FileRemoveStatus::ReadOnly;
        }

    };


    using PersistenceComposition = Framework::Composition<
        ESPressio::Persistence::Domain,
        DemoFileStorage
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
        ESPressio::Persistence::FileStorageProvider<DemoFileStorage>,
        "The demo FileStorage provider must satisfy the Persistence contract"
    );

    static_assert(
        ESPressio::Localisation::PackSourceProvider<PackSource>,
        "FilePackSource must satisfy the Localisation Pack Source contract"
    );


    [[nodiscard]] int Run(
        char* Text,
        std::size_t TextCapacity,
        char* SupplyingLanguage,
        std::size_t SupplyingLanguageCapacity
    ) noexcept {
        ByteOperations Bytes;
        DemoFileStorage Storage(
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
            return 1;
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
            return 2;
        }

        return Result.Facts.IsSet(
            ESPressio::Localisation::LocalisationFact::LanguageFallbackUsed
        ) ? 0 : 3;
    }

} // Demo


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

    Serial.print("Resolved through FilePackSource: ");
    Serial.println(Text);
    Serial.print("Supplying language: ");
    Serial.println(SupplyingLanguage);
}


void loop() {
}
