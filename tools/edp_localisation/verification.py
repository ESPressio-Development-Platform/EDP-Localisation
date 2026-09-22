from __future__ import annotations

import re
import tempfile
from pathlib import Path
from typing import Iterable

from .common import (
    BUILD_MANIFEST_SCHEMA_VERSION, EDPL_FORMAT_MAJOR, EDPL_FORMAT_MINOR,
    FINGERPRINT_CANONICALISATION_VERSION, TOOLCHAIN_IDENTITY, TOOLCHAIN_VERSION,
    ToolError, load_json, reject_unknown, require_array, require_int, require_keys,
    require_object, require_text, sha256_hex,
)
from .edpl import Pack
from .generator import generate_to_directory


def _validate_manifest_shape(path: Path, root: dict) -> None:
    allowed = {
        "schemaVersion", "toolchain", "edplocFormat", "fingerprintCanonicalisationVersion",
        "contractFamilyFingerprint", "contractFamilyDigestSha256", "terminalLanguage",
        "supportedLanguages", "outputs",
    }
    require_keys(root, allowed, path, "$")
    reject_unknown(root, allowed, path, "$")
    if require_int(root["schemaVersion"], path, "$.schemaVersion") != BUILD_MANIFEST_SCHEMA_VERSION:
        raise ToolError(f"{path}: unsupported build-manifest schema version")

    toolchain = require_object(root["toolchain"], path, "$.toolchain")
    require_keys(toolchain, {"identity", "version"}, path, "$.toolchain")
    reject_unknown(toolchain, {"identity", "version"}, path, "$.toolchain")
    if require_text(toolchain["identity"], path, "$.toolchain.identity", nonempty=True) != TOOLCHAIN_IDENTITY:
        raise ToolError(f"{path}: unexpected toolchain identity")
    if require_text(toolchain["version"], path, "$.toolchain.version", nonempty=True) != TOOLCHAIN_VERSION:
        raise ToolError(f"{path}: generated set was built by a different EDP-Localisation release")

    fmt = require_object(root["edplocFormat"], path, "$.edplocFormat")
    require_keys(fmt, {"major", "minor"}, path, "$.edplocFormat")
    reject_unknown(fmt, {"major", "minor"}, path, "$.edplocFormat")
    if (
        require_int(fmt["major"], path, "$.edplocFormat.major") != EDPL_FORMAT_MAJOR
        or require_int(fmt["minor"], path, "$.edplocFormat.minor") != EDPL_FORMAT_MINOR
    ):
        raise ToolError(f"{path}: generated set uses an unsupported EDPL format")
    if require_int(root["fingerprintCanonicalisationVersion"], path, "$.fingerprintCanonicalisationVersion") != FINGERPRINT_CANONICALISATION_VERSION:
        raise ToolError(f"{path}: unsupported fingerprint canonicalisation version")

    fingerprint = require_text(root["contractFamilyFingerprint"], path, "$.contractFamilyFingerprint", nonempty=True)
    digest = require_text(root["contractFamilyDigestSha256"], path, "$.contractFamilyDigestSha256", nonempty=True)
    if re.fullmatch(r"[0-9A-F]{32}", fingerprint) is None:
        raise ToolError(f"{path}: ContractFamilyFingerprint must be 32 uppercase hexadecimal digits")
    if re.fullmatch(r"[0-9A-F]{64}", digest) is None:
        raise ToolError(f"{path}: ContractFamily digest must be 64 uppercase hexadecimal digits")


