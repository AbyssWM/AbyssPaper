"""Create tiny real media fixtures and test detection and legacy config ingestion.

Requires ffmpeg for fixture encoding; never creates a GUI or opens an audio device.
Artifacts remain inside the supplied build directory.
"""
import struct
import subprocess
import sys
import zlib
from pathlib import Path

binary, ffmpeg, output = sys.argv[1:]
directory = Path(output)
directory.mkdir(parents=True, exist_ok=True)


def chunk(kind, payload):
    return (struct.pack(">I", len(payload)) + kind + payload
            + struct.pack(">I", zlib.crc32(kind + payload)))


def png(animated=False):
    data = b"\x89PNG\r\n\x1a\n"
    data += chunk(b"IHDR", struct.pack(">IIBBBBB", 2, 2, 8, 6, 0, 0, 0))
    if animated:
        data += chunk(b"acTL", struct.pack(">II", 2, 0))
        data += chunk(b"fcTL", struct.pack(">IIIIIHHBB", 0, 2, 2, 0, 0, 1, 10, 0, 0))
    data += chunk(b"IDAT", zlib.compress((b"\x00" + b"\xff\x00\x00\xff" * 2) * 2))
    if animated:
        data += chunk(b"fcTL", struct.pack(">IIIIIHHBB", 1, 2, 2, 0, 0, 1, 10, 0, 0))
        data += chunk(b"fdAT", struct.pack(">I", 2) + zlib.compress((b"\x00" + b"\x00\xff\x00\xff" * 2) * 2))
    return data + chunk(b"IEND", b"")


(directory / "static.png").write_bytes(png())
(directory / "animated.png").write_bytes(png(True))
(directory / "unrelated.txt").write_text("this is neither an image nor a video\n")

for source, target, options in [
    ("static.png", "static.webp", ["-frames:v", "1", "-c:v", "libwebp"]),
    ("animated.png", "animated.webp", ["-c:v", "libwebp_anim", "-loop", "0"]),
    ("animated.png", "animated.gif", []),
    ("animated.png", "clip.mp4", ["-c:v", "mpeg4"]),
]:
    subprocess.run([ffmpeg, "-nostdin", "-v", "error", "-y", "-i", str(directory / source),
                    "-threads", "1", *options, str(directory / target)], check=True)

subprocess.run([binary, str(directory)], check=True)
