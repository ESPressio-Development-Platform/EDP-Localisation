# Diagnostic Schema — Tranche I in progress

**Classification:** INTERNAL TOOLING / FUTURE GENERATED PUBLIC CONTRACT

[Durable source contract](../docs/DIAGNOSTIC-SCHEMA.MD) — governed optional parameter declaration and independent historical diagnostic fingerprint. Tooling implementation originates in `catalogue.py`, `model_types.py` and `fingerprint.py`; active source ref `feature/logging-v1`.

The governed parameter model contains ordered zero-based stable indexes, explicit active/retired lifecycle, canonical type name and optional symbolic authoring name. When a catalogue String is active, all parameter entries are active. A missing `parameters` field means a nonparameterised String, preserving old catalogue structure.

The independent SHA-256 domain-separated `build_diagnostic_fingerprint` excludes language, translation text, parameter symbol and runtime Logging sequence width; it includes sorted governed message identities, lifecycle and parameter index/type/status. A separately retained ContractFamilyFingerprint still changes when translated strings change; these two fingerprints must never be conflated.

**No supported Logging runtime interpretation is implemented here yet.** This toolchain can parse/fingerprint declarations but cannot yet emit usable generated C++ argument validators, EDPL parameter sections or Python historical schema bundles. The committed tests are unexecuted on unavailable authorised appliances.

Related references: [Catalogue](Tooling-edp-localisation-catalogue), [Model Types](Tooling-edp-localisation-model-types), [Fingerprint](Tooling-edp-localisation-fingerprint), [Tests](Tooling-tests-test-toolchain).

## Generated host/C++ schema identity

The new diagnostic fingerprint is emitted in three correlated generated surfaces: the C++ `GeneratedLocalisationContract::DiagnosticSchemaFingerprint`, the build-manifest identity fields, and the canonical `diagnostic-schema.json` describing sorted governed messages and typed parameter positions. Generated-set verification requires all three to remain consistent. Existing EDPL language packs are unchanged as to parameter interpolation; no LogRecord formatter is implemented in this tranche.

## Authoring-time placeholders

`_validate_diagnostic_placeholders` now validates `{N}` positional references and escaped literal braces for every present language's parameterised String text during SemanticModel loading. It rejects invalid indexes and syntax but does not yet implement rendering, C++ compile-time message-specific argument enforcement or translated parameter labels. This is an additive Python-source prerequisite only.

## C++20 compile-time signature schema

The generated Contract now provides `DiagnosticParameterKind`, `DiagnosticMessageSchema<Domain,SubDomain,String>` and `DiagnosticSignatureMatches<Domain,SubDomain,String, Kinds...>()`. The template is marked `Declared = false` for unknown identifiers, while active String specialisations retain their exact positional argument kinds, including zero-argument messages. The kind comparator is `consteval` and produces a compile-time boolean only.

Platform and Application catalogue String IDs share this schema vocabulary and no concrete device provider or Logging implementation is imported. This permits future Logging instrumentation to assert arity/kind using its type traits without coupling generated headers to an application. The additional C++ code has **not** been compiled or executed; tests and demos are outstanding.

## Canonical source/host text-template parsing

`parse_diagnostic_template` now supplies one strict grammar for authoring validation and later host presentation: `{N}` reference is positional, `{{` / `}}` are literal braces. Only declared diagnostic messages use this grammar. `render_diagnostic_template` substitutes *preformatted textual* arguments into parsed pieces under a caller-bounded output byte limit. It deliberately performs no numeric type coercion and is not a replacement for the schema-owned argument formatter. Tests for escaped braces, mixed positional ordering, malformed tokens, Unicode and bounded output are written but not executed.


## Generated identity is self-describing

For active messages the descriptor's `SourceDomain`,
`SourceSubDomain`, and `SourceString` are constants from the
authoritative Localisation numeric catalogue, avoiding a redundant
caller-selected message identity when Logging binds a typed record.
Undeclared primary templates expose their numeric arguments but do
not qualify as valid diagnostic call sites. The historical schema
fingerprint and existing EDPL representations are unchanged.
