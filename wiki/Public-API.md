# Public API

Core public types include language identifiers/views, `LocalisationContext`, result/status/fact types, writable text views/output modes, PackSource contracts/resources, `InBinaryPackSource`, Resolver and generated ContractFamily integration.

The optional Persistence umbrella adds `FilePackSource`. Resolution operations separately expose ordinary strings, language display names, Type name/description and Field name/description. A missing representation means continue fallback; a present empty representation is a successful empty value and stops fallback.

Output supports raw UTF-8 or null-terminated UTF-8 with code-point-safe truncation and explicit BufferTooSmall success facts.

Exact declarations remain authoritative in the exported headers.
