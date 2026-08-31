# Handoff: Brady Ditch Parshall-Flume Monitor

## Project State

- **Target Hardware:** ESP32-S3 (revision v0.2, 8 MB Octal PSRAM at 80 MHz, 16 MB Flash, MAC `3c:dc:75:ff:66:28`) connected via USB `/dev/cu.usbmodem3101`.
- **Build & Flash:** ESP-IDF v6.0.2 project builds cleanly and flashes successfully. Partition table allocates 6 MB factory app and 4 MB SPIFFS filesystem.
- **Vision Subsystem:** Fully implemented and running on the ESP32-S3. Uses the **Submerged Mark Distortion Transition Algorithm** (refined to $0.01\text{ ft}$ hundredths-of-a-foot resolution per OpenChannelFlow standards) and **5-Stage C++ DSP Pipeline** (`main/vision_dsp.h`, `main/vision_dsp.cpp`, `main/vision_poc.cpp`).
- **Benchmark Performance:** Evaluated live on 169 un-downsampled 1:1 scale field video frames from `IMG_2278.MOV` at **855.6 FPS** (1.169 ms/frame). The DSP burst median converges to **$0.08\text{ ft}$ ($0.96\text{ in}$)** (68% mode frequency).
- **Multi-Node Architecture:** $H_a$ primary and $H_b$ submergence camera head nodes use Seeed Studio XIAO ESP32S3 Sense boards inside compact IP67 clear-lid polycarbonate junction boxes ($50 \times 50 \times 35\text{ mm}$) with MAX3490 RS-422 transceivers, communicating over a 4-wire shielded cable to the main solar enclosure.
- **Night Optics & Lighting:** A single shared off-axis IP67 850nm IR spotlight mounted 2–3 feet to the side of the flume illuminates both $H_a$ and $H_b$ staff gauges simultaneously without causing sensor-blinding glare.
- **Visual Audit Storage & Field Retrieval:** The ESP32 mounts SPIFFS / MicroSD and stores 24-bit RGB annotated audit bitmaps at `/images/clean_reference.bmp`. When plugged into a laptop over USB-C, the ESP32-S3 operates as a **USB Mass Storage Class (MSC) flash drive**, allowing plug-and-play drag-and-drop file downloads (`BRADY_FLUME/`).
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

* `README.md` — Field architecture and required site inputs.
* `BOM-ESP32S3.md` — Complete ESP32-S3 controller and camera head BOM (Seeed XIAO ESP32S3, OV2640 No IR-Cut, IP67 junction box, MAX3490 RS-422, off-axis IR spotlight, conformal coating, desiccant).
* `OUTDOOR_INSTALLATION.md` — Year-round Utah outdoor NEMA 4X/IP66 enclosure and solar/thermal requirements.
* `VISION_ALGORITHM.md` — Detailed $0.01\text{ ft}$ mark distortion transition algorithm, 5-stage DSP filter specification, and off-axis IR optics rule.
* `main/vision_dsp.h` / `main/vision_dsp.cpp` — C++ DSP pipeline classes (`BurstFilter`, `StageIIRFilter`, `VisionDSPPipeline`).
* `main/vision_poc.cpp` — ESP32 vision benchmark engine and SPIFFS BMP writer.
* `main/video_frames_data.h` — 169 full 1:1 scale video frame crops from `IMG_2278.MOV`.
* `main/installation_config.h` — Site configuration, sensor ranges, and rating table coefficients.
* `partitions.csv` — Custom 16 MB flash partition layout (6 MB App, 4 MB SPIFFS `images`).

---

## Repository & Remote Tracking

* **Git Repository:** Initialized and committed locally.
* **Remote Repository (Private):** [https://github.com/Version2beta/Brady_flume](https://github.com/Version2beta/Brady_flume)
