"""Inspect the master rulesets, or create missing ones using the owner's gh login."""
import argparse
import json
from pathlib import Path
import subprocess
import sys

REPOSITORY = "MaysStar/remote_control_car"
OWNER_ID = 123299437
ROOT = f"repos/{REPOSITORY}"
RULES = f"{ROOT}/rulesets"
CONFIG = Path(__file__).resolve().parents[1] / ".github" / "rulesets"
NAMES = ("master-quality-gates", "master-owner-only")


def github(path, *, payload=None, pages=False):
    command = ["gh", "api", "--hostname", "github.com"]
    if pages:
        command += ["--paginate", "--slurp"]
    if payload is not None:
        command += ["--method", "POST", "--input", "-"]
    response = subprocess.run(command + [path], text=True, capture_output=True,
                              input=json.dumps(payload) if payload is not None else None)
    if response.returncode:
        raise RuntimeError(response.stderr.strip() or f"GitHub request failed: {path}")
    data = json.loads(response.stdout)
    return [item for page in data for item in page] if pages else data


def matches(expected, actual):
    """GitHub may add response fields and reorder lists. Never accept extra list items."""
    if isinstance(expected, dict):
        return isinstance(actual, dict) and all(
            key in actual and matches(value, actual[key]) for key, value in expected.items())
    if isinstance(expected, list):
        if not isinstance(actual, list) or len(expected) != len(actual):
            return False
        remaining = actual.copy()
        for item in expected:
            for index, candidate in enumerate(remaining):
                if matches(item, candidate):
                    remaining.pop(index)
                    break
            else:
                return False
        return True
    return type(expected) is type(actual) and expected == actual


def configure(api, policies, apply):
    repo = api(ROOT)
    if (repo["full_name"] != REPOSITORY or repo["owner"]["id"] != OWNER_ID or
            api("user")["id"] != OWNER_ID or not repo.get("permissions", {}).get("admin")):
        raise RuntimeError("Use the MaysStar GitHub CLI account with repository admin access.")

    existing = api(f"{RULES}?includes_parents=false&per_page=100", pages=True)
    missing = []
    verified = {}
    for policy in policies:
        name = policy["name"]
        found = [item for item in existing if item["name"] == name]
        if not found:
            missing.append(policy)
            print(f"Missing: {name}")
            continue
        if len(found) != 1:
            raise RuntimeError(f"Duplicate ruleset name: {name}")
        rule_id = found[0]["id"]
        if not matches(policy, api(f"{RULES}/{rule_id}")):
            raise RuntimeError(f"Existing {name} differs from the reviewed JSON. Inspect it in Settings.")
        verified[name] = rule_id

    if missing and not apply:
        print("Review the JSON files, then rerun with --apply to create the missing rulesets.")
        return False

    for policy in missing:
        name = policy["name"]
        rule_id = api(RULES, payload=policy)["id"]
        if not matches(policy, api(f"{RULES}/{rule_id}")):
            raise RuntimeError(f"Created {name}, but read-back differs. Inspect it in Settings.")
        verified[name] = rule_id
        print(f"Created and verified: {name}")

    effective = api(f"{ROOT}/rules/branches/master?per_page=100", pages=True)
    for policy in policies:
        name = policy["name"]
        active_types = {rule["type"] for rule in effective
                        if rule.get("ruleset_id") == verified[name]}
        if not all(rule["type"] in active_types for rule in policy["rules"]):
            raise RuntimeError(f"GitHub did not report all rules from {name} as effective on master.")
        print(f"Effective on master: {name}")
    return True


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--apply", action="store_true", help="Create missing rulesets")
    args = parser.parse_args()
    try:
        policies = [json.loads((CONFIG / f"{name}.json").read_text()) for name in NAMES]
        return 0 if configure(github, policies, args.apply) else 1
    except (OSError, RuntimeError, ValueError, KeyError) as error:
        print(error, file=sys.stderr)
        return 1


if __name__ == "__main__":
    sys.exit(main())
