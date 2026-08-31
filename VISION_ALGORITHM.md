# Staff-gauge machine-vision study

## Conclusion

A single still photograph is not a sufficient primary reading method for this gauge. The supplied images show the problem: glare, ripples, dirt, a partially obscured gauge, and strong printed tick edges. A camera can still provide a useful independent, non-contact record if it uses a fixed view, a short video burst, calibrated staff geometry, and an explicit *no reading* result when confidence is poor.

The first implementation should be classical image processing, not a trained model. It is explainable, can be checked against manual readings, and provides the labeled image set needed if a segmentation model is later justified.

## Proof-of-concept result

`tools/vision/staff_gauge_poc.py` implements local solarization plus horizontally-smoothed vertical-edge scoring on the two supplied still images. It writes annotated and diagnostic images to `build/vision-poc/`. The original 210-pixel-wide crop improperly included printed staff ticks; image 2 selected a staff tick. The embedded ESP32 test crop is now limited to the adjacent 160-pixel water ROI. This is an expected and valuable negative result: solarization and edge detection alone are not adequate. The deployed algorithm must use burst-frame motion, separate gauge registration, multiple water ROIs, and rejection rules described below.

## Clean-gauge calibration reference

`testdata/clean_gauge_reference.png` is the user-supplied cleaned image (SHA-256 `238afa2468aa22610169d96cc63add5faa2bd5d913b109460686464a9b4dbd07`). Its major 1–5 inch tick centers provide the commissioning scale. A provisional linear fit gives row 809 ≈ **0.071 ft** and row 795 ≈ **0.078 ft**. These are calibration estimates only; production code must derive the registration from each camera frame rather than retain fixed phone-image rows.

## Solarization and edge detection

Solarization (thresholding then inverting bright portions) can make the dark staff marks visually conspicuous, but a single global solarization threshold is not reliable for water detection: sun glint can be brighter than the gauge on one frame and absent on the next. It should be one candidate feature after local illumination normalization, not the decision rule.

Likewise, Canny/Sobel edges alone will preferentially find the printed graduations rather than the water surface. Detect gauge ticks in a dedicated **gauge ROI** for calibration, but detect the waterline in separate **water ROIs** immediately beside the gauge. This separation is essential.

## Camera and view requirements

- Mount a fixed 4K camera on an independent post or structure; do not attach to or alter the flume.
- Frame only the useful staff region (initially 0.00–0.50 ft) at at least 20 vertical pixels per 0.01 ft.
- Use a low-distortion lens, fixed focus, and a shaded/hooded, cleanable window. Avoid automatic focus changes.
- Capture a 5–10 second burst of 20–50 frames. Use exposure bracketing if the camera supports it; preserve the original images.
- Lock the camera geometry after calibration. A polarizer may reduce some water glare, but test it on site because its benefit changes with sun angle.

## Algorithm

### 1. Image-quality gate

Reject the burst before producing a number when it is dark, blurred, blocked, heavily snow/dirt covered, or saturated by glare. Useful measures include sharpness (Laplacian variance), saturated-pixel fraction, exposure histogram, and agreement between burst frames.

### 2. Gauge registration and scale

1. Crop the fixed gauge ROI.
2. Correct lens distortion and perspective using a commissioning calibration image.
3. Normalize local luminance (for example, CLAHE on the Lab L channel).
4. Produce several binary candidates: adaptive threshold, local solarization at multiple thresholds, and a dark-mark mask.
5. Find the regular horizontal tick pattern using horizontal line filtering and robust periodic-spacing estimation.
6. Fit image row to staff value using RANSAC. Anchor the fit to manually confirmed major ticks; do not depend on OCR of dirty numerals on every frame.

This step maps any image row to head in feet and provides a registration-confidence score.

### 3. Waterline candidates

Use two or more narrow ROIs adjacent to the gauge, excluding the printed scale. For each ROI and frame, calculate:

- local horizontal edge energy (Scharr or Canny),
- vertical color/luminance change after local normalization,
- local solarized-mask transition,
- temporal variance and optical-flow magnitude across the burst.

Water has ripple/glint motion while the gauge and bank are stationary. Combine the row-wise feature scores over the burst. Find near-horizontal edge segments, then fit a single line across the independent water ROIs with RANSAC. The fitted row, not an isolated bright edge, is the proposed surface.

### 4. Decision and confidence

Convert the fitted row through the staff calibration. Publish a reading only when:

- registration is stable,
- at least two water ROIs agree,
- burst-frame estimates have low spread,
- the line is geometrically plausible, and
- the image-quality gate passes.

Otherwise record `invalid_image`, retain the images, and request a manual reading. Never carry forward the last good value as though it were a new observation.

## Validation plan

1. Collect at least 200 time-stamped bursts through morning, noon, evening, wind, clear water, turbid water, and partial shade.
2. Record an independent manual staff reading for each validation burst, ideally to 0.01 ft.
3. Tune thresholds only on a development subset; evaluate error and rejection rate on withheld images.
4. Require documented performance at the relevant low range (0.00–0.10 ft), where the supplied images place the water surface.
5. If classical features do not meet the required error/rejection rate, label the accepted/rejected images and train a small water-vs-not-water segmentation model. Preserve the same calibration, confidence, and manual-audit requirements.

## References for later implementation

- Eltner et al., *Water Resources Research* 57 (2021), DOI: `10.1029/2020WR027608`, on automated camera-based water-stage measurement.
- The implementation should use maintained computer-vision libraries (OpenCV or equivalent) for calibration, CLAHE, Scharr/Canny filtering, optical flow, Hough/RANSAC fitting, and image archival.
