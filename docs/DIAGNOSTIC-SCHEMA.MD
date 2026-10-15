# Diagnostic schema — staged EDP-Logging prerequisite

**Branch:** `feature/logging-v1` — **PARTIAL TRANCHE I, NOT CONSUMABLE as complete logging interpolation**. Owning repository: EDP-Localisation. Parent: EDP-Logging#1; tracker EDP-Localisation#20. Package version remains `0.1.0`.

## Scope

The existing strict `strings_catalogue.json` ordinary String may optionally declare ordered `parameters`; omission preserves the previous nonparameterised String model and exact legacy ContractFamily source fingerprint bytes. Each parameter has zero-based dense numeric `index`, canonical `type`, explicit lifecycle `status`, and symbolic `symbol` (required on active declarations). Strings in Platform Domain 0 and Application domains use the same validator. Active Strings reject retired parameters. Duplicate symbols, index gaps and unknown types are errors. See [Source Contracts](./SOURCE-CONTRACTS.MD).

Current parser recognises: `bool`, signed/unsigned `i8/i16/i32/i64/u8/u16/u32/u64`, `f32/f64`, bounded `utf8/bytes`, canonical `type-identifier/field-identifier/string-identifier/device-identifier/runtime-incarnation-id/clock-era`. Any custom registered Enum/strong adapter is future work; accepting an unknown canonical argument kind without its reversible semantics would make archives ambiguous. Maximum owned value size is governed by later Logging record/argument capacity, not this parsing function.

## Two distinct fingerprints

`build_fingerprint(model)` is the existing **translation-sensitive ContractFamily** digest: it now includes a tagged canonical parameter section only when parameter declarations exist, to preserve exact previous fingerprints for legacy unparameterised catalogues.

`build_diagnostic_fingerprint(model)` is the independent **128-bit diagnostic identity** (first 16 bytes of SHA-256), with the full SHA-256 digest and canonical byte stream also returned. Domain separation is ASCII `EDP-LOCALISATION-DIAGNOSTIC-SCHEMA-V1` followed by a zero byte, then a version byte (1). Numeric counts/IDs are little-endian unsigned 32-bit, statuses are one-byte canonical 1=active / 2=retired, type names are length-prefixed UTF-8; traverse sorted Domain/SubDomain/String numeric identities and ordered parameter positions. Manifest identifier widths are recorded in one byte each before catalogue traversal. The diagnostic digest includes **neither language nor translation text, symbolic name, nor Bootstrap-selected Logging sequence width**.

The digest is a schema identity, **not cryptographic authentication** of a captured log and not a language-pack identity. It may change on governed message/argument identity/type/lifecycle evolution; language-only changes do not affect it.

## Current limits and the next steps

This checkpoint contains parsing + immutable semantic model + digest helper, a generated 16-byte C++ digest constant, a deterministic diagnostic-schema.json identity/argument catalogue, build-manifest linkage, and **unexecuted** unit tests. It does **not** yet generate a C++20 per-message arity/type validator, EDPL on-device parameter presentations, language-specific Name/Description, placeholder verification, durable historical schema distribution policy or a public CLI command. These must be added with new tests, production docs and Wiki coverage before EDP-Logging consumes the feature. Existing `0.1.0` package version must not mutate.

**Validation evidence:** reviewed GitHub source and published commit; no authorized execution appliance, therefore no Python suite, compiler test, HIL or physical test run. This is not a validated release.

## Generated diagnostic artefacts (initial release-coupled format)

- `GeneratedLocalisationContract.hpp` now publishes `GeneratedLocalisationContract::DiagnosticSchemaFingerprint` as exactly 16 bytes, independently of `ContractFamilyFingerprint`.
- `diagnostic-schema.json` is a canonical UTF-8 generated file with `schemaVersion=1`, `kind=edp-diagnostic-schema`, 32-uppercase-hex fingerprint, full SHA-256 digest, Domain/SubDomain/String identifier widths, and sorted `messages` with governed identities, lifecycle state and typed `parameters` in declaration order. It does **not** contain localisation text or mutable symbols.
- `localisation-build-manifest.json` now includes both diagnostic digest fields and lists the new output with `kind=diagnostic-schema` and its own SHA-256. The normal generated-set verifier checks output hash, file identity and consistency of embedded manifest fingerprint/digest.

