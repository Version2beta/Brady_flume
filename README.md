# Brady Ditch flow monitor

## Project Overview

The Brady Ditch Flume Monitor is an automated, solar-powered water recorder for a Parshall flume. It uses an ESP32-S3 microcontroller and two outdoor cameras to read staff gauges without touching the water. By detecting water distortion directly on the printed gauge marks, the system filters out surface waves, calculates flow rates, and stores both CSV flow logs and visual audit photos for field verification.

## Key System Features

* **Non-Contact Camera Reading**  
  Reads depth directly from staff gauges without sensors in the water, avoiding mud, moss, or physical damage.

* **$0.01\text{ Foot}$ Accuracy**  
  Detects water refraction across printed gauge lines, snapping readings to exact $0.01\text{ foot}$ ($0.12\text{ inch}$) increments matching how a human reads the gauge.

* **Wave and Glint Filtering**  
  Applies digital filtering across camera bursts to eliminate surface ripples, splash, and sun glint before recording flow.

* **24/7 Night Vision**  
  Uses an off-axis 850nm IR spotlight to illuminate both staff gauges at night without causing retroreflective glare into the camera lenses.

* **Year-Round Weatherproofing**  
  Built for $-30^\circ\text{C to }+60^\circ\text{C}$ outdoor Utah weather with NEMA 4X enclosures, regulated power, hydrophobic vents, and sealed clear-lid camera boxes.

* **Plug-and-Play USB File Access**  
  Plugging a laptop into the monitor mounts its MicroSD card as a standard USB flash drive (`BRADY_FLUME/`), letting technicians drag and drop flow logs and audit photos.

---

## Documentation Index

* [`BOM-ESP32S3.md`](BOM-ESP32S3.md)  
  Complete bill of materials for the main solar enclosure controller, Seeed Studio XIAO ESP32S3 camera head nodes, 850nm IR spotlight, power supplies, and IP67 weatherproofing hardware.

* [`OUTDOOR_INSTALLATION.md`](OUTDOOR_INSTALLATION.md)  
  Year-round Utah installation baseline (-30 °C to +60 °C), NEMA 4X solar enclosure layout, Gore hydrophobic vent, central 24V-to-12V power distribution, 3-wire Pt100 RTD radiation shield, and the 3.5-foot cross-arm mounting geometry.

* [`VISION_ALGORITHM.md`](VISION_ALGORITHM.md)  
  Technical specification for the $0.01\text{ ft}$ Submerged Mark Distortion Transition algorithm, off-axis IR lighting physics rule, 5-stage C++ DSP pipeline (`BurstFilter`, `StageIIRFilter`, `VisionDSPPipeline`), and visual audit image retention strategy.

* [`HANDOFF.md`](HANDOFF.md)  
  Current system status, quick start build/flash commands, firmware module index, and hardware handoff details.

---

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

---

## Field Architecture

| Function | Recommended Approach |
| --- | --- |
| Water Head | Primary non-contact camera vision on ESP32-S3 using mark-distortion transition tracking and DSP burst filtering on $H_a$ staff gauge. |
| Flow Rate | Determine submergence from simultaneous upstream ($H_a$) and downstream ($H_b$) dual camera vision readings; apply certified free-flow limit and submerged-flow correction when required. |
| Temperature | 316L stainless Pt100 RTD, Class A, IP68 with 3-wire interface; second RTD in a 6-plate solar radiation shield for air temperature. |
| Historic Data | Timestamped CSV records and 15-min annotated JPEG audit photos in a wear-leveled MicroSD card / SPIFFS ring log. |
| Field Retrieval | **Plug-and-Play USB Mass Storage (MSC) Drive Volume:** Connecting a USB-C cable from a laptop mounts the MicroSD filesystem directly as a USB flash drive (`BRADY_FLUME/`), enabling drag-and-drop log and JPEG audit image downloads. |
| Status Display | 4.2" low-power E-Ink display showing head, flow, daily/annual volume, temperature, battery, and sensor health. |
| Solar Power | 24V AGM/LiFePO4 battery through a Mean Well 24V-to-12V industrial buck converter and MPPT solar charge controller. |

---

## Required Site Inputs Before Field Hardware/Flow Configuration

1. Parshall flume throat width, upstream staff-gauge datum, and the governing rating table/equation.
2. Certified rating table coefficients for $Q = C \cdot H^n$ in `main/installation_config.h`.
3. Battery capacity, solar panel wattage, and winter shading assumptions.

---

## Build Commands

```sh
source ~/esp/esp-idf/export.sh
idf.py set-target esp32s3
idf.py build
idf.py -p /dev/cu.usbmodem3101 flash
```
