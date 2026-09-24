# Policy V2 Compliance Audit

## Scope

This audit re-baselines the complete current `Policy/main/V2` ruleset against the completed EDP-Localisation V1 workstream, including the Localisation-driven changes made in:

- `EDP-Localisation`;
- `EDP-Persistence-Arduino`;
- `EDP-Persistence-ESP-IDF`.

The authoritative living workstream handoff remains:

`Policy/Handoff/2026-09-21/2026-09-21-08-20-Localisation-Persistence-Serialisation-Architecture.MD`.

The audit is tracked by `EDP-Localisation#7`.

## Explicit workstream decisions and exceptions

The following instructions override the corresponding Policy V2 defaults for this workstream:

1. `EDP-Localisation` work is performed directly on `main`, as previously locked in Decision Point 567.
2. All library/toolchain versions remain locked at `0.1.0`; no version mutation is permitted.
3. The detailed top-level README getting-started walkthrough is intentionally retained because first-use adoption guidance is a primary requirement for this consumer-facing library/toolchain.
4. `examples/getting-started` remains under `examples/`. It is a qualitative proof/reference workflow for the documentation rather than an additional logical feature demo.
5. The two already-published V1 public result payload shapes below are explicitly excepted from the strict payload-inaccessibility clause of CODE_CONVENTION Rule 19 because correcting them would require a breaking public API change while version mutation is prohibited:
   - `LanguageIdentifierView::ValidationResult::Value`;
   - `PackSizeResult::SizeBytes`.
6. The conservative Persistence compatibility fallback for older/custom binding profiles without a `Concurrency` member is explicitly retained as a narrow LEGACY-policy exception. Such profiles advertise only `CallerSerialized`.

These exceptions are intentionally narrow and do not establish general exceptions for future APIs or workstreams.

## Policy-by-policy result

| Policy | Audit status | Result |
| --- | --- | --- |
| ABSTRACTION | Compliant | EDP-Localisation core contains no direct Arduino, ESP-IDF, FreeRTOS, POSIX, or other platform-SDK dependency. SDK interaction remains confined to concrete provider repositories and target-specific demos. |
| BRANCHING | Explicit workstream exception | Localisation work is intentionally performed directly on `main` under the locked workstream decision. |
| CIRCULAR_DEPENDENCIES | Compliant | Mandatory Localisation dependencies are EDP-System and EDP-Memory; EDP-Persistence is conditional for FilePackSource. No reverse dependency onto EDP-Localisation is introduced. |
| CODE_CONVENTION | Compliant except explicit Rule-19 exception | Runtime/generated/demo/template documentation and namespace formatting were remediated. Fallible demo operations now use typed results. Internal success-only representation payloads are optional. The two published V1 payload shapes listed above are explicitly excepted. |
| COMPOSITION | Compliant | Localisation exposes Domain/PackSource capabilities, declares scoped/cardinal ByteOperations and FileStorage Requirements inside consolidated Contracts, and uses explicit compile-time selection policies. |
| CPP_VERSION | Compliant | C++20 remains mandatory. PIOArduino is the documented preferred Arduino path and all maintained PlatformIO demos use the approved baseline. |
| DEMOS | Compliant plus explicit getting-started interpretation | Each logical feature demo retains Arduino IDE, PlatformIO Arduino/PIOArduino, and PlatformIO ESP-IDF forms. `examples/getting-started` is explicitly classified as a documentation proof/reference workflow. |
| DEPENDENCY_GRAPH | Compliant | `DEPENDENCY_GRAPH.MD` was revalidated after the audit changes; no dependency edge changed. |
| DETERMINISM | Compliant | Runtime memory is bounded/caller-owned, raw bytes flow through EDP-Memory, no Localisation heap/cache/index was introduced, and generated output remains deterministic. |
| DOCUMENTATION | Compliant plus explicit README interpretation | Public behavior, fallback, memory, concurrency, pack format, source contracts, tooling, build baseline, ISR boundary, and consumer onboarding are documented and reconciled with validated behavior. |
| GITHUB_ACTIONS | Compliant by recorded fallback | The workstream's unavailable/exhausted Actions execution path and the local/recursive validation fallback are recorded in the living handoff. |
| GITHUB_ISSUES | Compliant | Implementation, blocker, onboarding, audit, and decision-gated findings are tracked by repository issues. Audit issue #7 remains open until current-tip validation completes. |
| GITIGNORE | Compliant | OS/editor/IDE/Python/PlatformIO and generated getting-started artefacts are covered. |
| HANDOFF | Compliant | The living Policy handoff has been maintained throughout the implementation and audit, including failures, decisions, exceptions, commits, and validation frontiers. |
| ISR | Compliant | EDP-Localisation makes no ISR-safety guarantee and explicitly documents that resolution/persistence/tooling belong to ordinary execution context. |
| LEGACY | Explicit narrow exception | Older/custom Persistence binding profiles without `Concurrency` remain conservatively `CallerSerialized`; this behavior is now explicitly approved for this workstream. |
| MONOTONIC_TIME | Not applicable | Localisation introduces no clock source, synchronization, persisted timestamp, or monotonic-time contract. |
| OPTIMISATION | Compliant | No optimization-driven API divergence was introduced; runtime remains allocation-free/bounded by design and measured build/resource evidence is recorded where target validation was performed. |
| TEST_COVERAGE | Compliant | The post-audit current tip completed the Python/tooling regression path, host-contract validation, guarded PlatformIO demo matrix, getting-started generation workflow, consumer application build, and LittleFS image build successfully. Existing representative hardware evidence remains applicable because the audit changes did not alter runtime semantics. |
| VERSIONING | Explicit workstream exception/constraint | The user explicitly requires all versions to remain locked at `0.1.0`. The audit therefore avoids breaking public API changes and records the two existing Rule-19 result shapes as narrow exceptions. |

