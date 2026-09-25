# Public API

Core public types include language identifiers/views, `LocalisationContext`, result/status/fact types, writable text views/output modes, PackSource contracts/resources, `InBinaryPackSource`, Resolver and generated ContractFamily integration.

The optional Persistence umbrella adds `FilePackSource`. Resolution operations separately expose ordinary strings, language display names, Type name/description and Field name/description. A missing representation means continue fallback; a present empty representation is a successful empty value and stops fallback.

Output supports raw UTF-8 or null-terminated UTF-8 with code-point-safe truncation and explicit BufferTooSmall success facts.

Exact declarations remain authoritative in the exported headers.

## Type and Field identifiers

`ESPressio::Localisation::TypeIdentifierBytes` reflects `ESPressio::System::TypeIdentifier::Size` and equals `8`. `ESPressio::Localisation::FieldIdentifierBytes` reflects `ESPressio::System::FieldIdentifier::Size` and equals `1`.

For schema-backed ContractFamilies, `ContractIdentifiers<TContract>::TypeIdentifier` is exactly `ESPressio::System::TypeIdentifier` and `ContractIdentifiers<TContract>::FieldIdentifier` is exactly `ESPressio::System::FieldIdentifier`. Localisation therefore consumes, but does not redefine, the platform's Type/Field semantic identity domain.

The owning Type's 24-bit Authority and 40-bit authority-local value must both be non-zero. Field values `0..255` are all valid and are meaningful only within that owning Type. `FieldPresentationIdentifier` is the complete presentation key `(TypeIdentifier, FieldIdentifier)`.

Type/Field resolution methods reject invalid System Type identities with `LocalisationStatus::InvalidArgument` before fallback or pack lookup. `ContractIdentifiers<TContract>` permits only `0/0` for the no-schema sentinel or fixed `8/1` for a real Type/Field presentation universe.