**Compatibility note:** newly generated contract families require this additional file and metadata; existing generated sets produced by earlier tool snapshots do not contain it and must be regenerated from their pinned authoring sources for the new toolchain. If the project requires support for older deployed generated-set layouts without regeneration, that is a **V2/LEGACY decision gate**, not implied compatibility.

**Validation:** the schema-generation regression test is committed but **NOT EXECUTED** on the unavailable approved appliances. No claim is made that this generates the still-missing runtime argument formatter.

## Diagnostic positional placeholder validation (implemented in the Python semantic loader)

A localisation representation for a String with a declared non-empty parameter schema is checked as the `SemanticModel` loads, for **every present language**. The initial grammar accepts only `{0}`, `{1}` etc. referencing declared zero-based indexes, allows translated ordering/repetition/omission, and accepts `{{` and `}}` as literal braces. A malformed/unmatched brace, empty index, non-ASCII digit, leading-zero index, out-of-range index or inline formatting directive is a deterministic `ToolError`. No type formatting is performed at validation time.

Ordinary Strings *without* a parameter declaration remain unchanged: braces have their existing literal meaning. The check is purely at Python authoring/compilation time; **embedded runtime interpolation and per-parameter presentation remain unimplemented**.

The parser rejects an explicit JSON null for the optional `parameters` property; omit it entirely or supply a JSON array. Long decimal placeholder tokens are rejected before integer conversion to guarantee a controlled failure rather than interpreter-dependent oversized-number exceptions. These invariants are covered by newly authored (but unexecuted) unit tests.

## Generated C++20 positional argument descriptors — Tranche I continuation

`GeneratedLocalisationContract.hpp` now includes a minimal, allocation-free `DiagnosticParameterKind` vocabulary and numeric-identity keyed `DiagnosticMessageSchema<Domain,SubDomain,String>` specialisations for every **active** ordinary String, including valid zero-argument declarations. Each specialisation contains its compile-time `Declared` status, `ArgumentCount` and `std::array` of declared canonical types in positional order. Unknown numeric identities use the generic template with `Declared = false`.

`DiagnosticSignatureMatches<Domain,SubDomain,String, Kind...>()` is `consteval`; it returns false for undeclared IDs, mismatched arity or mismatched types. Once Logging supplies correctly scoped C++ type-to-kind traits, enabled instrumentation can `static_assert` the result. This helper deliberately does **not** yet claim that Logging's complete C++ type adapters are implemented. Only non-retired parameter declarations on active messages are emitted as instrumentable schemas; retired definitions remain in the history-facing `diagnostic-schema.json`.

This generated contract adds only standard C++20 library types (`array`, `cstddef`, `cstdint`) and no EDP-Mesh or platform dependencies, heap allocations or runtime tables. The new generator unit test is written but **not executed**; C++ compilation on the target/toolchain and actual compile-fail tests remain required before use.

## Explicit zero-argument signatures versus ordinary Strings

The `parameters` property is an **explicit diagnostic opt-in**, not an implicit empty list on every ordinary String. Omitting it entirely creates a normal String that **cannot be used as a declared diagnostic message**. Writing `"parameters": []` creates a deliberate diagnostic String with exactly zero arguments. These are different governance contracts even though each has no parameter values.

The model retains `CatalogueString.diagnostic_declared`, generated JSON retains `diagnosticDeclared`, and the translation-independent fingerprint encodes this bit per String. Only active Strings with an explicit signature generate an instrumentable `DiagnosticMessageSchema<...>` specialisation; undeclared Strings use the generic `Declared=false` template. Explicit zero-argument templates still undergo brace/placeholder validation (e.g., `{0}` is forbidden).

This correction prevents ordinary strings from silently becoming zero-argument Logging messages. Both semantic and ContractFamily fingerprints now change when an explicitly empty diagnostic signature is added, but older unparameterised catalogues still produce the original ContractFamily canonical transcript.