def load_build_manifest(generated_root: Path) -> dict:
    path = generated_root / "localisation-build-manifest.json"
    root = require_object(load_json(path), path, "$")
    _validate_manifest_shape(path, root)
    supported = require_array(root["supportedLanguages"], path, "$.supportedLanguages")
    if supported != sorted(supported) or len(supported) != len(set(supported)):
        raise ToolError(f"{path}: supportedLanguages must be unique and canonical-sorted")
    outputs = require_array(root["outputs"], path, "$.outputs")
    previous: str | None = None
    seen: set[str] = set()
    for index, raw in enumerate(outputs):
        location = f"$.outputs[{index}]"
        obj = require_object(raw, path, location)
        allowed = {"path", "kind", "sha256", "language"}
        require_keys(obj, {"path", "kind", "sha256"}, path, location)
        reject_unknown(obj, allowed, path, location)
        rel = require_text(obj["path"], path, f"{location}.path", nonempty=True)
        if rel.startswith("/") or "\\" in rel or any(part in ("", ".", "..") for part in rel.split("/")):
            raise ToolError(f"{path}:{location}.path: invalid generated relative path")
        if previous is not None and rel <= previous:
            raise ToolError(f"{path}: outputs must be strictly sorted by path")
        previous = rel
        if rel in seen:
            raise ToolError(f"{path}: duplicate output path {rel}")
        seen.add(rel)
        require_text(obj["kind"], path, f"{location}.kind", nonempty=True)
        digest_value = require_text(obj["sha256"], path, f"{location}.sha256", nonempty=True)
        if re.fullmatch(r"[0-9A-F]{64}", digest_value) is None:
            raise ToolError(f"{path}:{location}.sha256: expected 64 uppercase hexadecimal digits")
        if obj["kind"] == "language-pack":
            if "language" not in obj:
                raise ToolError(f"{path}:{location}: language-pack output requires language")
            require_text(obj["language"], path, f"{location}.language", nonempty=True)
        elif "language" in obj:
            raise ToolError(f"{path}:{location}: only language-pack outputs carry language")
    return root


def verify_internal_generated_set(generated_root: Path) -> dict:
    generated_root = generated_root.resolve()
    manifest = load_build_manifest(generated_root)
    declared = {item["path"]: item for item in manifest["outputs"]}
    actual_files = {
        path.relative_to(generated_root).as_posix()
        for path in generated_root.rglob("*")
        if path.is_file() and path.name != "localisation-build-manifest.json"
    }
    if actual_files != set(declared):
        missing = sorted(set(declared) - actual_files)
        unexpected = sorted(actual_files - set(declared))
        detail = []
        if missing:
            detail.append(f"missing outputs: {', '.join(missing)}")
        if unexpected:
            detail.append(f"unexpected outputs: {', '.join(unexpected)}")
        raise ToolError("generated output set mismatch: " + "; ".join(detail))

    expected_fp = bytes.fromhex(manifest["contractFamilyFingerprint"])
    for rel, item in sorted(declared.items()):
        data = (generated_root / rel).read_bytes()
        actual_digest = sha256_hex(data)
        if actual_digest != item["sha256"]:
            raise ToolError(f"generated output digest mismatch: {rel}")
        if item["kind"] == "language-pack":
            pack = Pack(data)
            if pack.language != item["language"]:
                raise ToolError(f"generated pack embedded language mismatch: {rel}")
            if pack.fingerprint != expected_fp:
                raise ToolError(f"generated pack ContractFamilyFingerprint mismatch: {rel}")
    return manifest


def _tree_bytes(root: Path) -> dict[str, bytes]:
    return {
        path.relative_to(root).as_posix(): path.read_bytes()
        for path in sorted(root.rglob("*"))
        if path.is_file()
    }


def verify_generated_set(
    source_root: Path,
    platform_bundle: Path,
    schema_inventories: Iterable[Path],
    generated_root: Path,
    cpp_namespace: str,
) -> None:
    verify_internal_generated_set(generated_root)
    schema_inputs = tuple(Path(value).resolve() for value in schema_inventories)
    with tempfile.TemporaryDirectory(prefix="edp-localisation-verify-") as temp:
        expected_root = Path(temp) / "generated"
        generate_to_directory(
            source_root.resolve(),
            platform_bundle.resolve(),
            schema_inputs,
            expected_root,
            cpp_namespace,
        )
        actual = _tree_bytes(generated_root.resolve())
        expected = _tree_bytes(expected_root)
        if actual.keys() != expected.keys():
            raise ToolError("generated set is stale: expected output path set differs")
        stale = [path for path in sorted(actual) if actual[path] != expected[path]]
        if stale:
            raise ToolError("generated set is stale: byte mismatch in " + ", ".join(stale))
