#include <algorithm>
#include <cstddef>
#include <cstdint>

#include <ESPressio_Persistence.hpp>
#include <ESPressio_System.hpp>
#include <memory/ByteOperationsContract.hpp>
#include <memory/MemoryComposition.hpp>
#include <memory/MemoryTypes.hpp>

#include <localisation/FilePackSource.hpp>

#include "generated/TestPacks.hpp"

namespace TestFilePackSource {

    namespace Framework = ESPressio::System::CompositionFramework;


    class TestByteOperations final : public Framework::Provider<
        ESPressio::Memory::Domain,
        Framework::Provides<
            Framework::Offer<ESPressio::Memory::ByteOperations>
        >
    > {
    public:

        void CopyBytes(
            void* Destination,
            const void* Source,
            std::size_t ByteCount
        ) const noexcept {
            auto* DestinationBytes = static_cast<std::uint8_t*>(Destination);
            const auto* SourceBytes = static_cast<const std::uint8_t*>(Source);

            for (std::size_t Index = 0U; Index < ByteCount; ++Index) {
                DestinationBytes[Index] = SourceBytes[Index];
            }
        }

        void MoveBytes(
            void* Destination,
            const void* Source,
            std::size_t ByteCount
        ) const noexcept {
            auto* DestinationBytes = static_cast<std::uint8_t*>(Destination);
            const auto* SourceBytes = static_cast<const std::uint8_t*>(Source);

            if (
                DestinationBytes == SourceBytes ||
                ByteCount == 0U
            ) {
                return;
            }

            if (DestinationBytes < SourceBytes) {
                CopyBytes(
                    Destination,
                    Source,
                    ByteCount
                );
                return;
            }

            for (std::size_t Index = ByteCount; Index > 0U; --Index) {
                DestinationBytes[Index - 1U] = SourceBytes[Index - 1U];
            }
        }

        void FillBytes(
            void* Destination,
            std::uint8_t Value,
            std::size_t ByteCount
        ) const noexcept {
            auto* DestinationBytes = static_cast<std::uint8_t*>(Destination);

            for (std::size_t Index = 0U; Index < ByteCount; ++Index) {
                DestinationBytes[Index] = Value;
            }
        }

        [[nodiscard]] ESPressio::Memory::ByteComparison CompareBytes(
            const void* Left,
            const void* Right,
            std::size_t ByteCount
        ) const noexcept {
            const auto* LeftBytes = static_cast<const std::uint8_t*>(Left);
            const auto* RightBytes = static_cast<const std::uint8_t*>(Right);

            for (std::size_t Index = 0U; Index < ByteCount; ++Index) {
                if (LeftBytes[Index] < RightBytes[Index]) {
                    return ESPressio::Memory::ByteComparison::Less;
                }

                if (LeftBytes[Index] > RightBytes[Index]) {
                    return ESPressio::Memory::ByteComparison::Greater;
                }
            }

            return ESPressio::Memory::ByteComparison::Equal;
        }

    };


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


