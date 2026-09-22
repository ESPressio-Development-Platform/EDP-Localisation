#include <cstddef>
#include <cstdint>
#include <optional>
#include <type_traits>

#include <ESPressio_Localisation.hpp>

namespace Test {

    namespace Framework = ESPressio::System::CompositionFramework;


    struct Contract final {

        static constexpr std::uint8_t DomainIdentifierBytes = 1U;
        static constexpr std::uint8_t SubDomainIdentifierBytes = 1U;
        static constexpr std::uint8_t StringIdentifierBytes = 2U;
        static constexpr std::uint8_t TypeIdentifierBytes = 8U;
        static constexpr std::uint8_t FieldIdentifierBytes = 2U;

    };


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
                for (std::size_t Index = 0U; Index < ByteCount; ++Index) {
                    DestinationBytes[Index] = SourceBytes[Index];
                }

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


    class TestPackSource final : public Framework::Provider<
        ESPressio::Localisation::Domain,
        Framework::Provides<
            Framework::Offer<ESPressio::Localisation::PackSource>
        >,
        Framework::Requires<>,
        Framework::DependsOn<
            ESPressio::Localisation::ByteOperationsNeed
        >
    > {
    public:

        struct PackResource final {

            const char* LanguageData;
            std::uint8_t LanguageLength;

        };


        [[nodiscard]] ESPressio::Localisation::PackLocateResult<PackResource> Locate(
            ESPressio::Localisation::LanguageIdentifierView Language
        ) const noexcept {
            return {
                ESPressio::Localisation::PackLocateStatus::Success,
                PackResource{
                    Language.Data(),
                    Language.Length()
                }
            };
        }

        [[nodiscard]] ESPressio::Localisation::PackSizeResult Size(
            const PackResource&
        ) const noexcept {
            return {
                ESPressio::Localisation::PackSizeStatus::Success,
                0U
            };
        }

        [[nodiscard]] ESPressio::Localisation::PackReadResult Read(
            const PackResource&,
            std::uint64_t,
            ESPressio::Localisation::WritableByteView Destination
        ) const noexcept {
            return {
                ESPressio::Localisation::PackReadStatus::Success,
                Destination.Size
            };
        }

        [[nodiscard]] ESPressio::Localisation::LanguageIdentifierView LanguageIdentity(
            const PackResource& Resource
        ) const noexcept {
            const auto Result = ESPressio::Localisation::LanguageIdentifierView::Validate(
                Resource.LanguageData,
                Resource.LanguageLength
            );

            return Result.Value;
        }

    };


    constexpr bool ValidateLanguageIdentifiers() {
        const auto English = ESPressio::Localisation::LanguageIdentifierView::Validate("en");
        const auto BritishEnglish = ESPressio::Localisation::LanguageIdentifierView::Validate("en-GB");
        const auto TraditionalChinese = ESPressio::Localisation::LanguageIdentifierView::Validate("zh-Hant-TW");
        const auto Variant = ESPressio::Localisation::LanguageIdentifierView::Validate("de-CH-1901");
        const auto Extension = ESPressio::Localisation::LanguageIdentifierView::Validate("en-US-u-ca-gregory");
        const auto PrivateUse = ESPressio::Localisation::LanguageIdentifierView::Validate("x-espressio-test");

        const auto BadPrimaryCase = ESPressio::Localisation::LanguageIdentifierView::Validate("EN");
        const auto BadRegionCase = ESPressio::Localisation::LanguageIdentifierView::Validate("en-gb");
        const auto BadScriptCase = ESPressio::Localisation::LanguageIdentifierView::Validate("zh-hant");
        const auto EmptySegment = ESPressio::Localisation::LanguageIdentifierView::Validate("en--GB");
        const auto EmptyExtension = ESPressio::Localisation::LanguageIdentifierView::Validate("en-US-u");

        return
            English.IsValuePresent &&
            BritishEnglish.IsValuePresent &&
            TraditionalChinese.IsValuePresent &&
            Variant.IsValuePresent &&
            Extension.IsValuePresent &&
            PrivateUse.IsValuePresent &&
            BadPrimaryCase.Status == ESPressio::Localisation::LanguageIdentifierValidationStatus::NonCanonicalCase &&
            BadRegionCase.Status == ESPressio::Localisation::LanguageIdentifierValidationStatus::NonCanonicalCase &&
            BadScriptCase.Status == ESPressio::Localisation::LanguageIdentifierValidationStatus::NonCanonicalCase &&
            EmptySegment.Status == ESPressio::Localisation::LanguageIdentifierValidationStatus::InvalidSyntax &&
            EmptyExtension.Status == ESPressio::Localisation::LanguageIdentifierValidationStatus::InvalidSyntax;
    }


