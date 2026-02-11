#!/usr/bin/env python3
"""
Preprocess GIF textures into atlas PNG + metadata JSON.

Input:
  assets/textures/gif/*.gif

Output for each input foo.gif:
  foo.atlas.png
  foo.atlas.json
"""

from __future__ import annotations

import argparse
import json
from pathlib import Path
from typing import List

try:
    from PIL import Image, ImageSequence
except ImportError as exc:
    raise SystemExit(
        "Pillow is required. Install with: pip install pillow"
    ) from exc

RESAMPLE_BILINEAR = getattr(getattr(Image, "Resampling", Image), "BILINEAR")


def preprocess_gif(gif_path: Path) -> None:
    with Image.open(gif_path) as gif:
        frames: List[Image.Image] = []
        durations_ms: List[int] = []

        for frame in ImageSequence.Iterator(gif):
            rgba = frame.convert("RGBA")
            frames.append(rgba.copy())
            durations_ms.append(int(frame.info.get("duration", 0)))

    if not frames:
        print(f"[skip] No frames: {gif_path}")
        return

    width, height = frames[0].size
    frame_count = len(frames)
    atlas = Image.new("RGBA", (width, height * frame_count), (0, 0, 0, 0))

    for i, frame in enumerate(frames):
        if frame.size != (width, height):
            frame = frame.resize((width, height), RESAMPLE_BILINEAR)
        atlas.paste(frame, (0, i * height))

    stem_path = gif_path.with_suffix("")
    atlas_png = stem_path.with_name(stem_path.name + ".atlas.png")
    atlas_json = stem_path.with_name(stem_path.name + ".atlas.json")

    atlas.save(atlas_png)

    avg_duration_ms = 0.0
    if durations_ms:
        avg_duration_ms = sum(durations_ms) / float(len(durations_ms))
    fps = (1000.0 / avg_duration_ms) if avg_duration_ms > 0 else 0.0

    meta = {
        "source": gif_path.name,
        "atlas": atlas_png.name,
        "frameCount": frame_count,
        "frameWidth": width,
        "frameHeight": height,
        "fps": fps,
    }
    atlas_json.write_text(json.dumps(meta, indent=2), encoding="utf-8")
    print(f"[ok] {gif_path.name} -> {atlas_png.name} ({frame_count} frames)")


def main() -> None:
    parser = argparse.ArgumentParser(description="Preprocess GIF textures into atlas files.")
    parser.add_argument(
        "--dir",
        default="../assets/textures/gif",
        help="Directory containing GIF files (default: assets/textures/gif)",
    )
    args = parser.parse_args()

    root = Path(args.dir)
    if not root.exists():
        raise SystemExit(f"Directory not found: {root}")

    gif_files = sorted(root.rglob("*.gif"))
    if not gif_files:
        print(f"No GIF files found in: {root}")
        return

    for gif_path in gif_files:
        preprocess_gif(gif_path)


if __name__ == "__main__":
    main()