## Remediation performed by the audit

The audit corrected non-breaking compliance defects including:

- missing documentation comments on runtime/internal types and template helpers;
- generated C++ namespace indentation and member documentation;
- missing documentation on committed generated demo fixtures;
- raw integer/Boolean operational results in InBinaryResolution, Validation, FileBackedResolution, and the getting-started application;
- discarded ESP-IDF filesystem-unmount result;
- internal EDPL representation lookup payload exposure on failure;
- stale architecture validation wording;
- explicit dependency-graph revalidation after audit changes;
- truthful README status while current-tip validation remains outstanding.

Generated C++ convention is now guarded by the Python toolchain regression suite.

## Explicitly retained public V1 result shapes

### LanguageIdentifierView::ValidationResult

The V1 result retains:

```cpp
LanguageIdentifierValidationStatus Status;
LanguageIdentifierView Value;
bool IsValuePresent;
```

Callers must check `IsValuePresent` before using `Value`.

### PackSizeResult

The V1 result retains:

```cpp
PackSizeStatus Status;
std::uint64_t SizeBytes;
```

`SizeBytes` is meaningful only when `Status == PackSizeStatus::Success`.

These are the only Rule-19 payload-access exceptions identified and accepted by this audit.

## Persistence compatibility exception

Both concrete filesystem providers retain:

```cpp
BindingConcurrency<TBindingProfile>()
```

with conservative fallback to:

```cpp
InvocationConcurrency::CallerSerialized
```

when a custom/older binding profile has no `Concurrency` member.

This fallback:

- never claims a stronger capability than can be proven;
- adds no lock;
- adds no allocation;
- adds no provider state;
- preserves source compatibility for existing custom profiles.

## Version lock

The current EDP-Localisation package/toolchain version remains:

```text
0.1.0
```

No version mutation is permitted by this workstream.

## Final validation result

The post-audit current-tip validation gate completed successfully on 2026-09-22.

Evidence supplied from the local validation run confirms:

- the guarded PlatformIO demo loop completed and execution continued into the getting-started workflow, proving every demo build in the loop returned success;
- the visible PlatformIO Arduino and ESP-IDF builds end in `[SUCCESS]`;
- `examples/getting-started/prepare.sh` completed `validate -> compile -> verify-generated` with `status: Success`;
- the getting-started PlatformIO application build completed successfully;
- the getting-started LittleFS `buildfs` target completed successfully;
- no `[FAILED]` marker appears in the captured post-audit output.

The supplied terminal capture begins part-way through a long ESP-IDF build, so the earliest Python/host-test summary lines are not retained in the pasted scrollback. The run nevertheless reached the guarded demo loop's completion and every subsequent required gate, with no failure marker in the captured output. The workstream accepts this as completion evidence for the final audit gate.

No additional hardware flash was required because the audit changes were non-semantic with respect to the already hardware-validated Localisation fallback and persisted-pack behavior.

**Audit result: COMPLETE.**

EDP-Localisation V1 is compliant with Policy V2 subject only to the explicit, narrow exceptions documented above. All library/toolchain versions remain locked at `0.1.0`.

## Primitive-introduction Type identity migration audit — 2026-09-24

The migration to the System-owned universal Type identity preserves the exact eight-byte EDPL representation while adding the locked 24-bit Authority / 40-bit authority-local validity semantics. Coverage now includes authoring rejection for both zero components, Resolver boundary rejection of invalid identities, and CRC-valid persisted-pack rejection for an invalid Type Authority. No dependency edge or released version changed.
