#!/usr/bin/env python3

import argparse
import pathlib
import subprocess
import sys


def existing_directory(value: str) -> pathlib.Path:
    path = pathlib.Path(value).expanduser().resolve()

    if not path.is_dir():
        raise argparse.ArgumentTypeError(f"not a directory: {path}")

    return path


def default_sibling(root: pathlib.Path, name: str) -> pathlib.Path:
    return (root.parent / name).resolve()


def main() -> int:
    repository = pathlib.Path(__file__).resolve().parents[1]

    parser = argparse.ArgumentParser(
        description="Compile and execute EDP-Localisation host contract tests."
    )
    parser.add_argument(
        "--system",
        type=existing_directory,
        default=default_sibling(repository, "EDP-System"),
    )
    parser.add_argument(
        "--memory",
        type=existing_directory,
        default=default_sibling(repository, "EDP-Memory"),
    )
    parser.add_argument(
        "--compiler",
        default="c++",
    )
    args = parser.parse_args()

    for dependency_name, dependency_path in (
        ("EDP-System", args.system),
        ("EDP-Memory", args.memory),
    ):
        if not dependency_path.is_dir():
            print(
                f"ERROR: missing {dependency_name} checkout at {dependency_path}",
                file=sys.stderr,
            )
            return 2

    output = repository / "tests" / ".localisation_contract_tests"

    command = [
        args.compiler,
        "-std=c++20",
        "-Wall",
        "-Wextra",
        "-Werror",
        f"-I{repository / 'src'}",
        f"-I{args.system / 'src'}",
        f"-I{args.memory / 'src'}",
        str(repository / "tests" / "LocalisationContractTests.cpp"),
        "-o",
        str(output),
    ]

    print(" ".join(command))
    subprocess.run(
        command,
        check=True,
    )

    try:
        subprocess.run(
            [str(output)],
            check=True,
        )
    finally:
        output.unlink(missing_ok=True)

    print("EDP-Localisation host contract tests passed.")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
