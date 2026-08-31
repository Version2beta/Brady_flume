# Brady Ditch flow monitor

A field monitor for the Brady Ditch Company diversion at the Parshall flume. Its job is to measure upstream head, convert it to flow using the *site's certified Parshall-flume rating*, retain readings and annual volume, measure temperature, and show the current status locally. [`BOM-P1AM-200.md`](BOM-P1AM-200.md) and [`BOM-ESP32S3.md`](BOM-ESP32S3.md) cover the two controller paths; [`OUTDOOR_INSTALLATION.md`](OUTDOOR_INSTALLATION.md) defines their shared year-round Utah installation baseline.

## Initial scope

This first buildable milestone establishes the ESP32-S3 firmware and a tested flow-integration boundary. A parallel ProductivityOpen P1AM-200 design uses Productivity 1000 I/O and will require a separate Arduino-compatible firmware implementation.

- `main/flow.c` converts head to flow with `Q = C × Hⁿ` and integrates cfs over time to acre-feet.
- `main/head.c` converts the selected linear transmitter range: 4 mA = 0.00 ft and 20 mA = 1.00 ft. It accepts either loop current or voltage plus the installed shunt resistance.
- A test/demo head generator ramps linearly from 1.00 ft on the hour to 0.08 ft at :30, then back to 1.00 ft on the next hour. Firmware logs it every minute. With valid RTC/SNTP time it follows civil-hour boundaries; until then it uses uptime, with boot treated as an hour boundary.
- Site and demonstration values live separately in the documented `main/installation_config.h`, keeping them apart from ESP-IDF board/build settings in `sdkconfig`. Update that file for an installation, then rebuild.

- The rating coefficients are deliberately disabled until the flume geometry and calibration are known.
- NVS is initialized, but no readings are yet stored. Do not use this firmware for operational accounting.

## System Architecture Diagram

```mermaid
graph TD
    subgraph Main_Solar_Enclosure [Main Solar Enclosure - NEMA 4X]
        Main_ESP32[ESP32-S3 Main Controller Node]
        Battery[24V AGM/LiFePO4 Solar Battery]
        Solar_MPPT[MPPT Solar Charge Controller]
        EPaper[4.2" E-Ink Status Display]
        Gore_Vent[Gore Hydrophobic Vent Gland]
        RTDs[3-Wire Pt100 Air & Water RTDs]
    end

    subgraph Cross_Arm_Assembly [3.5-Foot Rigid Cross-Arm Assembly - IP67]
        Ha_Cam[Ha Camera Box: Seeed XIAO ESP32S3 + OV2640 + MAX3490]
        Center_IR[Center IR Spotlight: Shared 850nm 60° Flood]
        Hb_Cam[Hb Camera Box: Seeed XIAO ESP32S3 + OV2640 + MAX3490]
    end

    Main_ESP32 <==>|6-Conductor Shielded Cable (RS-422 + 12V Power)| Ha_Cam
    Main_ESP32 <==>|6-Conductor Shielded Cable (RS-422 + 12V Power)| Hb_Cam
    Main_ESP32 --->|Gated 12V MOSFET Power| Center_IR
```

## Field architecture

| Function | Recommended approach |
| --- | --- |
| Water head | Primary non-contact camera vision on ESP32-S3 using mark-distortion transition tracking and DSP burst filtering on the $H_a$ staff gauge; optional vented 4–20 mA pressure/ultrasonic sensor fallback. |
| Flow | Determine submergence from simultaneous upstream ($H_a$) and downstream ($H_b$) dual camera vision readings; apply the certified free-flow limit and submerged-flow correction when required. |
| Temperature | 316L stainless Pt100 RTD, Class A or 1/3-DIN, IP68, with a 3-wire interface; second RTD for enclosure/air temperature. |
| Historic data | Timestamped records in a wear-leveled flash/SD ring log; store last reading and annual total separately in NVS. |
| Display | Small low-power e-paper display showing head, flow, daily/annual volume, temperature, battery, and sensor health. |
| Power | 24 V battery through a fused, reverse-polarity- and surge-protected buck converter. The solar panel must charge the battery through a suitable charge controller; it must not feed the ESP32 supply directly. |

The normal duty cycle should wake periodically, sample and log, refresh the display when needed, then deep-sleep. Keep the sensor supply and its warm-up time in the energy budget.

## Required site inputs before field hardware/flow configuration

1. Parshall flume throat width, upstream staff-gauge datum, and the governing rating table/equation.
2. Confirm the selected 0.00–1.00 ft transmitter range, required accuracy, and whether the flume can operate submerged.
3. Select the ADC and the precise current-to-voltage shunt resistance; the voltage conversion is `I_mA = 1000 × V / R_ohms`.
4. Sensor mounting method, cable length, and whether a vented pressure sensor is practical.
5. Sampling/reporting interval and retention period.
6. Battery chemistry/capacity, solar-panel size, enclosure location, and winter shading assumptions.
7. Display size/readability requirements and any telemetry requirement.

## Build

```sh
source ~/esp/esp-idf/export.sh
idf.py set-target esp32s3
idf.py build
```

Do not flash until a board and serial port have been explicitly identified.
