"""Build either firmware with non-secret CI configuration; never upload it."""
import argparse
from pathlib import Path
import shutil
import subprocess
import sys

ROOT = Path(__file__).resolve().parents[1]
PROJECTS = {"vehicle": "remote_car", "controller": "remote_car_user_control"}


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("project", choices=PROJECTS)
    args = parser.parse_args()
    project = ROOT / PROJECTS[args.project]
    config = project / "bsp/bsp_header/bsp_config.h"
    if config.exists():
        parser.error(f"Refusing to replace existing configuration: {config}")
    shutil.copyfile(ROOT / "ci/bsp_config.h", config)
    try:
        subprocess.run([sys.executable, "-m", "platformio", "run", "--project-dir", str(project)],
                       cwd=ROOT, check=True)
    finally:
        config.unlink(missing_ok=True)


if __name__ == "__main__":
    main()
