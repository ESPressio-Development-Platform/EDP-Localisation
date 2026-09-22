from __future__ import annotations

import hashlib
import json
import re
from dataclasses import dataclass
from pathlib import Path
from typing import Any, Iterable

TOOLCHAIN_IDENTITY = "EDP-Localisation"
TOOLCHAIN_VERSION = "0.1.0"
AUTHORING_SCHEMA_VERSION = 1
BUILD_MANIFEST_SCHEMA_VERSION = 1
SCHEMA_INVENTORY_VERSION = 1
PLATFORM_BUNDLE_SCHEMA_VERSION = 1
FINGERPRINT_CANONICALISATION_VERSION = 1
EDPL_FORMAT_MAJOR = 1
EDPL_FORMAT_MINOR = 0


class ToolError(Exception):
    """One deterministic user-facing tooling failure."""


class DuplicateKeyError(ToolError):
    pass


@dataclass(frozen=True, order=True)
class Diagnostic:
    source: str
    location: str
    reason: str

    def render(self) -> str:
        where = self.source
        if self.location:
            where += f":{self.location}"
        return f"{where}: {self.reason}"


class Diagnostics:
    def __init__(self) -> None:
        self._items: list[Diagnostic] = []

    def add(self, source: Path | str, location: str, reason: str) -> None:
        self._items.append(Diagnostic(str(source), location, reason))

    def extend(self, items: Iterable[Diagnostic]) -> None:
        self._items.extend(items)

    @property
    def items(self) -> list[Diagnostic]:
        return sorted(self._items)

    def raise_if_any(self) -> None:
        if not self._items:
            return
        rendered = "\n".join(item.render() for item in self.items)
        raise ToolError(rendered)


def _object_pairs_no_duplicates(pairs: list[tuple[str, Any]]) -> dict[str, Any]:
    result: dict[str, Any] = {}
    for key, value in pairs:
        if key in result:
            raise DuplicateKeyError(f"duplicate JSON object key {key!r}")
        result[key] = value
    return result


def load_json(path: Path) -> Any:
    try:
        raw = path.read_bytes()
    except OSError as exc:
        raise ToolError(f"{path}: unable to read: {exc}") from exc

    try:
        text = raw.decode("utf-8", errors="strict")
    except UnicodeDecodeError as exc:
        raise ToolError(f"{path}: not valid UTF-8: {exc}") from exc

    try:
        return json.loads(text, object_pairs_hook=_object_pairs_no_duplicates)
    except DuplicateKeyError as exc:
        raise ToolError(f"{path}: {exc}") from exc
    except json.JSONDecodeError as exc:
        raise ToolError(f"{path}:{exc.lineno}:{exc.colno}: invalid JSON: {exc.msg}") from exc


def require_object(value: Any, source: Path, location: str) -> dict[str, Any]:
    if not isinstance(value, dict):
        raise ToolError(f"{source}:{location}: expected JSON object")
    return value


def require_array(value: Any, source: Path, location: str) -> list[Any]:
    if not isinstance(value, list):
        raise ToolError(f"{source}:{location}: expected JSON array")
    return value


def reject_unknown(obj: dict[str, Any], allowed: set[str], source: Path, location: str) -> None:
    unknown = sorted(set(obj) - allowed)
    if unknown:
        raise ToolError(f"{source}:{location}: unknown properties: {', '.join(unknown)}")


def require_keys(obj: dict[str, Any], required: set[str], source: Path, location: str) -> None:
    missing = sorted(required - set(obj))
    if missing:
        raise ToolError(f"{source}:{location}: missing required properties: {', '.join(missing)}")


def require_int(value: Any, source: Path, location: str) -> int:
    if isinstance(value, bool) or not isinstance(value, int):
        raise ToolError(f"{source}:{location}: expected integer")
    return value


def require_bool(value: Any, source: Path, location: str) -> bool:
    if not isinstance(value, bool):
        raise ToolError(f"{source}:{location}: expected boolean")
    return value


def require_text(value: Any, source: Path, location: str, *, nonempty: bool = False) -> str:
    if not isinstance(value, str):
        raise ToolError(f"{source}:{location}: expected string")
    if "\x00" in value:
        raise ToolError(f"{source}:{location}: embedded NUL is not permitted")
    if nonempty and not value:
        raise ToolError(f"{source}:{location}: value must be non-empty")
    try:
        value.encode("utf-8", errors="strict")
    except UnicodeEncodeError as exc:
        raise ToolError(f"{source}:{location}: value is not valid UTF-8 scalar text") from exc
    return value