    class TestFileProvider final : public Framework::Provider<
        ESPressio::Persistence::Domain,
        Framework::Provides<FileOffer>
    > {
    private:

        ESPressio::Persistence::FileSizeStatus SizeStatus_;
        ESPressio::Persistence::FileReadStatus ReadStatus_;
        const TestByteOperations* ByteOperations_;

        [[nodiscard]] static bool IsEnglishPackPath(
            ESPressio::Persistence::FilePathView Path
        ) noexcept {
            constexpr char Expected[] = "localisation/en-GB.edploc";
            constexpr std::size_t ExpectedSize = sizeof(Expected) - 1U;

            if (Path.Size() != ExpectedSize) {
                return false;
            }

            for (std::size_t Index = 0U; Index < ExpectedSize; ++Index) {
                if (Path.Data()[Index] != Expected[Index]) {
                    return false;
                }
            }

            return true;
        }

    public:

        explicit TestFileProvider(
            const TestByteOperations& ByteOperationsValue
        ) noexcept :
            SizeStatus_(ESPressio::Persistence::FileSizeStatus::Succeeded),
            ReadStatus_(ESPressio::Persistence::FileReadStatus::Succeeded),
            ByteOperations_(&ByteOperationsValue) {}


        void SetSizeStatus(
            ESPressio::Persistence::FileSizeStatus Status
        ) noexcept {
            SizeStatus_ = Status;
        }

        void SetReadStatus(
            ESPressio::Persistence::FileReadStatus Status
        ) noexcept {
            ReadStatus_ = Status;
        }


        [[nodiscard]] bool IsFileStorageReady() const noexcept {
            return true;
        }

        [[nodiscard]] ESPressio::Persistence::FileSizeResult GetFileSize(
            ESPressio::Persistence::FilePathView Path
        ) const noexcept {
            if (!IsEnglishPackPath(Path)) {
                return {
                    ESPressio::Persistence::FileSizeStatus::NotFound,
                    ESPressio::Persistence::StorageSize{}
                };
            }

            if (
                SizeStatus_ !=
                ESPressio::Persistence::FileSizeStatus::Succeeded
            ) {
                return {
                    SizeStatus_,
                    ESPressio::Persistence::StorageSize{}
                };
            }

            return {
                ESPressio::Persistence::FileSizeStatus::Succeeded,
                ESPressio::Persistence::StorageSize{
                    sizeof(TestGenerated::EnglishPack)
                }
            };
        }

        [[nodiscard]] ESPressio::Persistence::FileReadResult ReadFileAt(
            ESPressio::Persistence::FilePathView Path,
            ESPressio::Persistence::StorageOffset Offset,
            ESPressio::Persistence::DestinationBufferView Destination
        ) const noexcept {
            if (!IsEnglishPackPath(Path)) {
                return {
                    ESPressio::Persistence::FileReadStatus::NotFound,
                    static_cast<std::uint8_t>(
                        ESPressio::Persistence::ReadFact::None
                    ),
                    0U,
                    ESPressio::Persistence::StorageSize{}
                };
            }

            if (
                ReadStatus_ !=
                ESPressio::Persistence::FileReadStatus::Succeeded
            ) {
                return {
                    ReadStatus_,
                    static_cast<std::uint8_t>(
                        ESPressio::Persistence::ReadFact::None
                    ),
                    0U,
                    ESPressio::Persistence::StorageSize{}
                };
            }

            const std::size_t PackSize = sizeof(TestGenerated::EnglishPack);

            if (Offset.RawValue > PackSize) {
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
                PackSize - static_cast<std::size_t>(Offset.RawValue);
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
                    TestGenerated::EnglishPack +
                        static_cast<std::size_t>(Offset.RawValue),
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
        TestFileProvider
    >;

    inline constexpr ESPressio::Localisation::StaticPackDirectory PackDirectory{
        "localisation"
    };

    using Source = ESPressio::Localisation::FilePackSource<
        PersistenceComposition,
        PackDirectory,
        TestGenerated::Contract,
        TestByteOperations
    >;

    using MemoryComposition = Framework::Composition<
        ESPressio::Memory::Domain,
        TestByteOperations
    >;

    using LocalisationComposition = Framework::Composition<
        ESPressio::Localisation::Domain,
        Source
    >;

    using TestArchitecture = Framework::Architecture<
        MemoryComposition,
        PersistenceComposition,
        LocalisationComposition
    >;


    static_assert(
        TestArchitecture::IsValid,
        "FilePackSource test Architecture must satisfy Memory and Persistence requirements"
    );

    static_assert(
        Source::PersistenceRequirement::Scope ==
            Framework::RequirementScope::ExternalDomain,
        "FilePackSource Persistence requirement must remain cross-domain"
    );

    static_assert(
        Source::PersistenceRequirement::Cardinality::Minimum == 1U &&
        Source::PersistenceRequirement::Cardinality::Maximum == 1U,
        "FilePackSource must require exactly one qualified FileStorage provider"
    );

    static_assert(
        ESPressio::Persistence::FileStorageProvider<TestFileProvider>,
        "TestFileProvider must satisfy EDP-Persistence FileStorage"
    );

    static_assert(
        ESPressio::Localisation::PackSourceProvider<Source>,
        "FilePackSource must satisfy the common Localisation Pack Source contract"
    );

    static_assert(
        Source::RequiredMaximumPackPathBytes == 25U,
        "FilePackSource path bound must derive from directory, slash, maximum language and .edploc"
    );


    [[nodiscard]] bool Run() {
        TestByteOperations ByteOperations;
        TestFileProvider Storage(
            ByteOperations
        );
        Source PackSource(
            Storage,
            ByteOperations
        );

        const auto English =
            ESPressio::Localisation::LanguageIdentifierView::Validate("en-GB");
        const auto German =
            ESPressio::Localisation::LanguageIdentifierView::Validate("de");

        if (
            !English.IsValuePresent ||
            !German.IsValuePresent
        ) {
            return false;
        }

        const auto Missing = PackSource.Locate(
            German.Value
        );

        if (
            Missing.Status !=
                ESPressio::Localisation::PackLocateStatus::PackUnavailable ||
            Missing.Resource.has_value()
        ) {
            return false;
        }

        const auto Located = PackSource.Locate(
            English.Value
        );

        if (
            Located.Status !=
                ESPressio::Localisation::PackLocateStatus::Success ||
            !Located.Resource.has_value()
        ) {
            return false;
        }

        const auto Size = PackSource.Size(
            *Located.Resource
        );

        if (
            Size.Status !=
                ESPressio::Localisation::PackSizeStatus::Success ||
            Size.SizeBytes != sizeof(TestGenerated::EnglishPack)
        ) {
            return false;
        }

        Storage.SetSizeStatus(
            ESPressio::Persistence::FileSizeStatus::NotReady
        );
        const auto LocateProviderUnavailable = PackSource.Locate(
            English.Value
        );

        if (
            LocateProviderUnavailable.Status !=
                ESPressio::Localisation::PackLocateStatus::ProviderUnavailable
        ) {
            return false;
        }

        Storage.SetSizeStatus(
            ESPressio::Persistence::FileSizeStatus::IoFailure
        );
        const auto LocateReadFailure = PackSource.Locate(
            English.Value
        );

        if (
            LocateReadFailure.Status !=
                ESPressio::Localisation::PackLocateStatus::ReadFailure
        ) {
            return false;
        }

        Storage.SetSizeStatus(
            ESPressio::Persistence::FileSizeStatus::NotFound
        );
        const auto ResourceUnavailable = PackSource.Size(
            *Located.Resource
        );

        if (
            ResourceUnavailable.Status !=
                ESPressio::Localisation::PackSizeStatus::ResourceUnavailable
        ) {
            return false;
        }

        Storage.SetSizeStatus(
            ESPressio::Persistence::FileSizeStatus::Succeeded
        );

        std::uint8_t Magic[4U]{};
        const auto Read = PackSource.Read(
            *Located.Resource,
            0U,
            {
                Magic,
                sizeof(Magic)
            }
        );

        if (
            Read.Status !=
                ESPressio::Localisation::PackReadStatus::Success ||
            Read.BytesRead != sizeof(Magic) ||
            Magic[0U] != 'E' ||
            Magic[1U] != 'D' ||
            Magic[2U] != 'P' ||
            Magic[3U] != 'L'
        ) {
            return false;
        }

        Storage.SetReadStatus(
            ESPressio::Persistence::FileReadStatus::NotReady
        );
        const auto ReadProviderUnavailable = PackSource.Read(
            *Located.Resource,
            0U,
            {
                Magic,
                sizeof(Magic)
            }
        );

        if (
            ReadProviderUnavailable.Status !=
                ESPressio::Localisation::PackReadStatus::ProviderUnavailable
        ) {
            return false;
        }

        Storage.SetReadStatus(
            ESPressio::Persistence::FileReadStatus::NotFound
        );
        const auto ReadResourceUnavailable = PackSource.Read(
            *Located.Resource,
            0U,
            {
                Magic,
                sizeof(Magic)
            }
        );

        if (
            ReadResourceUnavailable.Status !=
                ESPressio::Localisation::PackReadStatus::ResourceUnavailable
        ) {
            return false;
        }

        Storage.SetReadStatus(
            ESPressio::Persistence::FileReadStatus::InvalidOffset
        );
        const auto ExplicitOutOfRange = PackSource.Read(
            *Located.Resource,
            0U,
            {
                Magic,
                sizeof(Magic)
            }
        );

        if (
            ExplicitOutOfRange.Status !=
                ESPressio::Localisation::PackReadStatus::OutOfRange
        ) {
            return false;
        }

        Storage.SetReadStatus(
            ESPressio::Persistence::FileReadStatus::Succeeded
        );

        const auto InvalidDestination = PackSource.Read(
            *Located.Resource,
            0U,
            {
                nullptr,
                1U
            }
        );

        if (
            InvalidDestination.Status !=
                ESPressio::Localisation::PackReadStatus::ReadFailure
        ) {
            return false;
        }

        std::uint8_t TooLarge[8U]{};
        const auto OutOfRange = PackSource.Read(
            *Located.Resource,
            sizeof(TestGenerated::EnglishPack) - 2U,
            {
                TooLarge,
                sizeof(TooLarge)
            }
        );

        if (
            OutOfRange.Status !=
                ESPressio::Localisation::PackReadStatus::OutOfRange ||
            OutOfRange.BytesRead != 2U
        ) {
            return false;
        }

        return PackSource.LanguageIdentity(
            *Located.Resource
        ).IsEqualTo(
            English.Value
        );
    }

} // TestFilePackSource


int main() {
    return TestFilePackSource::Run()
        ? 0
        : 1;
}
