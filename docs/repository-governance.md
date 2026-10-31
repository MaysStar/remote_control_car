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
   Formatting and Python syntax checks. Keep strict/up-to-date checks enabled.

Use separate rulesets so the owner's permission to update master does not bypass
quality checks. Zero approvals avoids preventing the owner from merging their own
PR; self-approval is not available. Ownership review is informational here.

Run the workflow once before selecting the required checks in the GitHub UI.
Verify exact check names and verify each imported rule targets master. Existing
rulesets also apply. The observed develop protection rule had an empty branch
include list and only deletion/force-push restrictions; it did not implement the
requested owner-only master policy.

The connected GitHub integration cannot edit administrative rules, so activation
must be performed in GitHub settings. Keep ProFunc at Write, without administrative
access. Contributors may open PRs and push feature branches; master updates require
MaysStar using a PR and passing the independent quality ruleset.

## Validation scope

CI runs host safety regression tests with ASan/UBSan, formatting for the explicit
migration manifest and Python syntax compilation. Python syntax is not integration
or security testing. Firmware build and hardware acceptance remain separate gates
recorded manually in the PR until reproducible non-secret build configuration is
available. No CI job deploys, flashes hardware or merges PRs.
