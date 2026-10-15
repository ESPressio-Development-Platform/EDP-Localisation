# tools/edp_localisation/diagnostic_templates.py

**Classification:** INTERNAL TOOLING / FUTURE HOST PRESENTATION CONTRACT

**Source baseline:** `feature/logging-v1` commit `472c58dd8539794e8b70e98f095e1cd298421137`.

## DiagnosticTemplateError

A strongly distinguished Python exception class derived from `ValueError`, raised when a governed diagnostic source template or already-formatted argument presentation is malformed, unsupported, or exceeds its caller-defined output budget.

## parse_diagnostic_template(template, parameter_count)

Parses a complete String in the governed positional grammar. Each result element is either a literal `str` (after `{{` / `}}` decoding) or a numeric `int` parameter position. Valid references are canonical ASCII `{0}`, `{1}` etc, within the declared count. Invalid/mismatched braces, Unicode-digit placeholders, numeric leading zeros, overlong numeric tokens and undeclared parameter references fail without evaluating anything. The function neither allocates embedded device memory nor changes the caller's text.

## render_diagnostic_template(template, arguments, max_output_bytes=65536)

Accepts caller-supplied *already formatted and schema-validated* text argument positions and replaces parsed occurrences; it never converts strong types or formats numeric scalars implicitly. Every substituted/literal component is counted using exact UTF-8 bytes against the given finite limit. A non-string argument, illegal bound, invalid Unicode scalar, or overflow raises `DiagnosticTemplateError`. This is a Python host primitive; C++ on-device formatter is outstanding.

## Ownership

Only EDP-Localisation owns placeholder grammar. The Localisation compiler's `model._validate_diagnostic_placeholders` calls `parse_diagnostic_template` and adds source context to reported `ToolError`. The future EDP-Logging Python CLI may consume this helper after resolving appropriate historical schema and translations, rather than duplicate its own parser.

**Validation:** Unit test authored but NOT RUN; both user execution appliances unavailable.
