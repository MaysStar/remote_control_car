import json
from pathlib import Path
import sys
import unittest

sys.path.insert(0, str(Path(__file__).resolve().parents[1] / "scripts"))
import master_rules as rules


class Repository:
    def __init__(self):
        self.saved = {}
        self.writes = []
        self.user_id = rules.OWNER_ID
        self.inactive = False

    def __call__(self, path, *, payload=None, pages=False):
        if path == rules.ROOT:
            return {"full_name": rules.REPOSITORY, "owner": {"id": rules.OWNER_ID},
                    "permissions": {"admin": True}}
        if path == "user":
            return {"id": self.user_id}
        if payload is not None and path == rules.RULES:
            rule_id = len(self.saved) + 1
            self.saved[rule_id] = payload
            self.writes.append(payload["name"])
            return {"id": rule_id}
        if pages and path.startswith(rules.RULES + "?"):
            return [{"id": key, "name": rule["name"]} for key, rule in self.saved.items()]
        if pages and "/rules/branches/master?" in path:
            return [] if self.inactive else [
                {"type": rule["type"], "ruleset_id": key} for key, policy in self.saved.items()
                for rule in policy["rules"]]
        return self.saved[int(path.rsplit("/", 1)[1])]


class MasterRulesTests(unittest.TestCase):
    def setUp(self):
        self.api = Repository()
        self.policies = [json.loads((rules.CONFIG / f"{name}.json").read_text()) for name in rules.NAMES]

    def test_preview_and_repeat_apply(self):
        self.assertFalse(rules.configure(self.api, self.policies, False))
        self.assertEqual(self.api.writes, [])
        self.assertTrue(rules.configure(self.api, self.policies, True))
        self.assertTrue(rules.configure(self.api, self.policies, True))
        self.assertEqual(self.api.writes, list(rules.NAMES))

    def test_resume_partial_setup(self):
        self.api.saved[1] = self.policies[0]
        self.assertTrue(rules.configure(self.api, self.policies, True))
        self.assertEqual(self.api.writes, [rules.NAMES[1]])

    def test_drift_stops_before_creation(self):
        self.api.saved[1] = dict(self.policies[1], bypass_actors=[])
        with self.assertRaisesRegex(RuntimeError, "differs"):
            rules.configure(self.api, self.policies, True)
        self.assertEqual(self.api.writes, [])

    def test_wrong_account(self):
        self.api.user_id = 0
        with self.assertRaisesRegex(RuntimeError, "admin access"):
            rules.configure(self.api, self.policies, True)
        self.assertEqual(self.api.writes, [])

    def test_requires_effective_rules(self):
        self.api.inactive = True
        with self.assertRaisesRegex(RuntimeError, "effective"):
            rules.configure(self.api, self.policies, True)
