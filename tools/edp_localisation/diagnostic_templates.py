from __future__ import annotations

from collections.abc import Sequence


class DiagnosticTemplateError(ValueError):
    """Invalid governed positional diagnostic template or rendered value."""


def parse_diagnostic_template(
    template: str,
    parameter_count: int,
) -> tuple[str | int, ...]:
    """Decode canonical {N} placeholders and doubled-brace escapes.

    Literal components are str, argument references are int. This reusable
    grammar is shared by Localisation authoring validation and host formatting.
    """
    if not isinstance(template, str):
        raise DiagnosticTemplateError("template must be text")
    if type(parameter_count) is not int or parameter_count < 0:
        raise DiagnosticTemplateError("parameter count must be a non-negative integer")

    parts: list[str | int] = []
    literal: list[str] = []
    offset = 0
    while offset < len(template):
        current = template[offset]
        if current == "{":
            if template.startswith("{{", offset):
                literal.append("{")
                offset += 2
                continue
            close = template.find("}", offset + 1)
            if close < 0:
                raise DiagnosticTemplateError("unclosed placeholder")
            number = template[offset + 1:close]
            if (
                not number
                or len(number) > 10
                or not number.isascii()
                or not number.isdecimal()
                or len(number) > 1 and number.startswith("0")
            ):
                raise DiagnosticTemplateError(f"invalid placeholder {{{number}}}")
            index = int(number)
            if index >= parameter_count:
                raise DiagnosticTemplateError(f"undeclared parameter {index}")
            if literal:
                parts.append("".join(literal))
                literal.clear()
            parts.append(index)
            offset = close + 1
        elif current == "}":
            if template.startswith("}}", offset):
                literal.append("}")
                offset += 2
            else:
                raise DiagnosticTemplateError("unescaped closing brace")
        else:
            literal.append(current)
            offset += 1
    if literal:
        parts.append("".join(literal))
    return tuple(parts)


def render_diagnostic_template(
    template: str,
    arguments: Sequence[str],
    *,
    max_output_bytes: int = 65536,
) -> str:
    """Render already type-validated argument presentations into governed text.

    Typed value-to-text conversion remains the calling codec/presentation
    component's responsibility. This function does no implicit coercion,
    printf-style formatting, shell interpolation or dynamic eval.
    """
    if type(max_output_bytes) is not int or max_output_bytes < 0:
        raise DiagnosticTemplateError("maximum output bytes must be non-negative")
    parts = parse_diagnostic_template(template, len(arguments))
    output: list[str] = []
    byte_count = 0
    for value in parts:
        text = arguments[value] if type(value) is int else value
        if not isinstance(text, str):
            raise DiagnosticTemplateError("all argument presentations must be text")
        try:
            encoded_length = len(text.encode("utf-8", errors="strict"))
        except UnicodeEncodeError as error:
            raise DiagnosticTemplateError("argument or template contains invalid UTF-8") from error
        if encoded_length > max_output_bytes - byte_count:
            raise DiagnosticTemplateError("rendered output exceeds configured byte limit")
        byte_count += encoded_length
        output.append(text)
    return "".join(output)
