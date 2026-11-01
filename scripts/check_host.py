"""Compile and execute host safety tests without requiring ESP-IDF."""
import json
import os
from pathlib import Path
import subprocess
import sys
import tempfile

ROOT = Path(__file__).resolve().parents[1]


def main():
    compiler = os.environ.get("CC", "cc")
    sanitizers = os.environ.get("SANITIZERS", "undefined")
    flags = ["-std=c11", "-Wall", "-Wextra", "-Werror", "-g",
             f"-fsanitize={sanitizers}", "-fno-omit-frame-pointer",
             "-Itests/stubs", "-Iremote_car/app/app_header"]
    cases = json.loads((ROOT / "tests/host_tests.json").read_text())
    discovered = {str(path.relative_to(ROOT)) for path in (ROOT / "tests").rglob("test_*.c")}
    registered = {case[0] for case in cases.values()}
    if not discovered or discovered != registered:
        raise SystemExit(f"C test manifest mismatch: discovered={discovered}, registered={registered}")
    with tempfile.TemporaryDirectory(prefix="car-tests-") as directory:
        for name, sources in cases.items():
            executable = str(Path(directory) / name)
            subprocess.run([compiler, *flags, *sources, "-lm", "-o", executable],
                           cwd=ROOT, check=True)
            subprocess.run([executable], cwd=ROOT, check=True)
    subprocess.run([sys.executable, "-m", "unittest", "discover", "-s", "tests",
                    "-p", "test_*.py", "-v"], cwd=ROOT, check=True)


if __name__ == "__main__":
    main()
