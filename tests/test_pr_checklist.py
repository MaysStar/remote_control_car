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

    def test_unclosed_and_long_fences_hide_examples(self):
        self.assertTrue(validate("```markdown\n" + complete_body()))
        self.assertTrue(validate("~~~~\n~~~\n" + complete_body() + "\n~~~~"))
        self.assertTrue(validate("<!-- unfinished\n" + complete_body()))

    def test_indented_code_is_not_a_declaration(self):
        self.assertTrue(validate(complete_body().replace("- [x]", "    - [x]")))

    def test_extra_markdown_bullets_must_be_checked(self):
        for bullet in ["*", "+"]:
            self.assertTrue(validate(complete_body() + f"\n{bullet} [ ] pending"))

    def test_comment_marker_in_example_does_not_hide_pending_task(self):
        self.assertTrue(validate(complete_body() + "\n```\n<!--\n```\n- [ ] pending"))

    def test_empty_section_fails(self):
        self.assertTrue(validate(complete_body().replace("Concrete explanation.", "")))

    def test_extra_unchecked_item_fails(self):
        self.assertTrue(validate(complete_body() + "\n- [ ] More work"))
