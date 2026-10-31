"""Compile and execute host safety tests without requiring ESP-IDF."""
import os
from pathlib import Path
import subprocess
import tempfile

ROOT = Path(__file__).resolve().parents[1]


def main():
    compiler = os.environ.get("CC", "cc")
    sanitizers = os.environ.get("SANITIZERS", "undefined")
    flags = ["-std=c11", "-Wall", "-Wextra", "-Werror", "-g",
             f"-fsanitize={sanitizers}", "-fno-omit-frame-pointer",
             "-Itests/stubs", "-Iremote_car/app/app_header"]
    cases = {
        "parser": ["tests/test_command_parser.c"],
        "state": ["tests/test_command_state.c",
                  "remote_car/app/app_src/app_global_state.c"],
    }
    with tempfile.TemporaryDirectory(prefix="car-tests-") as directory:
        for name, sources in cases.items():
            executable = str(Path(directory) / name)
            subprocess.run([compiler, *flags, *sources, "-o", executable],
                           cwd=ROOT, check=True)
            subprocess.run([executable], cwd=ROOT, check=True)


if __name__ == "__main__":
    main()
