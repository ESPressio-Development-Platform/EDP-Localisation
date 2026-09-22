#pragma once

#include <ESPressio_System.hpp>
#include <memory/MemoryComposition.hpp>

namespace ESPressio::Localisation {

    /// Short name for the EDP-System compile-time Composition Framework.
    namespace Framework = ESPressio::System::CompositionFramework;


    /// Composition domain containing Localisation capabilities.
    struct Domain final : Framework::Domain {};


    /// Shared capability supplying immutable compiled Localisation pack bytes.
    struct PackSource final : Framework::SharedCapability<Domain> {};


    /// Cross-domain requirement for the raw byte operations used by Localisation.
    using ByteOperationsNeed = Framework::Need<
        ESPressio::Memory::ByteOperations
    >;

} // ESPressio::Localisation
