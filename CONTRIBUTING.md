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

Compile both firmware targets with `python3 scripts/build_firmware.py vehicle`
and `python3 scripts/build_firmware.py controller` after installing PlatformIO
6.1.18. These commands use dummy configuration and refuse to replace a local
bsp_config.h. Do not flash their output. Hardware testing is outside the current
CI scope and is not a required checklist item.

All C test_*.c files require registration in tests/host_tests.json. All Python
test_*.py files in tests run automatically. Missing C registrations fail CI.

## Reviews and merge policy

Address review comments and rerun checks after changes. Never mark tests passed
without execution. A checked template box is a declaration, not a test result.
CODEOWNERS identifies the reviewer; GitHub rulesets enforce access and required
checks. See docs/repository-governance.md for setup. Do not enable auto-merge.
