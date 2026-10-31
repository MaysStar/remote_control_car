# Repository governance

PRs remain drafts until MaysStar authorizes readiness. Only MaysStar merges.
The template and CODEOWNERS become effective for new PRs after merging to master.
They do not themselves restrict merge permission.

## Activate rules in GitHub

The files in .github/rulesets are import configurations, NOT automatically active
policy. In Settings > Rules > Rulesets use New ruleset > Import a ruleset to
import each JSON file, inspect it and save with enforcement Active.

1. master-owner-only: target refs/heads/master; Restrict updates; the only bypass
   actor is MaysStar (user ID 123299437), for pull requests only. Do not add Write,
   ProFunc, or a broad role to the bypass list.
2. master-quality-gates: target master, no bypass actors; forbid deletion and force
   pushes, require a PR and resolved review threads, and require Host safety,
   Formatting, Python syntax, Firmware (vehicle), Firmware (controller), PR checklist
   and Required checks. Keep strict/up-to-date checks enabled.

Use separate rulesets so the owner's permission to update master does not bypass
quality checks. Zero approvals avoids preventing the owner from merging their own
PR; self-approval is not available. Ownership review is informational here.

Run the workflow once before selecting the required checks in the GitHub UI.
Verify exact check names and verify each imported rule targets master. Existing
rulesets also apply. The observed develop protection rule targets the default branch but only blocks
deletion and force pushes. It does not require CI, a PR or owner-only merging.

The connected GitHub integration cannot edit administrative rules, so activation
must be performed in GitHub settings. Keep ProFunc at Write, without administrative
access. Contributors may open PRs and push feature branches; master updates require
MaysStar using a PR and passing the independent quality ruleset.

## Validation scope

Every C test matching tests/**/test_*.c must be registered in tests/host_tests.json;
CI fails if the manifest misses a test. All Python test_*.py files in tests are
run through unittest discovery. C tests use ASan/UBSan and strict compiler warnings.
Firmware matrix jobs compile and link both projects using PlatformIO 6.1.18 and
espressif32 6.12.0 (ESP-IDF 5.5), with build-only placeholders from ci/bsp_config.h.
CI never flashes hardware and produces no release firmware. Hardware tests are
not required by the checklist or these jobs.

The workflow runs only for PR events: there are no skipped push/dispatch checks
with the same names that could appear green for the same commit.

The PR checklist requires each exact template item once and checked, nonempty
sections and no additional unchecked items. Removing an item does not bypass it.
Hidden comments and fenced examples do not count. The current body is fetched
from GitHub; editing the PR retriggers CI. Checkboxes are author declarations;
actual unit tests and compilation are independently enforced by CI.

Required checks fails if any dependency fails, is cancelled or skipped. To block
merging, activate the updated quality ruleset with no bypass actors, including
for MaysStar. The owner-only rule is separate so it cannot bypass quality gates.
The new rules become enforceable only after an administrator activates them.

Workflow/checklist code is reviewed code. MaysStar must review changes to .github,
scripts and tests before merging; a green check does not prove an author has not
weakened the workflow in their branch. Only MaysStar should have merge authority.

## Required checks to select in Settings

| Check | What must pass |
| --- | --- |
| Host safety | Every registered C test and every discovered Python unit test |
| Formatting | All files in the formatting migration manifest |
| Python syntax | Compilation of Python sources without executing servers |
| Firmware (vehicle) | ESP32-S3 compile and link with dummy CI config |
| Firmware (controller) | ESP32 compile and link with dummy CI config |
| PR checklist | Required text and checked declarations in the live PR body |
| Required checks | Every job above reports success |

To activate manually: Settings > Rules > Rulesets > New ruleset > Import a ruleset;
import both JSON files from .github/rulesets, verify master is targeted and save
Active. The quality ruleset must have an empty bypass list. The existing rule
that only blocks deletion and force push is insufficient. A green pipeline does
not activate these settings automatically.
