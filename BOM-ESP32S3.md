# Brady Ditch monitor — ESP32-S3 bill of materials

This BOM covers the ESP32-S3 controller path. The parallel [`BOM-P1AM-200.md`](BOM-P1AM-200.md) covers the P1AM-200 controller path. Both must meet the shared [outdoor installation baseline](OUTDOOR_INSTALLATION.md).

Status labels: **selected** = agreed design choice; **candidate** = appropriate class/part pending review; **TBD** = required information is missing.

| Function | Item / specification | Manufacturer / candidate | Status | Notes |
| --- | --- | --- | --- | --- |
| Controller | ESP32-S3 module on a purpose-built carrier PCB | Connected development board identifies the target; module/board exact part TBD | candidate | A development board is not the field controller. The carrier needs protected power, connectors, watchdog/reset access, and service programming. |
| Development connection | Native USB serial/JTAG | `/dev/cu.usbmodem3101` on this Mac | observed | Provide a protected, sealed service access method in the field enclosure. |
| Head simulation | Firmware waveform, 1.000 ft at :00 and 0.080 ft at :30 | `main/installation_config.h` | selected | Retain as a commissioning/demo mode after analog hardware is assembled. |
| Upstream vision camera | 5 MP / 2 MP camera module (e.g. OV5640 / OV2640) with M12 glass lens mount, IP67 housing, DVP/parallel interface | Omnivision OV5640 / OV2640 candidate | selected | Primary non-contact $H_a$ staff gauge vision reading using physical mark-distortion transition tracking and DSP burst filtering. |
| Downstream vision camera | Second camera module for Parshall throat submergence ($H_b$ staff gauge) determination | Omnivision OV5640 / OV2640 candidate | selected | Secondary non-contact $H_b$ camera for submergence monitoring; uses identical DSP burst pipeline. |
| Night IR Illuminator | Standalone IP67 850nm IR Illuminator Spotlight (Off-Axis Mounted) | 12V/24V 850nm IP67 IR Spotlight | selected | **CRITICAL OPTICS NOTE:** Because a staff gauge is intentionally retroreflective, mounting the 850nm IR light source on the exact same axis as the lens will cause a blinding white reflection that completely obliterates the black tick marks. You must use a separate, standalone IP67 850nm IR Illuminator Spotlight mounted 2–3 feet off to the side of the camera housing. This casts the light at an angle, providing deep contrast on the gauge increments without causing sensor-blinding glare. Gated via GPIO relay for 2-second bursts. |
| Loop input | Protected 4–20 mA receiver, 150 Ω 0.1% low-tempco shunt, RC filter, external precision ADC | ADC TBD | candidate | 4–20 mA becomes 0.60–3.00 V. Include a field-rated input protector and loop fault detection; do not use the ESP32 internal ADC. |
| Water temperature | 316L stainless Pt100 RTD, Class A or 1/3-DIN, IP68, polyurethane cable; protected 3-wire RTD front end | RTD front end TBD | candidate | Use 3-wire wiring and field-rated input protection. |
| Air temperature | 3-wire Class A Pt100, 1/4 in diameter × 3 in 316 SS sheath, 40 in PFA pigtail; ventilated radiation shield | Evolution Sensors P3A-S316-250-PX-3-PFXX-40-STWL | selected | Mount the sheath in a stainless compression fitting immediately below the enclosure, with the sensing end inside a radiation shield. Do not use a cable grip as the shaft clamp. |
| Display | 4.2 in monochrome E Ink display, 400 × 300 pixels, SPI controller board | Good Display GDEY042T81 or equivalent | candidate | Mount behind a UV-stable enclosure window. It must meet the shared temperature requirement, or the enclosure needs thermal control. |
| Enclosure | NEMA 4X / IP66 minimum outdoor enclosure, UV-resistant window, vent, and condensation management | TBD | TBD | Follow the shared outdoor baseline; select after enclosure thermal and wiring layout review. |
| Power | 24 V battery, properly sized solar charge controller, fused/surge-protected DC conversion | TBD | TBD | Follow the shared outdoor baseline. |

## ESP32-S3 rugged implementation

Use an ESP32-S3 module on a purpose-built, conformally coated carrier board rather than placing the development board in the field. The carrier must provide a protected 24 V-to-logic power path, transient-protected field inputs, external precision ADC and RTD front ends, locking connectors or terminal blocks, a hardware watchdog/reset path, and a service/programming arrangement that does not compromise enclosure sealing.

This design requires more custom analog, power, and surge engineering than the P1AM path. Keep sensor ranges, engineering units, display layout, record format, and flow-calculation test vectors identical between the two implementations.
