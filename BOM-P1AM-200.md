# Brady Ditch monitor — ProductivityOpen P1AM-200 bill of materials

This BOM covers the P1AM-200 controller path. The parallel [`BOM-ESP32S3.md`](BOM-ESP32S3.md) covers the ESP32-S3 controller path. Both must meet the shared [outdoor installation baseline](OUTDOOR_INSTALLATION.md).

Status labels: **selected** = an agreed design choice; **candidate** = suitable class or proposed part, pending review; **TBD** = site or engineering input required. Do not substitute a candidate part without checking its data sheet, environmental rating, and installation requirements.

| Function | Item / specification | Manufacturer / candidate | Status | Notes |
| --- | --- | --- | --- | --- |
| Controller | ProductivityOpen P1AM-200 controller | AutomationDirect / FACTS Engineering | selected | Arduino-compatible SAMD51P20 controller. Its P1000 I/O communication pins are internal, leaving its shield interface free for non-I/O use. |
| Panel/backplane hardware | Productivity 1000 I/O modules with DIN-rail/panel-mounted power, terminals, and protection | AutomationDirect | selected | This is microcontroller-based equipment, not a PLC installation. The P1AM platform works with P1000 I/O modules. |
| Upstream head sensor | Non-contact ultrasonic level transmitter, 2-wire 4–20 mA, field configurable to 0.00–1.00 ft head | Flowline EchoPod DL10-00 | candidate | Budget target: **under $400** each before mount/protection; verify current price and data sheet before purchase. Mount above maximum water, outside its blanking zone, and protect it from impact and false echoes. |
| Downstream level sensor | Second non-contact ultrasonic level transmitter for Parshall submergence determination | Flowline EchoPod DL10-00 | candidate | Same budget target. Required if site measurements show the free-flow submergence limit can be exceeded. |
| Loop input | P1000 analog-input module with a directly supported 4–20 mA range | P1-04AD | selected | Prefer direct current input over a discrete shunt/ADC circuit. Verify input range, channel isolation, accuracy, diagnostics, and surge protection in the current data sheet before purchase. |
| Water temperature | 316L stainless Pt100 RTD, Class A or 1/3-DIN, IP68, polyurethane cable; 3-wire P1000 RTD input module | P1-04RTD | selected | Use 3-wire wiring; verify module accuracy and terminal arrangement in the current data sheet. |
| Air temperature | 3-wire Class A Pt100, 1/4 in diameter × 3 in 316 SS sheath, 40 in PFA pigtail; ventilated radiation shield | Evolution Sensors P3A-S316-250-PX-3-PFXX-40-STWL | selected | Mount the sheath in a stainless compression fitting immediately below the enclosure, with the sensing end inside a radiation shield. Do not use a cable grip as the shaft clamp. |
| Display | 4.2 in monochrome E Ink display, 400 × 300 pixels, SPI controller board | Good Display GDEY042T81 or equivalent | candidate | Mount behind a UV-stable enclosure window. It must meet the shared temperature requirement, or the enclosure needs thermal control. |
| Enclosure | NEMA 4X / IP66 minimum outdoor enclosure, UV-resistant window, vent, and condensation management | TBD | TBD | Follow the shared outdoor baseline; select after enclosure thermal and wiring layout review. |
| Power | 24 V battery, properly sized solar charge controller, fused/surge-protected DC conversion | TBD | TBD | Follow the shared outdoor baseline. |

## ProductivityOpen assessment

The ProductivityOpen P1AM is a microcontroller platform, not a PLC: it uses Arduino-compatible firmware and the P1AM library to access Productivity 1000 I/O. The P1AM-100 uses an SAMD21G18; the selected P1AM-200 uses an SAMD51P20. P1AM-200 keeps the P1000 I/O interface internal, whereas P1AM-100 consumes SPI pins 8–10 and A3/A4 for that interface.

The selected P1-04AD analog-input module and P1-04RTD RTD-input module provide the P1AM sensor interfaces. This permits direct industrial 4–20 mA input rather than ESP32 ADC conversion. Exact electrical ratings remain a data-sheet review item.

## P1AM-200 rugged implementation

Mount the P1AM-200, P1000 modules, power supply, terminal blocks, surge protection, and service disconnect on a labeled DIN rail or backplane inside the outdoor enclosure. Use the P1-04AD directly for 4–20 mA loops and P1-04RTD for Pt100s; do not add an unprotected microcontroller ADC path. Keep communications optional until its cable route and surge/isolation design are selected.

## Display recommendation

Start with the **Good Display GDEY042T81** 4.2-inch black-and-white E Ink module (400 × 300). It is small, inexpensive, reflective, and has enough resolution for large head/flow values plus smaller totals and alarm status. Mount it behind a clear, UV-stable polycarbonate or glass window in the outdoor enclosure; test readability through the actual window before field deployment.

E-paper is excellent in sun but does not emit light. Add a low-power front-light if readings are needed at night. Cold temperatures slow refreshes, so update only when values materially change or on a scheduled 15–60 minute interval.
