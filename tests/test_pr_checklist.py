import sys
from pathlib import Path
import unittest

sys.path.insert(0, str(Path(__file__).resolve().parents[1] / "scripts"))
from check_pr import REQUIRED, validate


def complete_body():
    sections = "\n".join(f"## {h}\nConcrete explanation.\n" for h in
                         ("Purpose", "Changes", "Verification", "Compatibility and risks"))
    return sections + "\n".join(f"- [x] {item}" for item in REQUIRED)


class ChecklistTests(unittest.TestCase):
    def test_complete(self):
        self.assertEqual(validate(complete_body()), [])

    def test_each_missing_or_unchecked_item_fails(self):
        for item in REQUIRED:
            for replacement in ("", f"- [ ] {item}"):
                with self.subTest(item=item, replacement=replacement):
                    self.assertTrue(validate(complete_body().replace(f"- [x] {item}", replacement)))

    def test_duplicate_does_not_satisfy_requirement(self):
        self.assertTrue(validate(complete_body() + f"\n- [x] {REQUIRED[0]}"))

    def test_hidden_checks_do_not_count(self):
        self.assertTrue(validate("<!--" + complete_body() + "-->"))
        self.assertTrue(validate("```markdown\n" + complete_body() + "\n```"))

    def test_empty_section_fails(self):
        self.assertTrue(validate(complete_body().replace("Concrete explanation.", "")))

    def test_extra_unchecked_item_fails(self):
        self.assertTrue(validate(complete_body() + "\n- [ ] More work"))
