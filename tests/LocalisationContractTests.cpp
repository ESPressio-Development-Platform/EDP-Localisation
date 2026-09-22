#include <array>
#include <cstddef>
#include <cstdint>
#include <optional>
#include <type_traits>

#include <ESPressio_Localisation.hpp>

#include "generated/TestPacks.hpp"

namespace Test {

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

            const auto DestinationAddress =
                reinterpret_cast<std::uintptr_t>(DestinationBytes);
            const auto SourceAddress =
                reinterpret_cast<std::uintptr_t>(SourceBytes);

            if (
                DestinationAddress < SourceAddress ||
                DestinationAddress >= SourceAddress + ByteCount
            ) {
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


    struct NoSchemaContract final {

        static constexpr std::uint8_t FormatMajor = 1U;
        static constexpr std::uint8_t FormatMinor = 0U;
        static constexpr std::size_t SupportedLanguageCount = 1U;
        static constexpr std::size_t MaximumSupportedLanguageIdentifierBytes = 5U;
        static constexpr std::uint8_t DomainIdentifierBytes = 1U;
        static constexpr std::uint8_t SubDomainIdentifierBytes = 1U;
        static constexpr std::uint8_t StringIdentifierBytes = 2U;
        static constexpr std::uint8_t TypeIdentifierBytes = 0U;
        static constexpr std::uint8_t FieldIdentifierBytes = 0U;

        inline static constexpr std::array<std::uint8_t, 16U> ContractFamilyFingerprint = {
            0x00U, 0x01U, 0x02U, 0x03U,
            0x04U, 0x05U, 0x06U, 0x07U,
            0x08U, 0x09U, 0x0AU, 0x0BU,
            0x0CU, 0x0DU, 0x0EU, 0x0FU
        };

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


    [[nodiscard]] bool IsTextEqual(
        const char* Actual,
        std::size_t ActualLength,
        const char* Expected,
        std::size_t ExpectedLength
    ) noexcept {
        if (ActualLength != ExpectedLength) { return false; }

        for (std::size_t Index = 0U; Index < ActualLength; ++Index) {
            if (Actual[Index] != Expected[Index]) { return false; }
        }

        return true;
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


    [[nodiscard]] bool ValidateResolver() {
        using Source = ESPressio::Localisation::InBinaryPackSource<TestByteOperations>;
        using Resolver = ESPressio::Localisation::Resolver<
            Source,
            TestByteOperations,
            TestGenerated::Contract
        >;

        TestByteOperations ByteOperations;
        Source PackSource(
            TestGenerated::Descriptors,
            sizeof(TestGenerated::Descriptors) / sizeof(TestGenerated::Descriptors[0]),
            ByteOperations
        );
        Resolver Localisation(
            PackSource,
            ByteOperations
        );

        const ESPressio::Localisation::LocalisationContext Context{
            TestGenerated::GermanValidation.Value,
            TestGenerated::EnglishValidation.Value
        };

        if (
            Localisation.ValidateLanguagePack(
                TestGenerated::EnglishValidation.Value
            ).Status != ESPressio::Localisation::ValidationStatus::Success ||
            Localisation.ValidateLanguagePack(
                TestGenerated::GermanValidation.Value
            ).Status != ESPressio::Localisation::ValidationStatus::Success ||
            Localisation.ValidateContext(Context).Status !=
                ESPressio::Localisation::ValidationStatus::Success
        ) {
            return false;
        }

        const typename Resolver::GeneralStringIdentifier Greeting{
            typename Resolver::Identifiers::DomainIdentifier(1U),
            typename Resolver::Identifiers::SubDomainIdentifier(0U),
            typename Resolver::Identifiers::StringIdentifierValue(3U)
        };

        char Text[32U]{};
        const auto GreetingResult = Localisation.ResolveString(
            Context,
            Greeting,
            {
                Text,
                sizeof(Text)
            },
            ESPressio::Localisation::TextOutputMode::NullTerminatedUtf8
        );

        if (
            GreetingResult.Status != ESPressio::Localisation::LocalisationStatus::Success ||
            GreetingResult.BytesWritten != 5U ||
            GreetingResult.RequiredBytes != 5U ||
            !GreetingResult.Facts.IsSet(
                ESPressio::Localisation::LocalisationFact::LanguageFallbackUsed
            ) ||
            GreetingResult.Facts.IsSet(
                ESPressio::Localisation::LocalisationFact::BufferTooSmall
            ) ||
            !GreetingResult.ResolvedLanguage.has_value() ||
            !IsTextEqual(
                Text,
                GreetingResult.BytesWritten,
                "Hello",
                5U
            )
        ) {
            return false;
        }

        const auto GreetingSizeQuery = Localisation.ResolveString(
            Context,
            Greeting,
            {
                nullptr,
                0U
            },
            ESPressio::Localisation::TextOutputMode::RawUtf8
        );

        if (
            GreetingSizeQuery.Status !=
                ESPressio::Localisation::LocalisationStatus::Success ||
            GreetingSizeQuery.BytesWritten != 0U ||
            GreetingSizeQuery.RequiredBytes != 5U ||
            !GreetingSizeQuery.Facts.IsSet(
                ESPressio::Localisation::LocalisationFact::BufferTooSmall
            ) ||
            !GreetingSizeQuery.Facts.IsSet(
                ESPressio::Localisation::LocalisationFact::LanguageFallbackUsed
            )
        ) {
            return false;
        }

        const typename Resolver::GeneralStringIdentifier MissingIdentifier{
            typename Resolver::Identifiers::DomainIdentifier(1U),
            typename Resolver::Identifiers::SubDomainIdentifier(0U),
            typename Resolver::Identifiers::StringIdentifierValue(6U)
        };

        const auto MissingResult = Localisation.ResolveString(
            Context,
            MissingIdentifier,
            {
                Text,
                sizeof(Text)
            },
            ESPressio::Localisation::TextOutputMode::NullTerminatedUtf8
        );

        if (
            MissingResult.Status !=
                ESPressio::Localisation::LocalisationStatus::NoStringFoundForIdentifier ||
            MissingResult.BytesWritten != 0U ||
            MissingResult.RequiredBytes != 0U ||
            MissingResult.ResolvedLanguage.has_value()
        ) {
            return false;
        }

        char ResolvedLanguage[8U]{};
        const auto LanguageResult = Localisation.ResolveLanguageIdentity(
            *GreetingResult.ResolvedLanguage,
            {
                ResolvedLanguage,
                sizeof(ResolvedLanguage)
            },
            ESPressio::Localisation::TextOutputMode::NullTerminatedUtf8
        );

        if (
            LanguageResult.Status != ESPressio::Localisation::TextMaterialisationStatus::Success ||
            LanguageResult.BytesWritten != 5U ||
            LanguageResult.RequiredBytes != 5U ||
            !IsTextEqual(
                ResolvedLanguage,
                LanguageResult.BytesWritten,
                "en-GB",
                5U
            )
        ) {
            return false;
        }

        const typename Resolver::GeneralStringIdentifier ExplicitEmpty{
            typename Resolver::Identifiers::DomainIdentifier(1U),
            typename Resolver::Identifiers::SubDomainIdentifier(0U),
            typename Resolver::Identifiers::StringIdentifierValue(4U)
        };

        Text[0U] = 'X';
        const auto EmptyResult = Localisation.ResolveString(
            Context,
            ExplicitEmpty,
            {
                Text,
                sizeof(Text)
            },
            ESPressio::Localisation::TextOutputMode::NullTerminatedUtf8
        );

        if (
            EmptyResult.Status != ESPressio::Localisation::LocalisationStatus::Success ||
            EmptyResult.BytesWritten != 0U ||
            EmptyResult.RequiredBytes != 0U ||
            Text[0U] != '\0' ||
            EmptyResult.Facts.IsSet(
                ESPressio::Localisation::LocalisationFact::LanguageFallbackUsed
            )
        ) {
            return false;
        }

        const auto EmptyZeroCapacity = Localisation.ResolveString(
            Context,
            ExplicitEmpty,
            {
                nullptr,
                0U
            },
            ESPressio::Localisation::TextOutputMode::NullTerminatedUtf8
        );

        if (
            EmptyZeroCapacity.Status != ESPressio::Localisation::LocalisationStatus::Success ||
            !EmptyZeroCapacity.Facts.IsSet(
                ESPressio::Localisation::LocalisationFact::BufferTooSmall
            )
        ) {
            return false;
        }

        const typename Resolver::GeneralStringIdentifier GermanUtf8{
            typename Resolver::Identifiers::DomainIdentifier(1U),
            typename Resolver::Identifiers::SubDomainIdentifier(0U),
            typename Resolver::Identifiers::StringIdentifierValue(5U)
        };

        char Truncated[4U]{};
        const auto TruncatedResult = Localisation.ResolveString(
            Context,
            GermanUtf8,
            {
                Truncated,
                sizeof(Truncated)
            },
            ESPressio::Localisation::TextOutputMode::NullTerminatedUtf8
        );

        if (
            TruncatedResult.Status != ESPressio::Localisation::LocalisationStatus::Success ||
            TruncatedResult.BytesWritten != 2U ||
            TruncatedResult.RequiredBytes != 7U ||
            !TruncatedResult.Facts.IsSet(
                ESPressio::Localisation::LocalisationFact::BufferTooSmall
            ) ||
            Truncated[0U] != 'G' ||
            Truncated[1U] != 'r' ||
            Truncated[2U] != '\0'
        ) {
            return false;
        }

        const auto DisplayResult = Localisation.ResolveLanguageDisplayName(
            Context,
            TestGenerated::EnglishValidation.Value,
            {
                Text,
                sizeof(Text)
            },
            ESPressio::Localisation::TextOutputMode::NullTerminatedUtf8
        );

        if (
            DisplayResult.Status != ESPressio::Localisation::LocalisationStatus::Success ||
            DisplayResult.Facts.IsSet(
                ESPressio::Localisation::LocalisationFact::LanguageFallbackUsed
            ) ||
            !IsTextEqual(
                Text,
                DisplayResult.BytesWritten,
                "Britisches Englisch",
                sizeof("Britisches Englisch") - 1U
            )
        ) {
            return false;
        }

        const typename Resolver::TypeIdentifier Type(
            std::array<std::uint8_t, 8U>{
                0x01U,
                0x23U,
                0x45U,
                0x67U,
                0x89U,
                0xABU,
                0xCDU,
                0xEFU
            }
        );

        const auto TypeResult = Localisation.ResolveTypeName(
            Context,
            Type,
            {
                Text,
                sizeof(Text)
            },
            ESPressio::Localisation::TextOutputMode::NullTerminatedUtf8
        );

        if (
            TypeResult.Status != ESPressio::Localisation::LocalisationStatus::Success ||
            !TypeResult.Facts.IsSet(
                ESPressio::Localisation::LocalisationFact::LanguageFallbackUsed
            ) ||
            !IsTextEqual(
                Text,
                TypeResult.BytesWritten,
                "Temperature Reading",
                sizeof("Temperature Reading") - 1U
            )
        ) {
            return false;
        }

        const typename Resolver::FieldPresentationIdentifier Field{
            Type,
            typename Resolver::Identifiers::FieldIdentifier(0U)
        };

        const auto FieldResult = Localisation.ResolveFieldName(
            Context,
            Field,
            {
                Text,
                sizeof(Text)
            },
            ESPressio::Localisation::TextOutputMode::NullTerminatedUtf8
        );

        if (
            FieldResult.Status != ESPressio::Localisation::LocalisationStatus::Success ||
            !FieldResult.Facts.IsSet(
                ESPressio::Localisation::LocalisationFact::LanguageFallbackUsed
            ) ||
            !IsTextEqual(
                Text,
                FieldResult.BytesWritten,
                "Temperature",
                sizeof("Temperature") - 1U
            )
        ) {
            return false;
        }

        const ESPressio::Localisation::InBinaryPackDescriptor EnglishOnlyDescriptors[]{
            {
                TestGenerated::EnglishValidation.Value,
                TestGenerated::EnglishPack,
                sizeof(TestGenerated::EnglishPack)
            }
        };
        Source EnglishOnlySource(
            EnglishOnlyDescriptors,
            1U,
            ByteOperations
        );
        Resolver EnglishOnlyResolver(
            EnglishOnlySource,
            ByteOperations
        );

        const auto MissingRequestedPack = EnglishOnlyResolver.ResolveString(
            Context,
            Greeting,
            {
                Text,
                sizeof(Text)
            },
            ESPressio::Localisation::TextOutputMode::NullTerminatedUtf8
        );

        if (
            MissingRequestedPack.Status !=
                ESPressio::Localisation::LocalisationStatus::LanguagePackUnavailable ||
            MissingRequestedPack.ResolvedLanguage.has_value()
        ) {
            return false;
        }

        std::array<std::uint8_t, sizeof(TestGenerated::EnglishPack)> CorruptPack{};
        ByteOperations.CopyBytes(
            CorruptPack.data(),
            TestGenerated::EnglishPack,
            CorruptPack.size()
        );
        CorruptPack[CorruptPack.size() - 1U] ^= 0x01U;

        const ESPressio::Localisation::InBinaryPackDescriptor CorruptDescriptor[]{
            {
                TestGenerated::EnglishValidation.Value,
                CorruptPack.data(),
                CorruptPack.size()
            }
        };
        Source CorruptSource(
            CorruptDescriptor,
            1U,
            ByteOperations
        );
        Resolver CorruptResolver(
            CorruptSource,
            ByteOperations
        );

        return
            CorruptResolver.ValidateLanguagePack(
                TestGenerated::EnglishValidation.Value
            ).Status == ESPressio::Localisation::ValidationStatus::InvalidDataset;
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


using NoSchemaIdentifiers =
    ESPressio::Localisation::ContractIdentifiers<Test::NoSchemaContract>;

using NoSchemaResolver = ESPressio::Localisation::Resolver<
    Test::TestPackSource,
    Test::TestByteOperations,
    Test::NoSchemaContract
>;

static_assert(
    !std::is_default_constructible_v<NoSchemaIdentifiers::TypeIdentifier>,
    "Type identifiers must be unavailable when the ContractFamily has no Type presentation universe"
);

static_assert(
    !std::is_default_constructible_v<NoSchemaIdentifiers::FieldIdentifier>,
    "Field identifiers must be unavailable when the ContractFamily has no Type presentation universe"
);

static_assert(
    sizeof(NoSchemaResolver) > 0U,
    "Resolver must remain a valid type for a ContractFamily without Type/Field presentation"
);

static_assert(
    std::is_same_v<
        ESPressio::Localisation::ContractIdentifiers<TestGenerated::Contract>::DomainIdentifier::Storage,
        std::uint8_t
    >,
    "Domain identifier storage width must follow the generated contract"
);

static_assert(
    std::is_same_v<
        ESPressio::Localisation::ContractIdentifiers<TestGenerated::Contract>::StringIdentifierValue::Storage,
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

    if (!Test::ValidateResolver()) {
        return 3;
    }

    return 0;
}
