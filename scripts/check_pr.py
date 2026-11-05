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


def visible_markdown(body):
    lines = []
    fence = None
    in_comment = False
    for line in (body or "").splitlines():
        if fence is not None:
            marker, length = fence
            if re.fullmatch(rf" {{0,3}}{marker}{{{length},}}[ \t]*", line):
                fence = None
            continue
        if not in_comment and line.startswith(("    ", "\t")):
            continue
        opening = re.match(r"^ {0,3}(`{3,}|~{3,})", line)
        if opening and not in_comment:
            marker = opening.group(1)
            fence = marker[0], len(marker)
            continue

        visible = []
        while line:
            if in_comment:
                _, end, line = line.partition("-->")
                in_comment = not end
            else:
                before, start, line = line.partition("<!--")
                visible.append(before)
                in_comment = bool(start)
        lines.append("".join(visible))
    return "\n".join(lines)


def validate(body):
    visible = visible_markdown(body)
    checks = re.findall(r"^ {0,3}[-*+] \[([ xX])\] (.+?)[ \t]*$", visible, re.M)
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
