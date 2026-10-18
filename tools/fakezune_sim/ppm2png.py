#!/usr/bin/env python3
"""Convert the PPM frames written by fakezune_sim into PNGs (needs Pillow).

Usage: ppm2png.py <frames-dir>
"""
import pathlib
import sys

from PIL import Image


def main() -> int:
    if len(sys.argv) != 2:
        print(__doc__)
        return 2
    for ppm in sorted(pathlib.Path(sys.argv[1]).glob("*.ppm")):
        Image.open(ppm).save(ppm.with_suffix(".png"))
        print(ppm.with_suffix(".png"))
    return 0


if __name__ == "__main__":
    sys.exit(main())
