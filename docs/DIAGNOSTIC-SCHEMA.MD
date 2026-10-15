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

This checkpoint contains parsing + immutable semantic model + digest helper + unexecuted unit tests. It does **not** yet generate the C++20 per-message arity/type descriptor, EDPL on-device parameter presentations, language-specific Name/Description, placeholder grammar verification, immutable historical schema bundle, decoder artefact, or a public CLI command. These must be added with new tests, production docs and Wiki coverage before EDP-Logging consumes the feature. Existing `0.1.0` package version must not mutate.

**Validation evidence:** reviewed GitHub source and published commit; no authorized execution appliance, therefore no Python suite, compiler test, HIL or physical test run. This is not a validated release.
