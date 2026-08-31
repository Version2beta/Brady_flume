# Handoff: Brady Ditch Parshall-Flume Monitor

## Project State

- **Target Hardware:** ESP32-S3 (revision v0.2, 8 MB Octal PSRAM at 80 MHz, 16 MB Flash, MAC `3c:dc:75:ff:66:28`) connected via USB `/dev/cu.usbmodem3101`.
- **Build & Flash:** ESP-IDF v6.0.2 project builds cleanly and flashes successfully. Partition table allocates 6 MB factory app and 4 MB SPIFFS filesystem.
- **Vision Subsystem:** Fully implemented and running on the ESP32-S3. Uses the **Submerged Mark Distortion Transition Algorithm** and **5-Stage C++ DSP Pipeline** (`main/vision_dsp.h`, `main/vision_dsp.cpp`, `main/vision_poc.cpp`).
- **Benchmark Performance:** Evaluated live on 169 un-downsampled 1:1 scale field video frames from `IMG_2278.MOV` at **855.6 FPS** (1.169 ms/frame). The DSP burst median converges to **$0.08\text{ ft}$ ($0.96\text{ in}$)**.
- **Visual Audit Storage:** The ESP32 mounts SPIFFS and stores annotated RGB bitmaps at `/images/clean_reference.bmp`.
- **Flow Engine:** `main/flow.c` provides $Q = C \cdot H^n$ conversion and cfs-to-acre-foot integration. Flow conversion is held until certified Parshall rating coefficients are set in `main/installation_config.h`.

---

## Quick Start

```sh
cd ~/Development/Brady_flume
source ~/esp/esp-idf/export.sh
idf.py build
idf.py -p /dev/cu.usbmodem3101 flash
```

---

## Key Files & Modules

* `README.md` — Field architecture and site inputs.
* `BOM-ESP32S3.md` — ESP32-S3 controller BOM with dual-camera vision subsystem ($H_a$ primary staff gauge and $H_b$ submergence staff gauge).
* `BOM-P1AM-200.md` — ProductivityOpen P1AM-200 controller BOM.
* `OUTDOOR_INSTALLATION.md` — Shared year-round Utah outdoor NEMA 4X/IP66 enclosure and solar/thermal requirements.
* `VISION_ALGORITHM.md` — Detailed mark distortion transition algorithm and 5-stage DSP filter specification.
* `main/vision_dsp.h` / `main/vision_dsp.cpp` — C++ DSP pipeline classes (`BurstFilter`, `StageIIRFilter`, `VisionDSPPipeline`).
* `main/vision_poc.cpp` — ESP32 vision benchmark engine and SPIFFS BMP writer.
* `main/video_frames_data.h` — 169 full 1:1 scale video frame crops from `IMG_2278.MOV`.
* `main/installation_config.h` — Site configuration, sensor ranges, and rating table coefficients.
* `partitions.csv` — Custom 16 MB flash partition layout (6 MB App, 4 MB SPIFFS `images`).

---

## Repository & Remote Tracking

* **Git Repository:** Initialized and committed locally.
* **Remote Repository (Private):** [https://github.com/Version2beta/Brady_flume](https://github.com/Version2beta/Brady_flume)
