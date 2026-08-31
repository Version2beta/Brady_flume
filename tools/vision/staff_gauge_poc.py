#!/usr/bin/env python3
"""Deterministic waterline and staff-gauge registration proof of concept.

The waterline is detected independently of the printed staff.  Horizontal staff
marks then provide per-frame pixels-per-inch calibration.  A commissioned
installation supplies the physical value and expected location of one reference
major mark; this avoids OCR and does not use AI.
"""

from __future__ import annotations

import argparse
from pathlib import Path

import cv2
import numpy as np

# Tuned only to the two supplied close-up images. These are image coordinates,
# not field settings. Commissioned camera geometry must replace them.
WATER_ROI = (320, 760, 160, 100)  # water texture only; deliberately excludes staff
STAFF_ROI = (480, 100, 210, 720)   # horizontal staff marks
# Commissioned physical datum for this reference image.  A field installation
# stores its own value and expected row after camera commissioning.
REFERENCE_MAJOR_INCHES = 1.0
REFERENCE_MAJOR_EXPECTED_Y = 777


def row_scores(gray: np.ndarray, roi: tuple[int, int, int, int]):
    x, y, width, height = roi
    crop = gray[y:y + height, x:x + width]
    clahe = cv2.createCLAHE(clipLimit=2.0, tileGridSize=(8, 8)).apply(crop)

    # Solarization is intentionally only one feature. A global threshold would
    # turn sun glint into a false waterline, so use a local adaptive threshold.
    solarized = cv2.adaptiveThreshold(
        clahe, 255, cv2.ADAPTIVE_THRESH_GAUSSIAN_C,
        cv2.THRESH_BINARY_INV, 31, 4,
    )

    # Blur horizontally before the vertical derivative. This favors a boundary
    # spanning the ROI over isolated ripple/rock texture.
    horizontal_smoothed = cv2.GaussianBlur(clahe, (31, 1), 0)
    edge = np.abs(cv2.Scharr(horizontal_smoothed, cv2.CV_32F, 0, 1))
    edge_score = edge.mean(axis=1)

    # Solarized-mask transitions provide a second, intentionally weak signal.
    solar_score = np.abs(np.diff(solarized.astype(np.float32), axis=0)).mean(axis=1)
    solar_score = np.pad(solar_score, (0, 1))

    def unit(values: np.ndarray) -> np.ndarray:
        lo, hi = np.percentile(values, (10, 99))
        return np.clip((values - lo) / max(hi - lo, 1e-6), 0, 1)

    score = 0.8 * unit(edge_score) + 0.2 * unit(solar_score)
    return crop, clahe, solarized, edge, score


def staff_major_marks(gray: np.ndarray) -> np.ndarray:
    """Return major-mark centers from long dark horizontal segments.

    The staff ROI deliberately excludes numerals.  Its threshold is based on
    horizontal dark-pixel extent, so texture and vertical staff edges do not
    become marks.  This is a geometry check, not character recognition.
    """
    x, y, width, height = STAFF_ROI
    roi = gray[y:y + height, x:x + width]
    dark_extent = (roi < 70).sum(axis=1)
    active = dark_extent >= 135
    groups: list[float] = []
    start = None
    for row, on in enumerate(np.append(active, False)):
        if on and start is None:
            start = row
        elif not on and start is not None:
            if row - start >= 3:
                weights = dark_extent[start:row].astype(np.float64)
                groups.append(y + (np.arange(start, row) * weights).sum() / weights.sum())
            start = None
    return np.asarray(groups)