    [[nodiscard]] bool ValidateInBinaryPackSource() {
        constexpr auto Language = ESPressio::Localisation::LanguageIdentifierView::Validate("en-GB");
        static_assert(Language.IsValuePresent);

        static constexpr std::uint8_t PackBytes[] = {
            0x45U,
            0x44U,
            0x50U,
            0x4CU
        };

        static constexpr ESPressio::Localisation::InBinaryPackDescriptor Descriptors[] = {
            {
                Language.Value,
                PackBytes,
                sizeof(PackBytes)
            }
        };

        TestByteOperations ByteOperations;
        ESPressio::Localisation::InBinaryPackSource<TestByteOperations> Source(
            Descriptors,
            1U,
            ByteOperations
        );

        const auto Located = Source.Locate(
            Language.Value
        );

        if (
            Located.Status != ESPressio::Localisation::PackLocateStatus::Success ||
            !Located.Resource.has_value()
        ) {
            return false;
        }

        const auto Size = Source.Size(
            *Located.Resource
        );

        if (
            Size.Status != ESPressio::Localisation::PackSizeStatus::Success ||
            Size.SizeBytes != sizeof(PackBytes)
        ) {
            return false;
        }

        std::uint8_t DestinationBytes[sizeof(PackBytes)]{};
        const auto Read = Source.Read(
            *Located.Resource,
            0U,
            {
                DestinationBytes,
                sizeof(DestinationBytes)
            }
        );

        if (
            Read.Status != ESPressio::Localisation::PackReadStatus::Success ||
            Read.BytesRead != sizeof(DestinationBytes)
        ) {
            return false;
        }

        for (std::size_t Index = 0U; Index < sizeof(PackBytes); ++Index) {
            if (DestinationBytes[Index] != PackBytes[Index]) {
                return false;
            }
        }

        const auto OutOfRange = Source.Read(
            *Located.Resource,
            3U,
            {
                DestinationBytes,
                2U
            }
        );

        return
            OutOfRange.Status == ESPressio::Localisation::PackReadStatus::OutOfRange &&
            Source.LanguageIdentity(*Located.Resource).IsEqualTo(Language.Value);
    }

} // Test


static_assert(
    Test::ValidateLanguageIdentifiers(),
    "Canonical BCP47 language validation contract failed"
);

static_assert(
    sizeof(ESPressio::Localisation::LocalisationFacts) == sizeof(std::uint8_t),
    "LocalisationFacts must remain one byte"
);

static_assert(
    sizeof(ESPressio::Localisation::TextMaterialisationFacts) == sizeof(std::uint8_t),
    "TextMaterialisationFacts must remain one byte"
);

static_assert(
    ESPressio::Localisation::PackSourceProvider<Test::TestPackSource>,
    "TestPackSource must satisfy the Localisation Pack Source concept"
);

static_assert(
    std::is_same_v<
        ESPressio::Localisation::ContractIdentifiers<Test::Contract>::DomainIdentifier::Storage,
        std::uint8_t
    >,
    "Domain identifier storage width must follow the generated contract"
);

static_assert(
    std::is_same_v<
        ESPressio::Localisation::ContractIdentifiers<Test::Contract>::StringIdentifierValue::Storage,
        std::uint16_t
    >,
    "String identifier storage width must follow the generated contract"
);


int main() {
    ESPressio::Localisation::LocalisationFacts Facts;

    Facts.Set(
        ESPressio::Localisation::LocalisationFact::LanguageFallbackUsed,
        ESPressio::Localisation::LocalisationFact::BufferTooSmall
    );

    if (!Facts.HasAll(
        ESPressio::Localisation::LocalisationFact::LanguageFallbackUsed,
        ESPressio::Localisation::LocalisationFact::BufferTooSmall
    )) {
        return 1;
    }

    if (!Test::ValidateInBinaryPackSource()) {
        return 2;
    }

    return 0;
}
