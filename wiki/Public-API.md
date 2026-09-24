# Public API

Core public types include language identifiers/views, `LocalisationContext`, result/status/fact types, writable text views/output modes, PackSource contracts/resources, `InBinaryPackSource`, Resolver and generated ContractFamily integration.

The optional Persistence umbrella adds `FilePackSource`. Resolution operations separately expose ordinary strings, language display names, Type name/description and Field name/description. A missing representation means continue fallback; a present empty representation is a successful empty value and stops fallback.

Output supports raw UTF-8 or null-terminated UTF-8 with code-point-safe truncation and explicit BufferTooSmall success facts.

Exact declarations remain authoritative in the exported headers.

## Type and Field identifiers

`ESPressio::Localisation::TypeIdentifierBytes` reflects `ESPressio::System::TypeIdentifier::Size` and therefore equals `8`. For schema-backed ContractFamilies, `ContractIdentifiers<TContract>::TypeIdentifier` is exactly the universal `ESPressio::System::TypeIdentifier`, not a Localisation-owned duplicate. The 24-bit Authority and 40-bit authority-local value must both be non-zero. `ContractIdentifiers<TContract>` permits `TContract::TypeIdentifierBytes == 0` only to make Type/Field identifiers unavailable when a ContractFamily has no schema presentation universe. Field identifiers remain local to their owning Type.
