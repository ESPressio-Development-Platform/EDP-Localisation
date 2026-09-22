from __future__ import annotations

import argparse
import json
import sys
import tempfile
from pathlib import Path
from typing import Iterable

from .common import ToolError, canonical_json_bytes
from .compiler import (
    compile_generated_set,
    generate_to_directory,
    verify_generated_set,
)
from .edpl import Pack
from .generated import GeneratedContractFamily, Resolution
from .scaffolding import initialise_source_tree


def _path(value: str) -> Path:
    return Path(value).expanduser().resolve()


def _uint(value: str) -> int:
    if not value or not value.isdecimal() or (len(value) > 1 and value[0] == "0"):
        raise argparse.ArgumentTypeError("expected canonical unsigned decimal integer")
    return int(value, 10)


def _common_source_args(parser: argparse.ArgumentParser, *, output: bool) -> None:
    parser.add_argument("--source", required=True, type=_path)
    parser.add_argument("--platform-bundle", required=True, type=_path)
    parser.add_argument("--schema-inventory", action="append", default=[], type=_path)
    if output:
        parser.add_argument("--output", required=True, type=_path)
    parser.add_argument("--cpp-namespace", required=True)


def _emit(value: object, *, json_output: bool) -> None:
    if json_output:
        sys.stdout.buffer.write(canonical_json_bytes(value))
        return
    if isinstance(value, str):
        print(value)
        return
    print(json.dumps(value, ensure_ascii=False, indent=2, sort_keys=True))


def _resolution_human(label: str, result: Resolution) -> str:
    lines = [f"{label}: {result.status}", f"Requested language: {result.requested_language}"]
    if result.supplying_language is not None:
        lines.append(f"Supplying language: {result.supplying_language}")
        lines.append(f"Fallback used: {'yes' if result.fallback_used else 'no'}")
        lines.append(f"Value: {result.value!r}")
    return "\n".join(lines)



def _command_init(args: argparse.Namespace) -> int:
    result = initialise_source_tree(
        args.source,
        args.platform_bundle,
        args.terminal_language,
        args.language,
        args.parent,
        domain_width=args.domain_width,
        subdomain_width=args.subdomain_width,
        string_width=args.string_width,
        schema_inventory=args.schema_inventory,
        type_identifier_bytes=args.type_identifier_bytes,
        field_identifier_bytes=args.field_identifier_bytes,
        schema_identity=args.schema_identity,
        schema_version=args.schema_version,
    )
    _emit(
        {
            "status": "Success",
            **result,
        },
        json_output=args.json,
    )
    return 0


def _command_validate(args: argparse.Namespace) -> int:
    with tempfile.TemporaryDirectory(prefix="edp-localisation-validate-") as temp:
        generated = generate_to_directory(
            args.source,
            args.platform_bundle,
            args.schema_inventory,
            Path(temp) / "generated",
            args.cpp_namespace,
        )
    _emit(
        {
            "status": "Success",
            "contractFamilyFingerprint": generated.fingerprint.runtime_hex,
            "contractFamilyDigestSha256": generated.fingerprint.digest_hex,
        },
        json_output=args.json,
    )
    return 0


def _command_compile(args: argparse.Namespace) -> int:
    generated = compile_generated_set(
        args.source,
        args.platform_bundle,
        args.schema_inventory,
        args.output,
        args.cpp_namespace,
    )
    _emit(
        {
            "status": "Success",
            "output": str(generated.root),
            "contractFamilyFingerprint": generated.fingerprint.runtime_hex,
        },
        json_output=args.json,
    )
    return 0


def _command_verify(args: argparse.Namespace) -> int:
    verify_generated_set(
        args.source,
        args.platform_bundle,
        args.schema_inventory,
        args.generated,
        args.cpp_namespace,
    )
    _emit({"status": "Success"}, json_output=args.json)
    return 0


def _command_resolve(args: argparse.Namespace) -> int:
    family = GeneratedContractFamily(args.generated)
    if args.resolve_kind == "string":
        result = family.resolve_string(args.language, args.domain, args.subdomain, args.string)
        payload = result.as_json()
        human = _resolution_human("String", result)
    elif args.resolve_kind == "language-name":
        result = family.resolve_language_name(args.language, args.target)
        payload = result.as_json()
        human = _resolution_human("Language name", result)
    elif args.resolve_kind == "type":
        name = family.resolve_type_property(args.language, args.type, "name")
        description = family.resolve_type_property(args.language, args.type, "description")
        payload = {
            "status": "Success" if name.status == "Success" else name.status,
            "requestedLanguage": args.language,
            "type": args.type,
            "name": name.as_json(),
            "description": description.as_json(),
        }
        human = _resolution_human("Type Name", name) + "\n\n" + _resolution_human("Type Description", description)
    elif args.resolve_kind == "field":
        name = family.resolve_field_property(args.language, args.type, args.field, "name")
        description = family.resolve_field_property(args.language, args.type, args.field, "description")
        payload = {
            "status": "Success" if name.status == "Success" else name.status,
            "requestedLanguage": args.language,
            "type": args.type,
            "field": args.field,
            "name": name.as_json(),
            "description": description.as_json(),
        }
        human = _resolution_human("Field Name", name) + "\n\n" + _resolution_human("Field Description", description)
    else:
        raise AssertionError(args.resolve_kind)
    _emit(payload if args.json else human, json_output=args.json)
    return 0