def register_staff(gray: np.ndarray) -> tuple[np.ndarray, float, float]:
    marks = staff_major_marks(gray)
    if len(marks) < 3:
        raise ValueError("reject: fewer than three staff major marks")
    # The major mark nearest its commissioned expected location defines datum.
    reference_y = marks[np.argmin(np.abs(marks - REFERENCE_MAJOR_EXPECTED_Y))]
    spacing = np.diff(marks)
    # Reject obstruction/perspective failures rather than invent a reading.
    spacing = spacing[(spacing > 100) & (spacing < 220)]
    if len(spacing) < 2:
        raise ValueError("reject: inconsistent staff-mark spacing")
    pixels_per_inch = float(np.median(spacing))
    if np.max(np.abs(spacing - pixels_per_inch)) > 0.12 * pixels_per_inch:
        raise ValueError("reject: staff-mark spacing is not consistent")
    return marks, float(reference_y), pixels_per_inch


def annotate(path: Path, output_dir: Path) -> None:
    image = cv2.imread(str(path))
    if image is None:
        raise ValueError(f"cannot read {path}")
    gray = cv2.cvtColor(image, cv2.COLOR_BGR2GRAY)
    x, y, width, height = WATER_ROI
    crop, clahe, solarized, edge, score = row_scores(gray, WATER_ROI)

    # The candidate is a local maximum after mild vertical smoothing, avoiding
    # an isolated one-pixel edge. This remains a candidate, not a reading.
    smoothed = cv2.GaussianBlur(score.reshape(-1, 1), (1, 7), 0).ravel()
    row = int(np.argmax(smoothed))
    candidate_y = y + row
    marks, reference_y, pixels_per_inch = register_staff(gray)
    head_inches = REFERENCE_MAJOR_INCHES + (reference_y - candidate_y) / pixels_per_inch

    annotated = image.copy()
    sx, sy, sw, sh = STAFF_ROI
    cv2.rectangle(annotated, (x, y), (x + width, y + height), (0, 255, 255), 2)
    cv2.rectangle(annotated, (sx, sy), (sx + sw, sy + sh), (255, 0, 0), 2)
    for mark in marks:
        cv2.line(annotated, (sx, round(mark)), (sx + sw, round(mark)), (255, 0, 0), 1)
    cv2.line(annotated, (0, candidate_y), (image.shape[1] - 1, candidate_y), (0, 0, 255), 2)
    cv2.putText(annotated, f"Head {head_inches / 12:.3f} ft  |  {pixels_per_inch:.1f} px/in",
                (14, 42), cv2.FONT_HERSHEY_SIMPLEX, 0.65, (0, 0, 255), 2, cv2.LINE_AA)

    # Compose a diagnostic panel that makes failures reviewable.
    edge_display = cv2.normalize(edge, None, 0, 255, cv2.NORM_MINMAX).astype(np.uint8)
    panels = [crop, clahe, solarized, edge_display]
    panel = cv2.hconcat([cv2.cvtColor(p, cv2.COLOR_GRAY2BGR) for p in panels])
    scale = 180
    plot = np.full((height, scale, 3), 255, dtype=np.uint8)
    for i, value in enumerate(smoothed):
        cv2.line(plot, (0, i), (int(value * (scale - 1)), i), (0, 0, 0), 1)
    cv2.line(plot, (0, row), (scale - 1, row), (0, 0, 255), 2)
    panel = cv2.hconcat([panel, plot])
    cv2.line(panel, (0, row), (panel.shape[1] - 1, row), (0, 0, 255), 2)

    stem = path.stem
    cv2.imwrite(str(output_dir / f"{stem}_annotated.jpg"), annotated)
    cv2.imwrite(str(output_dir / f"{stem}_diagnostic.jpg"), panel)
    print(f"{path.name}: water row {candidate_y}; head {head_inches / 12:.3f} ft; "
          f"staff scale {pixels_per_inch:.1f} px/in")


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("images", nargs="+", type=Path)
    parser.add_argument("--output", type=Path, default=Path("build/vision-poc"))
    args = parser.parse_args()
    args.output.mkdir(parents=True, exist_ok=True)
    for image in args.images:
        annotate(image, args.output)


if __name__ == "__main__":
    main()