_CANON_UINT_RE = re.compile(r"(?:0|[1-9][0-9]*)\Z")
_HEX_TYPE_RE = re.compile(r"0x[0-9A-F]+\Z")
_CPP_IDENTIFIER_RE = re.compile(r"[A-Za-z_][A-Za-z0-9_]*\Z")
_SHA256_PIN_RE = re.compile(r"sha256-([0-9A-Fa-f]{64})\Z")


_CPP20_RESERVED_IDENTIFIERS = frozenset({
    "alignas", "alignof", "and", "and_eq", "asm", "auto",
    "bitand", "bitor", "bool", "break",
    "case", "catch", "char", "char8_t", "char16_t", "char32_t", "class",
    "compl", "concept", "const", "consteval", "constexpr", "constinit",
    "const_cast", "continue", "co_await", "co_return", "co_yield",
    "decltype", "default", "delete", "do", "double", "dynamic_cast",
    "else", "enum", "explicit", "export", "extern",
    "false", "float", "for", "friend",
    "goto",
    "if", "inline", "int", "import",
    "long",
    "module", "mutable",
    "namespace", "new", "noexcept", "not", "not_eq", "nullptr",
    "operator", "or", "or_eq",
    "private", "protected", "public",
    "register", "reinterpret_cast", "requires", "return",
    "short", "signed", "sizeof", "static", "static_assert", "static_cast",
    "struct", "switch",
    "template", "this", "thread_local", "throw", "true", "try", "typedef",
    "typeid", "typename",
    "union", "unsigned", "using",
    "virtual", "void", "volatile",
    "wchar_t", "while",
    "xor", "xor_eq",
})


def parse_canonical_uint_key(text: str, max_value: int, source: Path, location: str) -> int:
    if not isinstance(text, str) or _CANON_UINT_RE.fullmatch(text) is None:
        raise ToolError(f"{source}:{location}: expected canonical unsigned-decimal object key")
    value = int(text, 10)
    if value > max_value:
        raise ToolError(f"{source}:{location}: value {value} does not fit configured identifier width")
    return value


def width_max(byte_width: int) -> int:
    if byte_width not in (1, 2, 4):
        raise ToolError(f"identifier width must be exactly 1, 2, or 4 bytes, got {byte_width}")
    return (1 << (byte_width * 8)) - 1


def parse_type_identifier(text: str, byte_width: int, source: Path, location: str) -> bytes:
    if not isinstance(text, str) or _HEX_TYPE_RE.fullmatch(text) is None:
        raise ToolError(f"{source}:{location}: TypeIdentifier must use 0x followed by uppercase hexadecimal")
    digits = text[2:]
    expected = byte_width * 2
    if len(digits) != expected:
        raise ToolError(f"{source}:{location}: TypeIdentifier requires exactly {expected} hexadecimal digits")
    return bytes.fromhex(digits)


def format_type_identifier(value: bytes) -> str:
    return "0x" + value.hex().upper()


def validate_cpp_identifier(identifier: str, description: str) -> str:
    if _CPP_IDENTIFIER_RE.fullmatch(identifier) is None:
        raise ToolError(f"{description} {identifier!r} is not a valid C++ identifier")

    if (
        identifier in _CPP20_RESERVED_IDENTIFIERS
        or identifier.startswith("_")
        or "__" in identifier
    ):
        raise ToolError(f"{description} {identifier!r} is reserved in C++20")

    return identifier


def validate_cpp_namespace(namespace: str) -> tuple[str, ...]:
    parts = tuple(namespace.split("::"))

    if not parts:
        raise ToolError(f"invalid C++ namespace {namespace!r}")

    for part in parts:
        validate_cpp_identifier(
            part,
            "C++ namespace component",
        )

    return parts


def parse_sha256_pin(value: Any, source: Path, location: str) -> str:
    text = require_text(value, source, location, nonempty=True)
    match = _SHA256_PIN_RE.fullmatch(text)
    if match is None:
        raise ToolError(f"{source}:{location}: integrity must be sha256- followed by 64 hexadecimal digits")
    return match.group(1).upper()


def sha256_hex(data: bytes) -> str:
    return hashlib.sha256(data).hexdigest().upper()


def canonical_json_bytes(value: Any) -> bytes:
    return (
        json.dumps(
            value,
            ensure_ascii=False,
            sort_keys=True,
            separators=(",", ":"),
        )
        + "\n"
    ).encode("utf-8")


def write_json(path: Path, value: Any) -> None:
    path.parent.mkdir(parents=True, exist_ok=True)
    path.write_bytes(canonical_json_bytes(value))


