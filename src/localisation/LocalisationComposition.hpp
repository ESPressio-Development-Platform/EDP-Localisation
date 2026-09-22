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


    /// Standalone consumer Requirement for exactly one Localisation PackSource provider.
    using PackSourceRequirement = Framework::Requirement<
        PackSource,
        Framework::RequirementScope::AnyDomain,
        Framework::ExactlyProviders<1U>
    >;

    /// Provider Contract Requirement for exactly one external EDP-Memory ByteOperations provider.
    using ExternalByteOperationsRequirement = Framework::Requirement<
        ESPressio::Memory::ByteOperations,
        Framework::RequirementScope::ExternalDomain,
        Framework::ExactlyProviders<1U>
    >;

    /// Standalone consumer Requirement for exactly one EDP-Memory ByteOperations provider.
    using ByteOperationsRequirement = Framework::Requirement<
        ESPressio::Memory::ByteOperations,
        Framework::RequirementScope::AnyDomain,
        Framework::ExactlyProviders<1U>
    >;

    /// Complete compile-time dependency contract of one Resolver consumer.
    ///
    /// Runtime provider ownership remains explicit: Bootstrap still constructs and binds the
    /// selected PackSource and ByteOperations instances to Resolver.
    using ResolverContract = Framework::Contract<
        PackSourceRequirement,
        ByteOperationsRequirement
    >;

} // ESPressio::Localisation
