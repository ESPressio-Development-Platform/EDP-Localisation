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
