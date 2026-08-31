# Handoff: Brady Ditch Parshall-Flume Monitor

## Project State

- **Target Hardware:** ESP32-S3 (revision v0.2, 8 MB Octal PSRAM at 80 MHz, 16 MB Flash, MAC `3c:dc:75:ff:66:28`). Routine development uses the board's UART USB-C port through the WCH bridge at `/dev/cu.wchusbserial310`; the native USB port is reserved for USB/JTAG and planned MSC validation.
- **Build & Flash:** ESP-IDF v6.0.2 project builds cleanly and flashes successfully. Partition table allocates 6 MB factory app and 4 MB SPIFFS filesystem.
- **Vision Proof of Concept:** The current application processes the embedded 169-frame, 120 × 200 grayscale corpus using fixed crop geometry and attempts to write one annotated BMP to a pre-provisioned SPIFFS partition. It is not camera-capture, field-commissioned, or dual-camera firmware.
- **Latest Hardware Validation:** Firmware revision `ff2602b` was built, flashed, and monitored through the WCH UART bridge. It booted on the documented ESP32-S3, completed the embedded 169-frame benchmark at 854.8 FPS (1.170 ms/frame), reported writing `/images/clean_reference.bmp`, and left flow conversion disabled pending calibration. This validates the complete fixed-corpus proof-of-concept loop on hardware, including the extracted detector; it does not prove camera capture, field accuracy, or retrieval of the SPIFFS artifact. The earlier 1555.3-FPS native-USB console observation has not been rerun after these changes. Benchmark timing includes periodic console logging but excludes the subsequent BMP write, so it is not algorithm-only throughput.
- **Planned Multi-Node Architecture:** The BOM describes $H_a$ and $H_b$ ESP32-S3 camera nodes communicating with the main enclosure over RS-422. That protocol and camera-node firmware are not present in this repository.
- **Planned Field Services:** SD/CSV logging, USB MSC retrieval, RTC, display, and LTE-M telemetry are design goals; the current component list registers only `esp_timer`, `nvs_flash`, and SPIFFS.
- **Flow Engine:** `main/flow.c` provides $Q = C \cdot H^n$ conversion and cfs-to-acre-foot integration. Flow conversion remains disabled until certified Parshall rating coefficients are configured in firmware.

---

## Quick Start

```sh
cd ~/Development/Brady_flume
source ~/esp/esp-idf/export.sh
idf.py build
idf.py -p /dev/cu.wchusbserial310 flash
```

---

## Key Files & Modules

* `README.md` — Field architecture and required site inputs.
* `BOM-ESP32S3.md` — Complete ESP32-S3 controller and camera head BOM (Seeed XIAO ESP32S3, OV2640 No IR-Cut, IP67 junction box, MAX3490 RS-422, off-axis IR spotlight, conformal coating, desiccant).
* `OUTDOOR_INSTALLATION.md` — Year-round Utah outdoor NEMA 4X/IP66 enclosure and solar/thermal requirements.
* `DATA_AND_AUDIT_RECORDS.md` — Planned record, audit-image, retention, and USB retrieval workflow.
* `VISION_ALGORITHM.md` — Detailed $0.01\text{ ft}$ mark distortion transition algorithm, five-stage vision/DSP filter specification, and off-axis IR optics rule.
* `main/vision_detector.h` / `main/vision_detector.cpp` — Fixed-crop, 0.01-ft staff-mark transition detector with geometry validation.
* `main/vision_dsp.h` / `main/vision_dsp.cpp` — C++ DSP pipeline classes (`BurstFilter`, `StageIIRFilter`, `VisionDSPPipeline`).
* `main/vision_poc.cpp` — ESP32 vision benchmark orchestration and SPIFFS BMP writer.
* `main/video_frames_data.h` — 169 full 1:1 scale video frame crops from `IMG_2278.MOV`.
* `main/installation_config.h` — Site configuration and sensor ranges; certified flow calibration is still pending.
* `tests/run_host_tests.sh` — Host behavior tests for head and flow conversion, staff-mark detection, and DSP filtering.
* `partitions.csv` — Custom 16 MB flash partition layout (6 MB App, 4 MB SPIFFS `images`).

---

## Repository & Remote Tracking

* **Git Repository:** Initialized and committed locally.
* **Remote Repository (Private):** [https://github.com/Version2beta/Brady_flume](https://github.com/Version2beta/Brady_flume)
