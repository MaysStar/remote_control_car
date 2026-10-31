"""Validate required PR declarations. Checkboxes do not replace CI test results."""
import json
import os
from pathlib import Path
import re
import sys
import urllib.request

REQUIRED = (
    "I reviewed the complete diff.",
    "Names, formatting and module boundaries follow CONTRIBUTING.md.",
    "Relevant invalid inputs, boundaries and failure paths are covered by unit tests.",
    "All host unit tests pass for this revision.",
    "Both firmware projects compile for this revision.",
    "Documentation and compatibility notes are up to date.",
    "No credentials or private configuration are included.",
)


def validate(body):
    # Ignore hidden text and fenced examples: they are not visible declarations.
    visible = re.sub(r"<!--.*?-->", "", body or "", flags=re.S)
    visible = re.sub(r"(?ms)^\s*(```|~~~).*?^\s*\1\s*$", "", visible)
    checks = re.findall(r"^\s*- \[([ xX])\] (.+?)\s*$", visible, re.M)
    errors = []
    for required in REQUIRED:
        matches = [state for state, text in checks if text == required]
        if len(matches) != 1 or matches[0].lower() != "x":
            errors.append(f"Required exactly once and checked: {required}")
    if any(state == " " for state, _ in checks):
        errors.append("All visible checklist items must be checked.")
    for heading in ("Purpose", "Changes", "Verification", "Compatibility and risks"):
        match = re.search(rf"(?m)^## {re.escape(heading)}\s*\n(.*?)(?=^## |\Z)",
                          visible, re.S | re.M)
        if match is None or not match.group(1).strip():
            errors.append(f"Missing or empty section: {heading}")
    return errors


def main():
    event = json.loads(Path(os.environ["GITHUB_EVENT_PATH"]).read_text())
    pr = event["pull_request"]
    # Fetch current text so a rerun cannot pass using an obsolete checked body.
    request = urllib.request.Request(
        pr["url"], headers={"Authorization": f"Bearer {os.environ['GITHUB_TOKEN']}",
                           "Accept": "application/vnd.github+json"})
    with urllib.request.urlopen(request, timeout=30) as response:
        current = json.load(response)
    if current["head"]["sha"] != pr["head"]["sha"]:
        raise SystemExit("PR changed during this run; use the checks for its latest commit.")
    errors = validate(current.get("body"))
    if errors:
        for error in errors:
            print(error, file=sys.stderr)
        raise SystemExit(1)
    print("PR description and checklist passed.")


if __name__ == "__main__":
    main()
