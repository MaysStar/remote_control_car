"""Check the explicit migration scope against the shared C formatting policy."""
from pathlib import Path
import subprocess

ROOT = Path(__file__).resolve().parents[1]


def main():
    paths = (ROOT / "scripts/format-files.txt").read_text().splitlines()
    files = [path for path in paths if path and not path.startswith("#")]
    subprocess.run(["clang-format", "--dry-run", "--Werror", *files],
                   cwd=ROOT, check=True)


if __name__ == "__main__":
    main()
