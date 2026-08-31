# Staff-gauge machine-vision algorithm & DSP specification

## Executive Overview

The intended machine-vision subsystem uses **deterministic classical computer vision** and **digital signal processing (DSP)** on ESP32-S3 camera nodes. It is designed to perform non-contact stage measurement on a standard USGS Parshall-flume staff gauge without AI, cloud processing, or continuous high-power video streams.

> **Implementation status:** `main/vision_poc.cpp` is a fixed-corpus proof of concept. It processes 169 embedded 120 × 200 grayscale frames with hard-coded crop geometry and thresholds, then writes `/images/clean_reference.bmp` to SPIFFS. It does not implement camera capture, field commissioning, dual-camera operation, MicroSD/JPEG retention, USB MSC, or the camera-node protocol. The sections below describe the intended field algorithm and must not be read as claims of current firmware behavior.

---

## Target Algorithm: Submerged Mark Distortion Transition

Standard edge detection and solarization in water-only ROIs fail near turbulent flumes because surface foam, glint, and algae stains create false edge boundaries.

The primary algorithm operates directly on the printed staff marks:

1. **Undistorted Marks (Above Water)**  
   Tick marks above the waterline present crisp horizontal edges, constant high contrast, and zero spatial refraction.

2. **Distorted Marks (Submerged Under Water)**  
   Tick marks under moving water are optically refracted, blurred, and distorted by surface turbulence.

3. **Discrete $0.01\text{ ft}$ Grid Quantization**
   The algorithm evaluates tick mark contrast and horizontal Sobel edge gradients along the right edge of the staff gauge ($0.04, 0.05, 0.06, 0.07, 0.08\text{ ft}$). It locates the optical transition boundary between the lowest crisp mark and highest distorted mark and snaps the reading to the nearest discrete **$0.01\text{ ft}$** ($0.12\text{ inch}$) staff mark.

---

## Target Four-Stage DSP Pipeline (`main/vision_dsp.h` / `main/vision_dsp.cpp`)

```
[Camera 20-Frame Burst] 
       │
       ▼
┌──────────────────────────────────────────────────────────┐
│ STAGE 1: Frame Confidence Gate (Outlier Rejection)       │
│ Rejects frames with sun glint, splash, or bug occlusion  │
└──────────────────────────┬───────────────────────────────┘
                           │ Valid Frames
                           ▼
┌──────────────────────────────────────────────────────────┐
│ STAGE 2: Burst Order-Statistic Filter (α-Trimmed Mean)   │
│ Sorts N frames; trims top/bottom 20% wave crests/troughs │
└──────────────────────────┬───────────────────────────────┘
                           │ 1-Second Burst Result
                           ▼
┌──────────────────────────────────────────────────────────┐
│ STAGE 3: IIR Exponential Moving Average                  │
│ Smooths minute-to-minute hydrological flow transitions   │
└──────────────────────────┬───────────────────────────────┘
                           │ Continuous Stage Reading
                           ▼
┌──────────────────────────────────────────────────────────┐
│ STAGE 4: Rate-of-Change (dH/dt) Sanity Clamping          │
│ Blocks physical step jumps exceeding 0.10 ft / step      │
└──────────────────────────────────────────────────────────┘
```

1. **Frame Confidence Gate**  
   Discards individual frames if tick mark contrast drops below $C_{\text{min}} = 110$, preventing glare or splash spikes.

2. **Burst $\alpha$-Trimmed Mean & Median Filter**  
   Sorts valid frames in a 10- to 30-frame burst, trims the top/bottom 20% (wave crests/troughs), and averages the middle 60%. This eliminates 100% of surface ripple jitter with 50% breakdown immunity to outliers.

3. **IIR Exponential Moving Average (EMA)**  
   Smooths consecutive burst measurements over logging intervals:
   $$H_{\text{smooth}}[t] = 0.25 \cdot H_{\text{burst}}[t] + 0.75 \cdot H_{\text{smooth}}[t-1]$$

4. **Rate-of-Change ($dH/dt$) Clamping**  
   Rejects step changes exceeding physical flume limits ($|\Delta H| > 0.10\text{ ft/step}$) to block floating debris, leaves, or birds.

---

## ESP32-S3 Hardware Benchmark Observation

Firmware revision `ff2602b` was built, flashed, and run against its embedded 169-frame, 120 × 200 grayscale corpus on the documented ESP32-S3:

* **WCH UART console:** **$854.8\text{ Frames Per Second}$** ($1.170\text{ ms}$ per frame)
* **Native USB console (earlier observation):** **$1555.3\text{ Frames Per Second}$** ($0.643\text{ ms}$ per frame); this has not been rerun for `ff2602b`.
* **Level Distribution:**
  * **$0.08\text{ ft}$:** 116 frames (68%)
  * **$0.07\text{ ft}$:** 33 frames (19%)
  * **$0.06\text{ ft}$:** 16 frames (9%)
  * **$0.05\text{ ft}$:** 4 frames (2%)
  * **$0.04\text{ ft}$:** 0 frames (0%)
* **DSP Filtered Level Convergence:** **$\mathbf{0.08\text{ ft}}$ ($0.96\text{ inches}$)**
* **Visual Audit Output:** SPIFFS reported successful storage of the 24-bit RGB bitmap at `/images/clean_reference.bmp`.

Benchmark timing currently includes a progress log every ten frames, so console transport materially affects the result. These observations validate the complete fixed-corpus proof-of-concept loop, not algorithm-only throughput, live camera capture, or field measurement accuracy.

---

## Target Audit Image Selection

For each intended camera burst, retain the single frame whose calculated level matches the burst median and annotate it with the detected $0.01\text{ ft}$ transition. Storage, retention, CSV records, and USB retrieval are specified separately in [Intended Data and Audit Records](DATA_AND_AUDIT_RECORDS.md); none are current firmware features.

---

## Physical Optics & Nighttime Lighting Rule

**CRITICAL OPTICAL REQUIREMENT:**  
Because staff gauge paint is intentionally retroreflective, mounting the 850nm IR light source on the exact same axis as the camera lens causes a blinding white reflection that completely obliterates the black tick marks.

To ensure high-contrast night readings:
* Use a **standalone IP67 850nm IR Illuminator Spotlight** mounted **2–3 feet off-axis (to the side)** of the camera housing.
* This casts light at an angle, providing high-contrast black mark definition on the white gauge background without causing sensor-blinding retroreflective glare.
* The IR spotlight is gated via a GPIO relay/MOSFET for the 2-second burst period to preserve solar battery power.
