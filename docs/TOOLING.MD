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

The suite covers deterministic compilation, generated-set verification, fallback and fallback-cycle rejection, intentional empty representations, language-display resolution, Type/Field fallback, semantic recovery, stale/corrupt output detection, strict duplicate-key JSON handling, terminal completeness, Platform bundle integrity, schema-inventory duplicate detection, canonical-string authoring enforcement, Domain-root metadata authority, reserved generated-C++ identifiers, canonical build-manifest language metadata, and ContractFamilies with no Type/Field schema inventory.
