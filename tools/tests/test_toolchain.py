from __future__ import annotations

import copy
import json
import tempfile
import unittest
from pathlib import Path

from edp_localisation.common import ToolError, canonical_bundle_digest, canonical_json_bytes
from edp_localisation.compiler import compile_generated_set, verify_generated_set
from edp_localisation.generated import GeneratedContractFamily


def write_json(path: Path, value: object) -> None:
    path.parent.mkdir(parents=True, exist_ok=True)
    path.write_bytes(canonical_json_bytes(value))


class ToolchainTests(unittest.TestCase):

    def make_fixture(self, root: Path) -> tuple[Path, Path, Path]:
        source = root / "source"
        platform = root / "platform"
        schema = root / "schema.json"

        write_json(
            platform / "platform-localisation-manifest.json",
            {
                "schemaVersion": 1,
                "identity": "ESPressio-Platform-Localisation",
                "version": "1.0.0",
            },
        )
        write_json(
            platform / "strings_catalogue.json",
            {
                "schemaVersion": 1,
                "domains": [
                    {
                        "id": 0,
                        "symbol": "Platform",
                        "status": "active",
                        "subDomains": [
                            {
                                "id": 0,
                                "symbol": "Root",
                                "status": "active",
                                "strings": [],
                            }
                        ],
                    }
                ],
            },
        )
        for language, values in {
            "en-GB": {
                "schemaVersion": 1,
                "domains": {
                    "0": {
                        "subDomains": {
                            "0": {"strings": {"0": "ESPressio"}}
                        }
                    }
                },
            },
            "de": {
                "schemaVersion": 1,
                "domains": {},
            },
        }.items():
            write_json(platform / language / "strings.json", values)
            write_json(platform / language / "type_schema.json", {"schemaVersion": 1, "types": {}})

        integrity = canonical_bundle_digest(platform)

        write_json(
            source / "manifest.json",
            {
                "schemaVersion": 1,
                "terminalLanguage": "en-GB",
                "supportedLanguages": ["de", "en-GB"],
                "identifierWidths": {
                    "domain": 1,
                    "subDomain": 1,
                    "string": 2,
                },
                "platformLocalisation": {
                    "identity": "ESPressio-Platform-Localisation",
                    "version": "1.0.0",
                    "integrity": f"sha256-{integrity}",
                },
            },
        )
        write_json(
            source / "strings_catalogue.json",
            {
                "schemaVersion": 1,
                "domains": [
                    {
                        "id": 1,
                        "symbol": "Application",
                        "status": "active",
                        "subDomains": [
                            {
                                "id": 0,
                                "symbol": "Root",
                                "status": "active",
                                "strings": [
                                    {
                                        "id": 3,
                                        "symbol": "Greeting",
                                        "status": "active",
                                        "localisation": "translatable",
                                    },
                                    {
                                        "id": 4,
                                        "symbol": "CanonicalEmpty",
                                        "status": "active",
                                        "localisation": "canonical",
                                    },
                                    {
                                        "id": 5,
                                        "symbol": "GermanText",
                                        "status": "active",
                                        "localisation": "translatable",
                                    },
                                ],
                            }
                        ],
                    }
                ],
            },
        )

        write_json(
            schema,
            {
                "schemaVersion": 1,
                "typeIdentifierBytes": 8,
                "fieldIdentifierBytes": 2,
                "provenance": {"identity": "Fixture.Schema", "version": "1"},
                "types": {
                    "0x0123456789ABCDEF": {
                        "status": "active",
                        "presentationExposed": True,
                        "symbol": "TemperatureReading",
                        "fields": {
                            "0": {
                                "status": "active",
                                "presentationExposed": True,
                                "symbol": "Temperature",
                            }
                        },
                    }
                },
            },
        )

        write_json(
            source / "en-GB" / "language.json",
            {
                "schemaVersion": 1,
                "language": "en-GB",
                "parent": None,
                "selfName": "British English",
                "displayNames": {"de": "German"},
            },
        )
        write_json(
            source / "en-GB" / "strings.json",
            {
                "schemaVersion": 1,
                "domains": {
                    "1": {
                        "subDomains": {
                            "0": {
                                "strings": {
                                    "0": "Application",
                                    "3": "Hello",
                                    "4": "",
                                    "5": "Fallback text",
                                }
                            }
                        }
                    }
                },
            },
        )
        write_json(
            source / "en-GB" / "type_schema.json",
            {
                "schemaVersion": 1,
                "types": {
                    "0x0123456789ABCDEF": {
                        "name": "Temperature Reading",
                        "description": "A temperature observation.",
                        "fields": {
                            "0": {
                                "name": "Temperature",
                                "description": "Degrees Celsius",
                            }
                        },
                    }
                },
            },
        )

        write_json(
            source / "de" / "language.json",
            {
                "schemaVersion": 1,
                "language": "de",
                "parent": "en-GB",
                "selfName": "Deutsch",
                "displayNames": {"en-GB": "Britisches Englisch"},
            },
        )
        write_json(
            source / "de" / "strings.json",
            {
                "schemaVersion": 1,
                "domains": {
                    "1": {
                        "subDomains": {
                            "0": {
                                "strings": {
                                    "0": "Anwendung",
                                    "5": "Grüße",
                                }
                            }
                        }
                    }
                },
            },
        )
        write_json(
            source / "de" / "type_schema.json",
            {"schemaVersion": 1, "types": {}},
        )

        return source, platform, schema

    def test_compile_is_deterministic_and_resolution_matches_fallback_semantics(self) -> None:
        with tempfile.TemporaryDirectory() as raw:
            root = Path(raw)
            source, platform, schema = self.make_fixture(root)
            first = root / "generated-one"
            second = root / "generated-two"

            compile_generated_set(
                source, platform, [schema], first, "Fixture::Localisation"
            )
            compile_generated_set(
                source, platform, [schema], second, "Fixture::Localisation"
            )

            first_files = {
                path.relative_to(first).as_posix(): path.read_bytes()
                for path in first.rglob("*") if path.is_file()
            }
            second_files = {
                path.relative_to(second).as_posix(): path.read_bytes()
                for path in second.rglob("*") if path.is_file()
            }
            self.assertEqual(first_files, second_files)

            verify_generated_set(
                source, platform, [schema], first, "Fixture::Localisation"
            )

            family = GeneratedContractFamily(first)
            greeting = family.resolve_string("de", 1, 0, 3)
            self.assertEqual(greeting.status, "Success")
            self.assertEqual(greeting.value, "Hello")
            self.assertEqual(greeting.supplying_language, "en-GB")
            self.assertTrue(greeting.fallback_used)

            explicit_empty = family.resolve_string("de", 1, 0, 4)
            self.assertEqual(explicit_empty.status, "Success")
            self.assertEqual(explicit_empty.value, "")
            self.assertEqual(explicit_empty.supplying_language, "en-GB")

            german = family.resolve_string("de", 1, 0, 5)
            self.assertEqual(german.value, "Grüße")
            self.assertEqual(german.supplying_language, "de")
            self.assertFalse(german.fallback_used)

            language_name = family.resolve_language_name("de", "en-GB")
            self.assertEqual(language_name.value, "Britisches Englisch")
            self.assertEqual(language_name.supplying_language, "de")

            type_name = family.resolve_type_property(
                "de", "0x0123456789ABCDEF", "name"
            )
            self.assertEqual(type_name.value, "Temperature Reading")
            self.assertEqual(type_name.supplying_language, "en-GB")
            self.assertTrue(type_name.fallback_used)

            field_name = family.resolve_field_property(
                "de", "0x0123456789ABCDEF", 0, "name"
            )
            self.assertEqual(field_name.value, "Temperature")
            self.assertEqual(field_name.supplying_language, "en-GB")

            recovered = family.decompile_generated()
            self.assertEqual(
                recovered["contract"]["terminalLanguage"],
                "en-GB",
            )
            self.assertEqual(
                recovered["languages"]["de"]["language"]["parent"],
                "en-GB",
            )

    def test_verify_generated_detects_stale_output(self) -> None:
        with tempfile.TemporaryDirectory() as raw:
            root = Path(raw)
            source, platform, schema = self.make_fixture(root)
            generated = root / "generated"
            compile_generated_set(
                source, platform, [schema], generated, "Fixture::Localisation"
            )

            strings_path = source / "en-GB" / "strings.json"
            data = json.loads(strings_path.read_text("utf-8"))
            data["domains"]["1"]["subDomains"]["0"]["strings"]["3"] = "Changed"
            write_json(strings_path, data)

            with self.assertRaises(ToolError):
                verify_generated_set(
                    source, platform, [schema], generated, "Fixture::Localisation"
                )

    def test_active_domain_requires_root_namespace(self) -> None:
        with tempfile.TemporaryDirectory() as raw:
            root = Path(raw)
            source, platform, schema = self.make_fixture(root)

            path = source / "strings_catalogue.json"
            data = json.loads(path.read_text("utf-8"))
            data["domains"][0]["subDomains"] = []
            write_json(path, data)

            with self.assertRaises(ToolError):
                compile_generated_set(
                    source,
                    platform,
                    [schema],
                    root / "generated",
                    "Fixture::Localisation",
                )

    def test_domain_root_metadata_policy_has_single_authority(self) -> None:
        with tempfile.TemporaryDirectory() as raw:
            root = Path(raw)
            source, platform, schema = self.make_fixture(root)

            path = source / "strings_catalogue.json"
            data = json.loads(path.read_text("utf-8"))
            data["domains"][0]["subDomains"][0]["metadata"] = {
                "name": "canonical"
            }
            write_json(path, data)

            with self.assertRaises(ToolError):
                compile_generated_set(
                    source,
                    platform,
                    [schema],
                    root / "generated",
                    "Fixture::Localisation",
                )

    def test_platform_integrity_mismatch_is_rejected(self) -> None:
        with tempfile.TemporaryDirectory() as raw:
            root = Path(raw)
            source, platform, schema = self.make_fixture(root)

            path = platform / "en-GB" / "strings.json"
            data = json.loads(path.read_text("utf-8"))
            data["domains"]["0"]["subDomains"]["0"]["strings"]["0"] = "Changed"
            write_json(path, data)

            with self.assertRaises(ToolError):
                compile_generated_set(
                    source,
                    platform,
                    [schema],
                    root / "generated",
                    "Fixture::Localisation",
                )

    def test_generated_corruption_is_rejected(self) -> None:
        with tempfile.TemporaryDirectory() as raw:
            root = Path(raw)
            source, platform, schema = self.make_fixture(root)
            generated = root / "generated"

            compile_generated_set(
                source,
                platform,
                [schema],
                generated,
                "Fixture::Localisation",
            )

            pack = generated / "packs" / "en-GB.edploc"
            data = bytearray(pack.read_bytes())
            data[-1] ^= 0x01
            pack.write_bytes(data)

            with self.assertRaises(ToolError):
                verify_generated_set(
                    source,
                    platform,
                    [schema],
                    generated,
                    "Fixture::Localisation",
                )

    def test_duplicate_type_across_schema_inventories_is_rejected(self) -> None:
        with tempfile.TemporaryDirectory() as raw:
            root = Path(raw)
            source, platform, schema = self.make_fixture(root)
            duplicate = root / "schema-duplicate.json"
            duplicate.write_bytes(schema.read_bytes())

            with self.assertRaises(ToolError):
                compile_generated_set(
                    source,
                    platform,
                    [schema, duplicate],
                    root / "generated",
                    "Fixture::Localisation",
                )

    def test_cpp_namespace_reserved_word_is_rejected(self) -> None:
        with tempfile.TemporaryDirectory() as raw:
            root = Path(raw)
            source, platform, schema = self.make_fixture(root)

            with self.assertRaises(ToolError):
                compile_generated_set(
                    source,
                    platform,
                    [schema],
                    root / "generated",
                    "Fixture::class",
                )

    def test_generated_identifier_collision_is_rejected(self) -> None:
        with tempfile.TemporaryDirectory() as raw:
            root = Path(raw)
            source, platform, schema = self.make_fixture(root)

            path = source / "strings_catalogue.json"
            data = json.loads(path.read_text("utf-8"))
            data["domains"][0]["subDomains"][0]["strings"][0]["symbol"] = "Name"
            write_json(path, data)

            with self.assertRaises(ToolError):
                compile_generated_set(
                    source,
                    platform,
                    [schema],
                    root / "generated",
                    "Fixture::Localisation",
                )

    def test_noncanonical_language_in_build_manifest_is_rejected(self) -> None:
        with tempfile.TemporaryDirectory() as raw:
            root = Path(raw)
            source, platform, schema = self.make_fixture(root)
            generated = root / "generated"

            compile_generated_set(
                source,
                platform,
                [schema],
                generated,
                "Fixture::Localisation",
            )

            manifest_path = generated / "localisation-build-manifest.json"
            manifest = json.loads(manifest_path.read_text("utf-8"))
            manifest["supportedLanguages"][0] = "DE"
            write_json(manifest_path, manifest)

            with self.assertRaises(ToolError):
                verify_generated_set(
                    source,
                    platform,
                    [schema],
                    generated,
                    "Fixture::Localisation",
                )

    def test_contract_family_without_schema_inventory_compiles(self) -> None:
        with tempfile.TemporaryDirectory() as raw:
            root = Path(raw)
            source, platform, _schema = self.make_fixture(root)

            for language in ("de", "en-GB"):
                write_json(
                    source / language / "type_schema.json",
                    {
                        "schemaVersion": 1,
                        "types": {},
                    },
                )

            generated = root / "generated"
            compile_generated_set(
                source,
                platform,
                [],
                generated,
                "Fixture::Localisation",
            )

            contract = (
                generated / "GeneratedLocalisationContract.hpp"
            ).read_text("utf-8")

            self.assertIn(
                "TypeIdentifierBytes = 0U",
                contract,
            )
            self.assertIn(
                "FieldIdentifierBytes = 0U",
                contract,
            )

            verify_generated_set(
                source,
                platform,
                [],
                generated,
                "Fixture::Localisation",
            )

    def test_duplicate_json_object_key_is_rejected(self) -> None:
        with tempfile.TemporaryDirectory() as raw:
            root = Path(raw)
            source, platform, schema = self.make_fixture(root)

            manifest_path = source / "manifest.json"
            text = manifest_path.read_text("utf-8")
            text = text.replace(
                '"schemaVersion":1',
                '"schemaVersion":1,"schemaVersion":1',
                1,
            )
            manifest_path.write_text(
                text,
                encoding="utf-8",
            )

            with self.assertRaises(ToolError):
                compile_generated_set(
                    source,
                    platform,
                    [schema],
                    root / "generated",
                    "Fixture::Localisation",
                )

    def test_terminal_general_string_completeness_is_enforced(self) -> None:
        with tempfile.TemporaryDirectory() as raw:
            root = Path(raw)
            source, platform, schema = self.make_fixture(root)

            strings_path = source / "en-GB" / "strings.json"
            data = json.loads(strings_path.read_text("utf-8"))
            del data["domains"]["1"]["subDomains"]["0"]["strings"]["3"]
            write_json(strings_path, data)

            with self.assertRaises(ToolError):
                compile_generated_set(
                    source,
                    platform,
                    [schema],
                    root / "generated",
                    "Fixture::Localisation",
                )

    def test_non_terminal_canonical_override_is_rejected(self) -> None:
        with tempfile.TemporaryDirectory() as raw:
            root = Path(raw)
            source, platform, schema = self.make_fixture(root)

            path = source / "de" / "strings.json"
            data = json.loads(path.read_text("utf-8"))
            data["domains"]["1"]["subDomains"]["0"]["strings"]["4"] = "not allowed"
            write_json(path, data)

            with self.assertRaises(ToolError):
                compile_generated_set(
                    source,
                    platform,
                    [schema],
                    root / "generated",
                    "Fixture::Localisation",
                )


if __name__ == "__main__":
    unittest.main()
