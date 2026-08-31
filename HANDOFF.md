# Handoff: Brady Ditch Parshall-flume monitor

## Project state

- ESP-IDF v6.0.2 project targeting **ESP32-S3**.
- `idf.py build` passes.
- An ESP32-S3 (revision v0.2, 8 MB embedded octal PSRAM; MAC `3c:dc:75:ff:66:28`) was identified on `/dev/cu.usbmodem3101` and flashed for the embedded machine-vision proof of concept. PSRAM is enabled at 80 MHz in `sdkconfig.defaults` and passed its boot memory test.
- The initial firmware initializes NVS, contains the flow-conversion boundary in `main/flow.c`, and has no ADC driver yet.
- `main/head.c` converts a provisional linear 4–20 mA range (4 mA = 0.00 ft; 20 mA = 1.00 ft), including current-to-voltage shunt conversion.
- Sensor range, demo endpoints, and demo update period live in the documented `main/installation_config.h`, separately from ESP-IDF board/build settings in `sdkconfig`.
- The firmware also logs a demonstration head every minute: a linear 1.00 ft at :00 to 0.08 ft at :30 waveform. It follows hour boundaries after the clock is set; otherwise boot is its hour boundary.
- Flow conversion is intentionally disabled: its coefficients are zero until the certified Parshall-flume rating is supplied.

## Start here

```sh
cd ~/Development/Brady_flume
source ~/esp/esp-idf/export.sh
idf.py build
```

## Key files

- `README.md` — field architecture, safety notes, and required site inputs.
- `BOM-P1AM-200.md` — P1AM-200 controller-path BOM: P1-04AD direct-current input and P1-04RTD are selected. The proposed display is a Good Display GDEY042T81.
- `BOM-ESP32S3.md` — ESP32-S3 controller-path BOM. The two paths share sensor engineering units and test vectors but need separate firmware implementations.
- `OUTDOOR_INSTALLATION.md` — required year-round Utah design baseline: NEMA 4X/IP66 enclosure, thermal/condensation plan, 24 V power/surge protection, vented-transmitter care, and commissioning requirements.
- `main/flow.c` / `main/flow.h` — `Q = C × Hⁿ` conversion and cfs-to-acre-foot integration.
- `main/main.c` — current application skeleton; it does not erase NVS automatically.
- `main/vision_poc.cpp` / `main/vision_test_images.h` — deterministic C++ proof of concept using crops from the two supplied phone images. On the flashed ESP32-S3 it reports image rows 809 (first image) and 772 (second; known false edge). `main/vision_test_images.md` preserves source paths, SHA-256 hashes, crop geometry, and outcomes. `VISION_ALGORITHM.md` explains why this is diagnostic only.
- `sdkconfig` / `sdkconfig.defaults` — ESP32-S3 configuration.

## Next decisions

1. Obtain the Parshall flume throat width, upstream datum, certified rating table/equation, and whether submerged-flow correction is necessary.
2. Select the head sensor and interface. A vented pressure transmitter with 4–20 mA output and a protected external precision ADC is the recommended baseline.
3. Choose record interval/retention and display hardware, then implement sensor acquisition, RTC timekeeping, a wear-managed log, and deep-sleep power management.

Do not flash or erase a device until its board and serial port are explicitly identified.
