# Brady Ditch monitor — ESP32-S3 bill of materials

This BOM covers the ESP32-S3 controller path. The parallel [`BOM-P1AM-200.md`](BOM-P1AM-200.md) covers the P1AM-200 controller path. Both must meet the shared [outdoor installation baseline](OUTDOOR_INSTALLATION.md).

Status labels: **selected** = agreed design choice; **candidate** = appropriate class/part pending review; **TBD** = required information is missing.

| Function | Item / specification | Manufacturer / candidate | Status | Notes |
| --- | --- | --- | --- | --- |
| Main Controller | Main ESP32-S3 controller board in primary solar battery enclosure | Purpose-built ESP32-S3 carrier board | candidate | Handles main system clock, RTC timekeeping, 3-wire RTDs, flow rate integration, E-Ink display, SD logging, and RS-422 bus master communication. |
| Development connection | Native USB serial/JTAG | `/dev/cu.usbmodem3101` on this Mac | observed | Provide a protected, sealed service access method in the field enclosure. |
| Head simulation | Firmware waveform, 1.000 ft at :00 and 0.080 ft at :30 | `main/installation_config.h` | selected | Retain as a commissioning/demo mode after analog hardware is assembled. |
| Camera Head Microcontroller | Thumb-sized ESP32-S3 board ($21 \times 17.5\text{ mm}$) with 8 MB PSRAM | Seeed Studio XIAO ESP32S3 Sense | selected | Co-located inside IP67 camera head. Runs C++ vision algorithm and 5-stage DSP locally on frame bursts; connects to MAX3490 transceiver over UART. |
| Camera Sensor & Lens | OV2640 / OV5640 Night Vision (No IR-Cut) sensor + M12 6mm glass lens | OmniVision OV2640 / OV5640 No IR-Cut module | selected | 24-pin FPC connection to Seeed XIAO. M12 glass lens provides narrow $45^\circ$ FOV focused on staff gauge. |
| Camera Head Enclosure | Anodized Aluminum IP67 CCTV Camera Tube Enclosure ($30\text{ mm}$ ID) | Outdoor IP67 CCTV Mini Bullet Tube | selected | Heavy-duty waterproof housing with front optical glass window, rear O-ring end cap, and adjustable bracket. Holds Seeed XIAO, camera, and MAX3490 IC. |
| Board Moisture Protection | Silicone Conformal Coating | MG Chemicals 422B Silicone Conformal Coating | selected | Applied to Seeed XIAO board and MAX3490 transceiver PCB inside camera head to prevent corrosion/shorts from humidity. |
| Lens Anti-Fog Protection | Mini 1g Silica Gel Desiccant Packets | Sealed Silica Gel Desiccant Packets | selected | Sealed inside aluminum camera tube behind glass window to absorb internal air moisture and prevent cold-weather lens fogging. |
| Waterproof Cable Glands | IP68 PG7 Compression Cable Glands | IP68 PG7 Liquid-tight Cable Gland | selected | Mounts in rear endcap of camera tube. Seals 4-wire shielded cable (12V Power + RS-422 Pair) with external drip loop. |
| $H_a$ Camera Head Assembly | Primary upstream staff gauge camera head node | Integrated IP67 camera head node | selected | Mounts directly over $H_a$ staff gauge. Transmits computed level ($0.08\text{ ft}$) and JPEGs over RS-422 bus. |
| $H_b$ Submergence Camera Assembly | Secondary throat staff gauge camera head node | Integrated IP67 camera head node | selected | Identical camera head node mounted over downstream $H_b$ throat gauge for Parshall submergence detection over RS-422 bus. |
| RS-422 Communication Bus | 3.3V full-duplex RS-422 transceiver IC (MAX3490 / ADM3491) | MAX3490 / ADM3491 ICs | selected | Connects camera nodes to main controller over 4-wire shielded cable up to 4,000 feet at 1 Mbps. |
| Night IR Illuminators (Qty 2) | Dual Standalone IP67 850nm IR Illuminator Spotlights (Off-Axis Mounted) | 12V/24V 850nm IP67 IR Spotlight (Qty 2) | selected | **CRITICAL OPTICS NOTE:** Two spotlights required (one for $H_a$ primary gauge and one for $H_b$ submergence gauge). Because staff gauges are intentionally retroreflective, mounting an IR light source on the exact same axis as the camera lens causes a blinding white reflection that completely obliterates the black tick marks. Each spotlight must be mounted 2–3 feet off-axis (to the side) from its camera housing. Gated via GPIO MOSFET relay for 2-second bursts. |
| Loop input | Protected 4–20 mA receiver, 150 Ω 0.1% low-tempco shunt, RC filter, external precision ADC | ADC TBD | candidate | 4–20 mA becomes 0.60–3.00 V. Include a field-rated input protector and loop fault detection; do not use the ESP32 internal ADC. |
| Water temperature | 316L stainless Pt100 RTD, Class A or 1/3-DIN, IP68, polyurethane cable; protected 3-wire RTD front end | RTD front end TBD | candidate | Use 3-wire wiring and field-rated input protection. |
| Air temperature | 3-wire Class A Pt100, 1/4 in diameter × 3 in 316 SS sheath, 40 in PFA pigtail; ventilated radiation shield | Evolution Sensors P3A-S316-250-PX-3-PFXX-40-STWL | selected | Mount the sheath in a stainless compression fitting immediately below the enclosure, with the sensing end inside a radiation shield. Do not use a cable grip as the shaft clamp. |
| Display | 4.2 in monochrome E Ink display, 400 × 300 pixels, SPI controller board | Good Display GDEY042T81 or equivalent | candidate | Mount behind a UV-stable enclosure window. It must meet the shared temperature requirement, or the enclosure needs thermal control. |
| Enclosure | NEMA 4X / IP66 minimum outdoor enclosure, UV-resistant window, vent, and condensation management | Fibox / Hammond NEMA 4X fiberglass | selected | Follows shared outdoor baseline. Light-colored enclosure with top shade shield. |
| Hydrophobic Vent Gland | Gore Hydrophobic Breathable Vent Gland | Gore M12 Hydrophobic Vent | selected | Equalizes pressure inside main solar enclosure during temperature swings while blocking liquid water, snow, and dust ingress. |
| Radiation Shield | 6-Plate Ventilated Solar Radiation Shield | Standard 6-Plate Louvered Shield | selected | Houses the 3-inch 316 SS air RTD sheath below the main enclosure to prevent direct solar radiation from skewing air temperature logs. |
| Power | 24 V AGM/LiFePO4 battery, properly sized MPPT solar charge controller, fused/surge-protected DC-DC converter | Morningstar / Victron MPPT + 24V Battery | selected | Follows shared outdoor baseline. Powers main controller, 3-wire RTDs, RS-422 bus, and gated 2-second IR illuminator bursts. |

## ESP32-S3 rugged implementation

Use an ESP32-S3 module on a purpose-built, conformally coated carrier board rather than placing the development board in the field. The carrier must provide a protected 24 V-to-logic power path, transient-protected field inputs, external precision ADC and RTD front ends, locking connectors or terminal blocks, a hardware watchdog/reset path, and a service/programming arrangement that does not compromise enclosure sealing.

This design requires more custom analog, power, and surge engineering than the P1AM path. Keep sensor ranges, engineering units, display layout, record format, and flow-calculation test vectors identical between the two implementations.
