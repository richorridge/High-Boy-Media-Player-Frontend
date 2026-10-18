# Copyright (c) 2025 HIGH CODE LLC
#
# TentacleOS is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# TentacleOS is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with TentacleOS. If not, see <https://www.gnu.org/licenses/>.

import os
import sys
import glob
import math
import numpy as np
from PIL import Image


def find_coeffs(source_coords, target_coords):
    matrix = []
    for s, t in zip(source_coords, target_coords):
        matrix.append([t[0], t[1], 1, 0, 0, 0, -s[0]*t[0], -s[0]*t[1]])
        matrix.append([0, 0, 0, t[0], t[1], 1, -s[1]*t[0], -s[1]*t[1]])
    A = np.array(matrix, dtype=np.float64)
    B = np.array([coord for pair in source_coords for coord in pair], dtype=np.float64)
    return np.linalg.solve(A, B).tolist()


def rotate_y_3d(image, angle_deg):
    w, h = image.size
    angle = math.radians(angle_deg)
    f = w * 2.0

    new_corners = []
    for x, y in [(0, 0), (w, 0), (w, h), (0, h)]:
        xc = x - w / 2
        x_rot = xc * math.cos(angle)
        z_rot = xc * math.sin(angle)
        scale = f / (f + z_rot)
        x_proj = x_rot * scale + w / 2
        y_proj = (y - h / 2) * scale + h / 2
        new_corners.append((x_proj, y_proj))

    xs = [c[0] for c in new_corners]
    ys = [c[1] for c in new_corners]
    min_x, min_y = min(xs), min(ys)
    max_x, max_y = max(xs), max(ys)

    out_w = int(math.ceil(max_x - min_x))
    out_h = int(math.ceil(max_y - min_y))

    offset_corners = [(x - min_x, y - min_y) for x, y in new_corners]

    src = [(0, 0), (w, 0), (w, h), (0, h)]
    coeffs = find_coeffs(src, offset_corners)

    result = image.transform(
        (out_w, out_h),
        Image.PERSPECTIVE,
        coeffs,
        Image.BICUBIC,
        fillcolor=(0, 0, 0, 0),
    )
    return result


def process_frames(frames_dir):
    pattern = os.path.join(frames_dir, "*frame_0*")
    files = sorted(glob.glob(pattern))

    if not files:
        print(f"rotate3d: no frame_0 files found in {frames_dir}")
        return

    for filepath in files:
        filename = os.path.basename(filepath)
        img = Image.open(filepath).convert("RGBA")

        rotated1 = rotate_y_3d(img, 30)
        name1 = filename.replace("frame_0", "frame_1")
        rotated1.save(os.path.join(frames_dir, name1))
        print(f"rotate3d: {name1}")

        rotated2 = rotate_y_3d(img, -30)
        name2 = filename.replace("frame_0", "frame_2")
        rotated2.save(os.path.join(frames_dir, name2))
        print(f"rotate3d: {name2}")

    print(f"rotate3d: {len(files)} frames processed")


if __name__ == "__main__":
    if len(sys.argv) > 1:
        frames_dir = sys.argv[1]
    else:
        frames_dir = os.path.join(os.path.dirname(os.path.abspath(__file__)), "assets", "frames")

    process_frames(frames_dir)
