#include <Arduino.h>

#include <array>
#include <cstddef>
#include <cstdint>

#include <ESPressio_Localisation.hpp>
#include <memory/ByteOperationsProvider.hpp>

#include "DemoGenerated.hpp"

namespace Demo {

    using ByteOperations =
        ESPressio::Platform::Portable::Memory::ByteOperationsProvider;

    using PackSource =
        ESPressio::Localisation::InBinaryPackSource<ByteOperations>;

    using Resolver = ESPressio::Localisation::Resolver<
        PackSource,
        ByteOperations,
        DemoGenerated::Contract
    >;


    [[nodiscard]] int Run() noexcept {
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
            return 1;
        }

        if (
            Localisation.ValidateLanguagePack(
                DemoGenerated::GermanValidation.Value
            ).Status != ESPressio::Localisation::ValidationStatus::Success
        ) {
            return 2;
        }

        if (
            Localisation.ValidateContext(Context).Status !=
                ESPressio::Localisation::ValidationStatus::Success
        ) {
            return 3;
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
            return 4;
        }

        Serial.println("Both generated packs and the fallback context are valid.");
        Serial.println("The deliberately corrupted pack was rejected by CRC32C validation.");
        return 0;
    }

} // Demo


void setup() {
    Serial.begin(115200);

    const int Result = Demo::Run();

    if (Result != 0) {
        Serial.print("Validation demo failed: ");
        Serial.println(Result);
    }
}


void loop() {
}
