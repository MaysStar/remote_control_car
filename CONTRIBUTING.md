# Contributing

Create a feature branch and open a draft PR against master. Describe the problem,
resulting behavior, validation and limitations using the PR template. Keep changes
focused and review your diff before requesting review. Only MaysStar authorizes
readiness and merges; contributors must not push directly to master.

## C style and module boundaries

Use four spaces, no tabs, Allman braces and the repository .clang-format.
Use descriptive module-prefixed functions and one responsibility per function.
Separate declarations, validation and processing with blank lines. Always use
braces for conditionals. Name configurable limits and timeouts, including units
in identifiers; do not hide ordinary arithmetic behind meaningless macros.
Validate external inputs and public API arguments; document failure behavior.
Keep hardware details in BSP and avoid adding platform types to domain interfaces.
Do not use volatile as a replacement for synchronization.

## Local checks

Run `python3 scripts/check_host.py` for host regression tests with strict warnings
and UBSan. CI additionally enables AddressSanitizer.
Install `clang-format==22.1.5` with pip, then run
`python3 scripts/check_format.py` for the migrated files.
The explicit formatting manifest is incremental: add files when migrating them,
without reformatting unrelated legacy code in a functional change.

Record both firmware builds and physical tests separately. Host mutex stubs do
not validate FreeRTOS scheduling or electrical behavior. Firmware builds currently
require local ignored bsp_config.h files and an ESP-IDF/PlatformIO toolchain;
the host workflow does not certify firmware compilation or hardware safety.

## Reviews and merge policy

Address review comments and rerun checks after changes. Never mark tests passed
without execution. A checked template box is a declaration, not a test result.
CODEOWNERS identifies the reviewer; GitHub rulesets enforce access and required
checks. See docs/repository-governance.md for setup. Do not enable auto-merge.
