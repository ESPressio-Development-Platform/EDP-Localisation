from __future__ import annotations

import os
import stat
from contextlib import contextmanager
import re
import shutil
import tempfile

try:
    import fcntl
except ImportError:
    fcntl = None
from pathlib import Path

from .common import ToolError
from .generated import GeneratedContractFamily
from .verification import verify_internal_generated_set

_HEX16 = re.compile(r"[0-9A-F]{32}\Z")
_HEX32 = re.compile(r"[0-9A-F]{64}\Z")


def _fsync_directory(directory: Path) -> None:
    """Durability barrier for published directory entries (POSIX filesystem)."""
    descriptor = os.open(directory, os.O_RDONLY | getattr(os, "O_DIRECTORY", 0))
    try:
        os.fsync(descriptor)
    finally:
        os.close(descriptor)


@contextmanager
def _publisher_lock(root: Path):
    """One active local publisher; no unlink/recreate of the lock inode.

    A competing independent process fails closed instead of waiting forever.
    No advisory lock is claimed to coordinate non-cooperating writers.
    """
    if fcntl is None or not hasattr(fcntl, "flock"):
        raise ToolError("historical publication requires POSIX flock")
    lock_path = root / ".publisher.lock"
    flags = os.O_CREAT | os.O_RDWR | getattr(os, "O_NOFOLLOW", 0)
    try:
        descriptor = os.open(lock_path, flags, 0o600)
    except OSError as error:
        raise ToolError(f"historical publisher lock could not be opened: {error}") from error
    try:
        if not stat.S_ISREG(os.fstat(descriptor).st_mode):
            raise ToolError("historical publisher lock must be a regular file")
        try:
            fcntl.flock(descriptor, fcntl.LOCK_EX | fcntl.LOCK_NB)
        except BlockingIOError as error:
            raise ToolError("another historical publisher is active") from error
        try:
            _fsync_directory(root)
            yield
        finally:
            fcntl.flock(descriptor, fcntl.LOCK_UN)
    finally:
        os.close(descriptor)


def _copy_synced(source: Path, destination: Path) -> None:
    """Exact bytes persisted before the family becomes visible."""
    with source.open("rb") as input_file, destination.open("xb") as output_file:
        shutil.copyfileobj(input_file, output_file)
        output_file.flush()
        os.fsync(output_file.fileno())


def _fsync_tree(root: Path) -> None:
    """Persist all nested directory entries before atomic family rename."""
    directories = [root]
    directories.extend(path for path in root.rglob("*") if path.is_dir())
    for directory in sorted(directories, key=lambda p: len(p.parts), reverse=True):
        _fsync_directory(directory)


def _reject_symlinks(root: Path) -> None:
    """Prevent an archive snapshot from following source or archive links."""
    if root.is_symlink():
        raise ToolError(f"historical archive path is a symbolic link: {root}")
    for path in root.rglob("*"):
        if path.is_symlink():
            raise ToolError(f"historical archive may not contain symbolic links: {path}")


def _validated_family(path: Path, *, digest: str | None = None) -> dict:
    if not path.is_dir():
        raise ToolError(f"historical family is not a directory: {path}")
    _reject_symlinks(path)
    manifest = verify_internal_generated_set(path)
    if digest is not None and manifest["contractFamilyDigestSha256"] != digest:
        raise ToolError("historical archive family path does not match semantic digest")
    if manifest["contractFamilyDigestSha256"][:32] != manifest["contractFamilyFingerprint"]:
        raise ToolError("historical fingerprint must equal the canonical digest prefix")
    # Existing parser enforces exact CRC32C, pack fingerprints and output hashes.
    return manifest


def _families(root: Path) -> list[tuple[Path, dict]]:
    base = root / "families"
    if not base.exists():
        return []
    _reject_symlinks(base)
    result: list[tuple[Path, dict]] = []
    for path in sorted(base.iterdir()):
        if path.name.startswith(".stage-"):
            # An interrupted publication cannot be considered historical.
            continue
        if not _HEX32.fullmatch(path.name):
            raise ToolError(f"unexpected historical family directory {path.name!r}")
        result.append((path, _validated_family(path, digest=path.name)))
    return result


