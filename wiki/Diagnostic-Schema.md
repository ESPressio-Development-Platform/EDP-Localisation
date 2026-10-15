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
