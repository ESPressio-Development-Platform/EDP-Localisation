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


def compile_and_run(
    repository: pathlib.Path,
    compiler: str,
    dependency_includes: list[pathlib.Path],
    source_name: str,
    output_name: str,
) -> None:
    output = repository / "tests" / output_name
    command = [
        compiler,
        "-std=c++20",
        "-Wall",
        "-Wextra",
        "-Werror",
        "-pthread",
        f"-I{repository / 'src'}",
        *(
            f"-I{dependency / 'src'}"
            for dependency in dependency_includes
        ),
        str(repository / "tests" / source_name),
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
        "--persistence",
        type=existing_directory,
        default=default_sibling(repository, "EDP-Persistence"),
    )
    parser.add_argument(
        "--compiler",
        default="c++",
    )
    args = parser.parse_args()

    dependencies = (
        ("EDP-System", args.system),
        ("EDP-Memory", args.memory),
        ("EDP-Persistence", args.persistence),
    )

    for dependency_name, dependency_path in dependencies:
        if not dependency_path.is_dir():
            print(
                f"ERROR: missing {dependency_name} checkout at {dependency_path}",
                file=sys.stderr,
            )
            return 2

    compile_and_run(
        repository,
        args.compiler,
        [
            args.system,
            args.memory,
        ],
        "LocalisationContractTests.cpp",
        ".localisation_contract_tests",
    )

    compile_and_run(
        repository,
        args.compiler,
        [
            args.system,
            args.memory,
            args.persistence,
        ],
        "FilePackSourceContractTests.cpp",
        ".file_pack_source_contract_tests",
    )

    print("EDP-Localisation host contract tests passed.")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