def open_historical_family(
    archive_root: Path,
    contract_family_fingerprint: str,
    *,
    digest: str | None = None,
) -> GeneratedContractFamily:
    """Resolve exact 128-bit family identity, rejecting all ambiguous matches.

    This is structural integrity, not proof that an archive came from a
    trusted publisher; callers must trust the archive source/provenance.
    """
    if not _HEX16.fullmatch(contract_family_fingerprint):
        raise ToolError("historical ContractFamilyFingerprint must be 32 uppercase hex digits")
    if digest is not None and not _HEX32.fullmatch(digest):
        raise ToolError("historical ContractFamilyDigest must be 64 uppercase hex digits")
    matching = [
        path for path, manifest in _families(archive_root)
        if manifest["contractFamilyFingerprint"] == contract_family_fingerprint
        and (digest is None or manifest["contractFamilyDigestSha256"] == digest)
    ]
    if len(matching) != 1:
        raise ToolError("historical family unavailable or ambiguous for the supplied fingerprint")
    return GeneratedContractFamily(matching[0])


def archive_generated_family(generated_root: Path, archive_root: Path) -> dict:
    """Publish an internally verified, immutable, by-digest generated family.

    No silent overwrite. Does not recreate a historical SemanticModel or
    sign/attest provenance; caller owns verified provenance and external
    serialization of multiple simultaneous publishers.
    """
    source = Path(generated_root)
    if not source.is_dir():
        raise ToolError(f"generated family is not a directory: {source}")
    _reject_symlinks(source)
    manifest = _validated_family(source)
    digest = manifest["contractFamilyDigestSha256"]
    fingerprint = manifest["contractFamilyFingerprint"]

    root = Path(archive_root)
    if root.is_symlink():
        raise ToolError("historical archive root cannot be a symbolic link")
    root.mkdir(parents=True, exist_ok=True)
    _reject_symlinks(root)

    with _publisher_lock(root):
        base = root / "families"
        base.mkdir(exist_ok=True)
        _reject_symlinks(base)
        _fsync_directory(root)

        existing = _families(root)
        for _path, other in existing:
            if other["contractFamilyFingerprint"] == fingerprint and (
                other["contractFamilyDigestSha256"] != digest
            ):
                raise ToolError("historical 128-bit fingerprint collision: publication rejected")

        target = base / digest
        if target.exists():
            current = _validated_family(target, digest=digest)
            if current != manifest:
                raise ToolError("existing historical family has altered manifest")
            expected = {
                "localisation-build-manifest.json",
                *(entry["path"] for entry in manifest["outputs"]),
            }
            for relative in expected:
                if (source / relative).read_bytes() != (target / relative).read_bytes():
                    raise ToolError("existing historical family differs byte-for-byte")
            return {"status": "AlreadyArchived", "digest": digest, "fingerprint": fingerprint}

        stage = Path(tempfile.mkdtemp(prefix=".stage-", dir=base))
        try:
            for entry in manifest["outputs"]:
                relative = Path(entry["path"])
                destination = stage / relative
                destination.parent.mkdir(parents=True, exist_ok=True)
                _copy_synced(source / relative, destination)
            _copy_synced(
                source / "localisation-build-manifest.json",
                stage / "localisation-build-manifest.json",
            )
            _validated_family(stage, digest=digest)
            _fsync_tree(stage)
            if target.exists():
                raise ToolError("concurrent historical publication detected; retry verification")
            os.rename(stage, target)
            # A returned success requires persistent directory publication.
            _fsync_directory(base)
        finally:
            if stage.exists():
                shutil.rmtree(stage)

        _validated_family(target, digest=digest)
        return {"status": "Archived", "digest": digest, "fingerprint": fingerprint}