def canonical_bundle_digest(root: Path) -> str:
    """Hash exact bundle file bytes in canonical relative-path order."""
    if not root.is_dir():
        raise ToolError(f"platform bundle path is not a directory: {root}")

    files = sorted(
        p for p in root.rglob("*")
        if p.is_file()
    )
    digest = hashlib.sha256()
    digest.update(b"EDP-LOCALISATION-PLATFORM-BUNDLE-INTEGRITY-V1\0")
    for path in files:
        rel = path.relative_to(root).as_posix().encode("utf-8")
        data = path.read_bytes()
        digest.update(len(rel).to_bytes(4, "little"))
        digest.update(rel)
        digest.update(len(data).to_bytes(8, "little"))
        digest.update(data)
    return digest.hexdigest().upper()


def canonical_bcp47(tag: Any, source: Path, location: str) -> str:
    tag = require_text(tag, source, location, nonempty=True)
    raw = tag.encode("ascii", errors="strict") if tag.isascii() else None
    if raw is None or len(raw) > 255:
        raise ToolError(f"{source}:{location}: BCP47 identity must be ASCII and at most 255 bytes")
    if any(not (ch.isalnum() or ch == "-") for ch in tag):
        raise ToolError(f"{source}:{location}: invalid BCP47 syntax")

    parts = tag.split("-")
    if any(not part or len(part) > 8 for part in parts):
        raise ToolError(f"{source}:{location}: invalid BCP47 subtag length")

    if parts[0] == "x":
        if len(parts) < 2 or any(part.lower() != part for part in parts[1:]):
            raise ToolError(f"{source}:{location}: private-use BCP47 tag must be canonical lowercase")
        return tag

    primary = parts[0]
    if not (2 <= len(primary) <= 8 and primary.isalpha() and primary.lower() == primary):
        raise ToolError(f"{source}:{location}: invalid or non-canonical primary language subtag")

    index = 1
    if len(primary) <= 3:
        extlang_count = 0
        while index < len(parts) and extlang_count < 3:
            part = parts[index]
            if len(part) != 3 or not part.isalpha():
                break
            if part.lower() != part:
                raise ToolError(f"{source}:{location}: extlang subtags must be canonical lowercase")
            index += 1
            extlang_count += 1

    if index < len(parts) and len(parts[index]) == 4 and parts[index].isalpha():
        script = parts[index]
        if not (script[0].isupper() and script[1:].islower()):
            raise ToolError(f"{source}:{location}: script subtag must use canonical title case")
        index += 1

    if index < len(parts):
        region = parts[index]
        if len(region) == 2 and region.isalpha():
            if region.upper() != region:
                raise ToolError(f"{source}:{location}: alpha region subtag must be canonical uppercase")
            index += 1
        elif len(region) == 3 and region.isdigit():
            index += 1

    while index < len(parts):
        part = parts[index]
        is_variant = (5 <= len(part) <= 8 and part.isalnum()) or (
            len(part) == 4 and part[0].isdigit() and part.isalnum()
        )
        if not is_variant:
            break
        if part.lower() != part:
            raise ToolError(f"{source}:{location}: variant subtags must be canonical lowercase")
        index += 1

    previous_singleton: str | None = None
    while index < len(parts):
        singleton = parts[index]
        if len(singleton) != 1 or singleton == "x":
            break
        if not singleton.isalnum() or singleton.lower() != singleton:
            raise ToolError(f"{source}:{location}: extension singleton must be canonical lowercase alphanumeric")
        if previous_singleton is not None and singleton <= previous_singleton:
            raise ToolError(f"{source}:{location}: extension singletons must be unique and canonical ordered")
        previous_singleton = singleton
        index += 1
        start = index
        while index < len(parts) and len(parts[index]) != 1:
            part = parts[index]
            if not (2 <= len(part) <= 8 and part.isalnum() and part.lower() == part):
                raise ToolError(f"{source}:{location}: invalid extension subtag")
            index += 1
        if index == start:
            raise ToolError(f"{source}:{location}: extension singleton must be followed by a subtag")

    if index < len(parts):
        if parts[index] != "x":
            raise ToolError(f"{source}:{location}: invalid BCP47 trailing subtag")
        index += 1
        if index >= len(parts):
            raise ToolError(f"{source}:{location}: private-use singleton requires at least one subtag")
        for part in parts[index:]:
            if not (1 <= len(part) <= 8 and part.isalnum() and part.lower() == part):
                raise ToolError(f"{source}:{location}: invalid canonical private-use subtag")
        index = len(parts)

    if index != len(parts):
        raise ToolError(f"{source}:{location}: invalid BCP47 syntax")
    return tag
