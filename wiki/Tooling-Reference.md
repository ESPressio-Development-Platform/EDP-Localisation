# Tooling Reference

The Localisation toolchain is part of the maintained repository contract. This index covers every Python module under `tools/`, including the validation test module.

| Module | Classification | Reference |
|---|---|---|
| `tools/edp_localisation/__init__.py` | TOOLING | [open](Tooling-edp-localisation-init-) |
| `tools/edp_localisation/__main__.py` | TOOLING | [open](Tooling-edp-localisation-main-) |
| `tools/edp_localisation/catalogue.py` | TOOLING | [open](Tooling-edp-localisation-catalogue) |
| `tools/edp_localisation/cli.py` | TOOLING | [open](Tooling-edp-localisation-cli) |
| `tools/edp_localisation/common.py` | TOOLING | [open](Tooling-edp-localisation-common) |
| `tools/edp_localisation/compiler.py` | TOOLING | [open](Tooling-edp-localisation-compiler) |
| `tools/edp_localisation/edpl_format.py` | TOOLING | [open](Tooling-edp-localisation-edpl-format) |
| `tools/edp_localisation/edpl_parser.py` | TOOLING | [open](Tooling-edp-localisation-edpl-parser) |
| `tools/edp_localisation/edpl_writer.py` | TOOLING | [open](Tooling-edp-localisation-edpl-writer) |
| `tools/edp_localisation/edpl.py` | TOOLING | [open](Tooling-edp-localisation-edpl) |
| `tools/edp_localisation/fingerprint.py` | TOOLING | [open](Tooling-edp-localisation-fingerprint) |
| `tools/edp_localisation/generated.py` | TOOLING | [open](Tooling-edp-localisation-generated) |
| `tools/edp_localisation/generator.py` | TOOLING | [open](Tooling-edp-localisation-generator) |
| `tools/edp_localisation/model_types.py` | TOOLING | [open](Tooling-edp-localisation-model-types) |
| `tools/edp_localisation/model.py` | TOOLING | [open](Tooling-edp-localisation-model) |
| `tools/edp_localisation/presentation.py` | TOOLING | [open](Tooling-edp-localisation-presentation) |
| `tools/edp_localisation/scaffolding.py` | TOOLING | [open](Tooling-edp-localisation-scaffolding) |
| `tools/edp_localisation/verification.py` | TOOLING | [open](Tooling-edp-localisation-verification) |
| `tools/tests/test_toolchain.py` | TEST | [open](Tooling-tests-test-toolchain) |

## CLI surface

The supported user-facing command groups are implemented by `tools/edp_localisation/cli.py` and include project initialization, source validation, deterministic compilation, generated-output verification, resolution inspection and decompilation. The module reference records the exact parser/dispatch functions at the audited baseline.

> Primitive-introduction Type-identity baseline: `9a0ca6321eaaf9940bfb0d84556e438cca5b7e55`.

## Fixed Type identity contract

The toolchain no longer accepts or generates an author-selectable Type identifier width. `TYPE_IDENTIFIER_BYTES` is fixed at 8, schema inventory Type keys must contain exactly 16 hexadecimal digits after `0x`, the first 24 bits must contain a non-zero Type Authority, and the remaining 40 bits must contain a non-zero authority-local Type value. `edp-localisation init` exposes only the Type-local Field-width option. This keeps generated schema interoperable with the universal EDP-System Type identity contract.
