#include <Arduino.h>

#include <array>
#include <cstddef>
#include <cstdint>

#include <ESPressio_Localisation.hpp>
#include <ESPressio_Platform_Portable_ByteOperations.hpp>

#include "DemoGenerated.hpp"

namespace Demo {

    namespace Framework = ESPressio::System::CompositionFramework;


    /// Mutually exclusive outcome of the validation demonstration.
    enum class DemoStatus : std::uint8_t {
        Succeeded = 0U,
        EnglishPackInvalid = 1U,
        GermanPackInvalid = 2U,
        ContextInvalid = 3U,
        CorruptionNotDetected = 4U
    };


    using ByteOperations =
        ESPressio::Platform::Portable::Memory::ByteOperationsProvider;

    using PackSource =
        ESPressio::Localisation::InBinaryPackSource<ByteOperations>;

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
        "Demo Architecture must satisfy consolidated Localisation dependencies"
    );

    static_assert(
        ResolverContractValidation::IsValid,
        "Application Architecture must satisfy Resolver's standalone consumer Contract"
    );


    /// Executes complete pack/context validation and the corruption-rejection check.
    [[nodiscard]] DemoStatus Run() noexcept {
        ByteOperations Bytes;
        PackSource Source(
            DemoGenerated::Descriptors,
            sizeof(DemoGenerated::Descriptors) /
                sizeof(DemoGenerated::Descriptors[0]),
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

        if (
            Localisation.ValidateLanguagePack(
                DemoGenerated::EnglishValidation.Value
            ).Status != ESPressio::Localisation::ValidationStatus::Success
        ) {
            return DemoStatus::EnglishPackInvalid;
        }

        if (
            Localisation.ValidateLanguagePack(
                DemoGenerated::GermanValidation.Value
            ).Status != ESPressio::Localisation::ValidationStatus::Success
        ) {
            return DemoStatus::GermanPackInvalid;
        }

        if (
            Localisation.ValidateContext(Context).Status !=
                ESPressio::Localisation::ValidationStatus::Success
        ) {
            return DemoStatus::ContextInvalid;
        }

        std::array<
            std::uint8_t,
            sizeof(DemoGenerated::EnglishPack)
        > CorruptPack{};

        Bytes.CopyBytes(
            CorruptPack.data(),
            DemoGenerated::EnglishPack,
            CorruptPack.size()
        );
        CorruptPack[CorruptPack.size() - 1U] ^= 0x01U;

        const ESPressio::Localisation::InBinaryPackDescriptor CorruptDescriptors[]{
            {
                DemoGenerated::EnglishValidation.Value,
                CorruptPack.data(),
                CorruptPack.size()
            }
        };

        PackSource CorruptSource(
            CorruptDescriptors,
            1U,
            Bytes
        );
        Resolver CorruptResolver(
            CorruptSource,
            Bytes
        );

        if (
            CorruptResolver.ValidateLanguagePack(
                DemoGenerated::EnglishValidation.Value
            ).Status != ESPressio::Localisation::ValidationStatus::InvalidDataset
        ) {
            return DemoStatus::CorruptionNotDetected;
        }

        Serial.println("Both generated packs and the fallback context are valid.");
        Serial.println("The deliberately corrupted pack was rejected by CRC32C validation.");
        return DemoStatus::Succeeded;
    }

} // Demo


/// Executes the Arduino startup path for the validation demonstration.
void setup() {
    Serial.begin(115200);

    const auto Result = Demo::Run();

    if (Result != Demo::DemoStatus::Succeeded) {
        Serial.print("Validation demo failed: ");
        Serial.println(
            static_cast<unsigned>(Result)
        );
    }
}


/// Provides the intentionally idle Arduino loop for this one-shot demonstration.
void loop() {
}