def _command_decompile(args: argparse.Namespace) -> int:
    if args.decompile_kind == "pack":
        recovered = Pack.from_path(args.pack).recover_json()
    elif args.decompile_kind == "generated":
        recovered = GeneratedContractFamily(args.generated).decompile_generated()
    else:
        raise AssertionError(args.decompile_kind)
    data = canonical_json_bytes(recovered)
    if args.output is None:
        sys.stdout.buffer.write(data)
    else:
        args.output.parent.mkdir(parents=True, exist_ok=True)
        args.output.write_bytes(data)
    return 0


def build_parser() -> argparse.ArgumentParser:
    parser = argparse.ArgumentParser(prog="edp-localisation")
    parser.add_argument("--json", action="store_true", help="emit machine-readable JSON where supported")
    commands = parser.add_subparsers(dest="command", required=True)

    init = commands.add_parser(
        "init",
        help="create a compileable Localisation application-source skeleton",
    )
    init.add_argument("--source", required=True, type=_path)
    init.add_argument("--platform-bundle", required=True, type=_path)
    init.add_argument("--terminal-language", required=True)
    init.add_argument(
        "--language",
        action="append",
        default=[],
        help="supported non-terminal language; repeat as required",
    )
    init.add_argument(
        "--parent",
        action="append",
        default=[],
        metavar="CHILD=PARENT",
        help="override a non-terminal language parent; otherwise it falls back directly to the terminal language",
    )
    init.add_argument("--domain-width", type=int, choices=(1, 2, 4), default=1)
    init.add_argument("--subdomain-width", type=int, choices=(1, 2, 4), default=1)
    init.add_argument("--string-width", type=int, choices=(1, 2, 4), default=2)
    init.add_argument("--schema-inventory", type=_path)
    init.add_argument("--type-identifier-bytes", type=int, default=8)
    init.add_argument("--field-identifier-bytes", type=int, choices=(1, 2, 4), default=2)
    init.add_argument("--schema-identity", default="Application.Types")
    init.add_argument("--schema-version", default="1")
    init.set_defaults(handler=_command_init)

    validate = commands.add_parser("validate", help="validate the complete Localisation source/build input set")
    _common_source_args(validate, output=False)
    validate.set_defaults(handler=_command_validate)

    compile_parser = commands.add_parser("compile", help="compile one coherent ContractFamily output set")
    _common_source_args(compile_parser, output=True)
    compile_parser.set_defaults(handler=_command_compile)

    verify = commands.add_parser("verify-generated", help="verify generated output freshness and integrity")
    _common_source_args(verify, output=False)
    verify.add_argument("--generated", required=True, type=_path)
    verify.set_defaults(handler=_command_verify)

    resolve = commands.add_parser("resolve", help="resolve values from a coherent generated ContractFamily")
    resolve.add_argument("--generated", required=True, type=_path)
    resolve_sub = resolve.add_subparsers(dest="resolve_kind", required=True)

    string = resolve_sub.add_parser("string")
    string.add_argument("--language", required=True)
    string.add_argument("--domain", required=True, type=_uint)
    string.add_argument("--subdomain", required=True, type=_uint)
    string.add_argument("--string", required=True, type=_uint)

    type_parser = resolve_sub.add_parser("type")
    type_parser.add_argument("--language", required=True)
    type_parser.add_argument("--type", required=True)

    field = resolve_sub.add_parser("field")
    field.add_argument("--language", required=True)
    field.add_argument("--type", required=True)
    field.add_argument("--field", required=True, type=_uint)

    language_name = resolve_sub.add_parser("language-name")
    language_name.add_argument("--language", required=True)
    language_name.add_argument("--target", required=True)
    resolve.set_defaults(handler=_command_resolve)

    decompile = commands.add_parser("decompile", help="recover deterministic semantic JSON from generated packs")
    decompile_sub = decompile.add_subparsers(dest="decompile_kind", required=True)
    pack = decompile_sub.add_parser("pack")
    pack.add_argument("--pack", required=True, type=_path)
    pack.add_argument("--output", type=_path)
    generated = decompile_sub.add_parser("generated")
    generated.add_argument("--generated", required=True, type=_path)
    generated.add_argument("--output", type=_path)
    decompile.set_defaults(handler=_command_decompile)
    return parser


def main(argv: Iterable[str] | None = None) -> int:
    parser = build_parser()
    args = parser.parse_args(list(argv) if argv is not None else None)
    try:
        return args.handler(args)
    except ToolError as exc:
        if getattr(args, "json", False):
            sys.stdout.buffer.write(canonical_json_bytes({"status": "Error", "error": str(exc)}))
        else:
            print(f"ERROR: {exc}", file=sys.stderr)
        return 2


if __name__ == "__main__":
    raise SystemExit(main())
