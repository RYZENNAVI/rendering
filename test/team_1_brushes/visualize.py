#!!! CHATGPT GENERATED - ONLY FOR VIEWING TEST RESULTS!!!

#!/usr/bin/env python3
import sys
import re
import matplotlib.pyplot as plt
from matplotlib.path import Path
import matplotlib.patches as patches

def parse_segments(text):
    """
    Extract cubic Bézier segments from input text.
    Returns list of (start, c1, c2, end) tuples.
    """
    segments = []
    lines = text.splitlines()
    for i, line in enumerate(lines):
        if line.strip().startswith("Bezier"):
            coords = []
            # Next four lines: start, c1, c2, end
            for j in range(1, 5):
                match = re.search(r'\(\s*(-?\d+\.?\d*)\s*,\s*(-?\d+\.?\d*)\s*\)', lines[i + j])
                if match:
                    coords.append((float(match.group(1)), float(match.group(2))))
            if len(coords) == 4:
                segments.append(tuple(coords))
    return segments

def plot_segments(segments):
    """
    Plot the Bézier segments with control points and labels.
    """
    verts, codes = [], []
    for start, c1, c2, end in segments:
        verts.append(start); codes.append(Path.MOVETO)
        verts.extend([c1, c2, end]); codes.extend([Path.CURVE4]*3)
    path = Path(verts, codes)

    fig, ax = plt.subplots()
    ax.add_patch(patches.PathPatch(path, facecolor='none', edgecolor=(1,0,0,1), lw=2))

    for i, (s, c1, c2, e) in enumerate(segments):
        xs, ys = zip(s, c1, c2, e)
        ax.scatter(xs, ys, marker='x')
        ax.text(s[0]+0.05, s[1]+0.05, str(i), fontweight='bold')

    all_x = [x for seg in segments for x,y in seg]
    all_y = [y for seg in segments for x,y in seg]
    ax.set_aspect('equal')
    ax.set_xlim(min(all_x)-0.5, max(all_x)+0.5)
    ax.set_ylim(min(all_y)-0.5, max(all_y)+0.5)

    ax.set_xlabel('X'); ax.set_ylabel('Y')
    ax.set_title('Brush Stroke Visualization')
    ax.grid(True)
    plt.show()

def main():
    print("Paste your Bézier data. When done, type a line with only 'END' and press Enter.")
    lines = []
    while True:
        try:
            line = input()
        except EOFError:
            break
        if line.strip() == "END":
            break
        lines.append(line)
    text = "\n".join(lines)
    segments = parse_segments(text)
    if not segments:
        print("No Bézier segments found in input.", file=sys.stderr)
        sys.exit(1)
    plot_segments(segments)

if __name__ == "__main__":
    main()
