# EDP-Localisation Tooling

## Scope

The Python toolchain in `tools/edp_localisation` is release-coupled to EDP-Localisation. Version `0.1.0` of the toolchain emits the exact EDPL format consumed by version `0.1.0` of the runtime library.

The runtime never invokes Python and never parses authoring JSON.

## Installation

The tooling can be used directly from a checkout:

```bash
PYTHONPATH=tools python3 -m edp_localisation --help
```

or installed from the repository's `tools` directory:

```bash
python3 -m pip install ./tools
edp-localisation --help
```

## Source layout

A source root contains:

```text
manifest.json
strings_catalogue.json
<language>/
    language.json
    strings.json
    type_schema.json
```

Every language declared by `manifest.json` must contain all three per-language files.

The compiler additionally consumes:

- one already-resolved immutable Platform Localisation Source Bundle;
- zero or more schema-inventory JSON artefacts;
- an output directory;
- a generated C++ namespace.

Dependency fetching is intentionally outside the compiler.

## Commands

### init

Create a compileable Application Localisation source skeleton from an already-resolved Platform Localisation Source Bundle:

```bash
edp-localisation init \
    --source localisation \
    --platform-bundle dependencies/platform-localisation \
    --terminal-language en-GB \
    --language de \
    --language de-AT \
    --parent de-AT=de \
    --schema-inventory localisation/schema-inventory.json \
    --schema-identity MyApplication.Types
```

The terminal language is always included. Each repeated `--language` adds a supported non-terminal language. Unless overridden by `--parent CHILD=PARENT`, a non-terminal language falls back directly to the terminal language.

The command:

- validates canonical BCP47 language identities;
- derives and pins Platform bundle identity, version and canonical SHA-256 integrity from the supplied bundle;
- creates `manifest.json` and `strings_catalogue.json`;
- creates `language.json`, `strings.json` and `type_schema.json` for every supported language;
- creates an optional empty schema inventory when `--schema-inventory` is supplied; Type identities are implicitly fixed at the platform-wide 64-bit width and Type-local Field identities at the platform-wide 8-bit width;
- refuses to overwrite a non-empty source tree or existing schema inventory.

The generated source tree is intentionally minimal but compileable. Its human-readable placeholder language names and empty catalogues are expected to be edited by the application author.


### validate

```bash
edp-localisation validate \
    --source localisation \
    --platform-bundle dependencies/platform-localisation \
    --schema-inventory generated/schema.json \
    --cpp-namespace MyApplication::Localisation
```

Validation consumes the complete ContractFamily source/build input set and publishes no deployment artefacts.

### compile

```bash
edp-localisation compile \
    --source localisation \
    --platform-bundle dependencies/platform-localisation \
    --schema-inventory generated/schema.json \
    --output generated/localisation \
    --cpp-namespace MyApplication::Localisation
```

Compilation is all-or-nothing. The complete generated set is staged and self-validated before publication.

The generated set contains:

- `GeneratedLocalisationContract.hpp`;
- `GeneratedLocalisationIdentifiers.hpp`;
- one `packs/<language>.edploc` per supported language;
- `localisation-build-manifest.json`.

`GeneratedLocalisationIdentifiers.hpp` exposes strongly typed symbolic bindings for active Application general strings. When schema inventories provide symbols, it also exposes Type/Field presentation identifiers under `LocalisationIdentifiers::Types`, so consuming code does not need to manufacture raw TypeIdentifier bytes or numeric FieldIdentifier values. Every generated Type binding constructs the universal `EDP-System::TypeIdentifier` from the same fixed eight canonical bytes required for compatibility across ESPressio libraries, application code, and third-party ESPressio-compatible libraries. Every generated Field presentation binding uses the universal one-byte `EDP-System::FieldIdentifier`; there is no Field-width CLI or schema option. Authoring/tooling validation also rejects a zero 24-bit Type Authority or zero 40-bit authority-local Type value.

### verify-generated

```bash
edp-localisation verify-generated \
    --source localisation \
    --platform-bundle dependencies/platform-localisation \
    --schema-inventory generated/schema.json \
    --generated generated/localisation \
    --cpp-namespace MyApplication::Localisation
```

Verification checks exact output digests and pack metadata, regenerates into temporary storage, and compares byte-for-byte. It therefore detects both corruption and stale-but-internally-consistent generated output.

### resolve

The generated-set resolver reproduces runtime explicit-parent fallback semantics:

```bash
edp-localisation resolve \
    --generated generated/localisation \
    string --language de --domain 1 --subdomain 0 --string 3
```

Other mandatory V1 surfaces are:

```text
resolve type
resolve field
resolve language-name
```

Name and Description are resolved independently and may report different supplying languages.

### decompile

Recover provable semantic content from one pack:

```bash
edp-localisation decompile pack \
    --pack generated/localisation/packs/en-GB.edploc
```

or from a coherent generated ContractFamily:

```bash
edp-localisation decompile generated \
    --generated generated/localisation
```

Decompiler output is deterministic semantic recovery. It does not invent source-only symbols, comments, provenance, paths, retired catalogue history, or other facts that are absent from runtime packs.

## Determinism

Identical semantic inputs produce byte-identical generated outputs.

The semantic ContractFamily digest is SHA-256 over a versioned canonical semantic stream. The first 16 raw digest bytes form the runtime `ContractFamilyFingerprint`; the full SHA-256 is retained in the tooling build manifest.

Generated semantic artefacts contain no wall-clock timestamps, random IDs, hostnames, absolute source paths, or filesystem-order-dependent content.

## Toolchain tests

Run the self-contained Python regression suite with:

```bash
PYTHONPATH=tools python3 -m unittest discover -s tools/tests -v
```

The suite covers source-tree scaffolding and parent-graph creation, fixed 64-bit Type-identity and fixed 8-bit Field-identity enforcement, deterministic compilation, generated-set verification, generated String/Type/Field symbolic bindings, fallback and fallback-cycle rejection, intentional empty representations, language-display resolution, Type/Field fallback, semantic recovery, stale/corrupt output detection, strict duplicate-key JSON handling, terminal completeness, Platform bundle integrity, schema-inventory duplicate detection, canonical-string authoring enforcement, Domain-root metadata authority, reserved generated-C++ identifiers, canonical build-manifest language metadata, and ContractFamilies with no Type/Field schema inventory.
