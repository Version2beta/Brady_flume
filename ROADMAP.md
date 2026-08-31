# Brady Ditch Flume Monitor roadmap

This roadmap separates the validated proof of concept from the work required for a deployable, seasonal field monitor. It describes intended scope and sequencing; it is not a schedule.

## Current position

The repository contains a tested ESP32-S3 proof of concept for deterministic staff-gauge image analysis. The target field system is a solar-powered, dual-camera Parshall-flume monitor that records head, flow, temperature, device health, and audit evidence during the April–October irrigation season.

## Completed foundation

- Project builds as an ESP-IDF application for ESP32-S3, and the fixed-corpus vision loop has been run on the documented board.
- Vision detector and DSP components process the embedded 169-frame grayscale corpus; host tests cover detector, filtering, head conversion, and flow calculations.
- Flow conversion correctly remains disabled until certified Parshall calibration is supplied.
- The intended dual-camera, off-axis IR, RS-422, enclosure, RTD, storage, and field-retrieval architecture is documented.
- The preliminary seasonal power design is documented: three 25.6V/6Ah LiFePO4 modules in parallel, candidate 100W panel/MPPT, protected parallel bus, and required interconnect constraints.

## Remaining work

### 1. Close site and hardware decisions

- Obtain the flume throat width, staff-gauge datum, governing rating information, and independently verified calibration coefficients.
- Select and procure the main controller carrier, camera modules, RTD front ends, IR illuminator, display, MicroSD hardware, panel, MPPT controller, converter, protection hardware, and enclosure components.
- Confirm the battery manufacturer/BMS permits parallel operation and identify the actual VersaBlade connector pinout before building battery harnesses.
- Measure actual device power in sleep, acquisition, night-IR, storage, and telemetry modes; finalize converter size, wire sizes, fuse ratings, low-voltage settings, and panel/battery margin.

### 2. Build and validate the power and enclosure system

- Assemble the fused 24V battery bank, MPPT charging path, low-voltage protection, 24V-to-12V converter, and fused 12V distribution specified in [POWER_SYSTEM.md](POWER_SYSTEM.md).
- Verify LiFePO4 charging is inhibited below 0°C and that the intentional November–March shutdown preserves the battery.
- Complete grounding, surge protection, cable shielding, waterproofing, and strain relief.
- Perform thermal, condensation, and cold-weather enclosure testing before field deployment.

### 3. Implement field acquisition and node communication

- Develop firmware for both camera-head ESP32-S3 nodes: camera capture, locally commissioned crop geometry, burst processing, and health reporting.
- Define and implement the RS-422 protocol between camera heads and the main controller, including message validation, retries/timeouts, and sensor-fault reporting.
- Implement the main-controller drivers for RTC, battery measurement, RTDs, display, and the gated IR illuminator.
- Add intentional power-state management so the system sleeps between 15-minute records and powers the IR source only for nighttime capture bursts.

### 4. Commission and validate measurement behavior

- Commission camera placement, focus, exposure, gauge geometry, thresholds, confidence gates, and day/night IR performance at the actual flume.
- Collect representative field imagery across water conditions, sun angles, weather, and darkness; establish test fixtures from it.
- Validate upstream and downstream head accuracy against trusted manual readings, including confidence and failure behavior.
- Configure and independently check the certified Parshall flow calculation and submergence correction before enabling flow accounting.

### 5. Implement records, retrieval, and optional telemetry

- Implement timestamped CSV records, MicroSD storage, audit-image selection, retention, and failure handling.
- Implement and validate USB MSC service access without corrupting the recording filesystem.
- Add LTE-M telemetry only after local operation is reliable; define message schema, retry/backoff behavior, power budget, and offline retention.

### 6. Field acceptance and operations

- Run an end-to-end outdoor pilot through representative April–October conditions.
- Verify energy balance, autonomy, recovery after cloudy weather, measurement accuracy, image/audit retention, and technician retrieval.
- Document installation, commissioning, maintenance, seasonal shutdown/startup, troubleshooting, and data-quality procedures.

## Deployment gates

Do not deploy the monitor for operational flow accounting until all of these are true:

1. Battery parallel approval and connector pinout are confirmed, and the protected power system has passed bench testing.
2. The actual field load is measured and solar/battery sizing is confirmed for the installation site.
3. Camera capture, dual-node RS-422 communication, sensing, records, and retrieval work on target hardware.
4. Site-specific camera commissioning and head-accuracy validation are complete.
5. Certified flume calibration is configured and independently verified.
6. An end-to-end field pilot demonstrates reliable records and safe recovery from expected faults.

## Reference documents

- [README.md](README.md) — system overview and intended architecture.
- [BOM-ESP32S3.md](BOM-ESP32S3.md) — component choices and procurement status.
- [OUTDOOR_INSTALLATION.md](OUTDOOR_INSTALLATION.md) — field-installation baseline.
- [POWER_SYSTEM.md](POWER_SYSTEM.md) — preliminary battery, solar, protection, and interconnect design.
- [VISION_ALGORITHM.md](VISION_ALGORITHM.md) — intended measurement pipeline.
- [DATA_AND_AUDIT_RECORDS.md](DATA_AND_AUDIT_RECORDS.md) — intended record and retrieval behavior.
- [HANDOFF.md](HANDOFF.md) — current implementation and hardware-validation state.
