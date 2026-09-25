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

> Field-schema tooling audit refreshed through implementation tip `6ee7f3434139a25c32a682fe4f5e10e211405803`.

## Fixed Type/Field identity contract

The toolchain accepts no author-selectable Type or Field identifier width.

- `TYPE_IDENTIFIER_BYTES = 8`;
- `FIELD_IDENTIFIER_BYTES = 1`;
- Type keys contain exactly 16 uppercase hexadecimal digits after `0x`, with non-zero Authority/local components;
- Field keys are canonical decimal integers in `0..255`;
- `edp-localisation init` exposes no Type-width or Field-width option;
- generated non-empty ContractFamilies advertise `8/1`; no-schema ContractFamilies advertise `0/0`;
- EDPL keeps the width bytes only as structural self-description and validates those fixed values.

This keeps generated schema interoperable with the universal EDP-System Type/Field identity contract.
