# tools/edp_localisation/cli.py

**Classification:** INTERNAL TOOLING

**Source baseline:** `83aa04323787a3b9ce48fa452dffdd960f1358af`

[Open exact source](https://github.com/ESPressio-Development-Platform/EDP-Localisation/blob/83aa04323787a3b9ce48fa452dffdd960f1358af/tools/edp_localisation/cli.py)

## `_path`

**Kind:** def · **Scope:** module

Purpose is defined by the implementation and call sites; no module docstring is attached to this declaration.

```python
def _path(value: str) -> Path:
```

## `_uint`

**Kind:** def · **Scope:** module

Purpose is defined by the implementation and call sites; no module docstring is attached to this declaration.

```python
def _uint(value: str) -> int:
```

## `_common_source_args`

**Kind:** def · **Scope:** module

Purpose is defined by the implementation and call sites; no module docstring is attached to this declaration.

```python
def _common_source_args(parser: argparse.ArgumentParser, *, output: bool) -> None:
```

## `_emit`

**Kind:** def · **Scope:** module

Purpose is defined by the implementation and call sites; no module docstring is attached to this declaration.

```python
def _emit(value: object, *, json_output: bool) -> None:
```

## `_resolution_human`

**Kind:** def · **Scope:** module

Purpose is defined by the implementation and call sites; no module docstring is attached to this declaration.

```python
def _resolution_human(label: str, result: Resolution) -> str:
```

## `_command_init`

**Kind:** def · **Scope:** module

Purpose is defined by the implementation and call sites; no module docstring is attached to this declaration.

```python
def _command_init(args: argparse.Namespace) -> int:
```

## `_command_validate`

**Kind:** def · **Scope:** module

Purpose is defined by the implementation and call sites; no module docstring is attached to this declaration.

```python
def _command_validate(args: argparse.Namespace) -> int:
```

## `_command_compile`

**Kind:** def · **Scope:** module

Purpose is defined by the implementation and call sites; no module docstring is attached to this declaration.

```python
def _command_compile(args: argparse.Namespace) -> int:
```

## `_command_verify`

**Kind:** def · **Scope:** module

Purpose is defined by the implementation and call sites; no module docstring is attached to this declaration.

```python
def _command_verify(args: argparse.Namespace) -> int:
```

## `_command_resolve`

**Kind:** def · **Scope:** module

Purpose is defined by the implementation and call sites; no module docstring is attached to this declaration.

```python
def _command_resolve(args: argparse.Namespace) -> int:
```

## `_command_decompile`

**Kind:** def · **Scope:** module

Purpose is defined by the implementation and call sites; no module docstring is attached to this declaration.

```python
def _command_decompile(args: argparse.Namespace) -> int:
```

## `build_parser`

**Kind:** def · **Scope:** module

Purpose is defined by the implementation and call sites; no module docstring is attached to this declaration.

```python
def build_parser() -> argparse.ArgumentParser:
```

## `main`

**Kind:** def · **Scope:** module

Purpose is defined by the implementation and call sites; no module docstring is attached to this declaration.

```python
def main(argv: Iterable[str] | None = None) -> int:
```


## `init` Type identity contract

The `init` command exposes neither `--type-identifier-bytes` nor `--field-identifier-bytes`. EDP Type identity is fixed at 64 bits (8 bytes) and Type-local Field identity is fixed at 8 bits (1 byte). Width is therefore never supplied by CLI input.
