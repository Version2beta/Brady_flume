# Outdoor installation baseline — Utah, year round

This baseline applies to both controller paths. It is a design requirement, not a claim that an unselected part is suitable. Final ratings must be confirmed against the installation elevation, local temperature history, solar loading, enclosure location, and electrical code.

## Environmental and enclosure requirements

- Design electronics for an internal operating range of **-30 to +60 °C** at minimum; prefer individually rated **-40 to +85 °C** components. Direct solar loading can make a sealed enclosure substantially hotter than ambient.
- Use a gasketed **NEMA 4X / IP66 minimum** outdoor enclosure with a UV-stable, impact-resistant viewing window. Prefer a light-colored enclosure, shaded mounting, and a rain/snow-shedding orientation.
- Provide a pressure-equalizing hydrophobic vent, condensation strategy, drip loops, listed cable glands, strain relief, service loops, labeled terminals, and a lockable service disconnect.
- **Nighttime Camera Optics & IR Lighting Requirement:** Because staff gauge paint is intentionally retroreflective, mounting an IR light source on the exact same axis as the camera lens causes a blinding white glare reflection that obliterates the black tick marks. The 850nm IR illuminator spotlight must be a standalone IP67 unit mounted **2–3 feet off-axis (to the side)** from the camera housing. This casts light at an angle, delivering high contrast on the graduation marks without sensor-blinding retroreflective glare.
- A vented pressure-transmitter cable needs a dry, replaceable desiccant termination inside the enclosure. Do not seal its vent tube or allow condensation into it.
- The display and window must be qualified together. Standard-temperature e-paper is not acceptable for year-round deployment unless the enclosure thermal design keeps it within its specified range. Add a front-light only if night reading is required.

## Power, grounding, and surge protection

- Use a 24 V battery and an appropriately rated solar charge controller. Fuse each branch at its source; use reverse-polarity protection, a service disconnect, and regulated DC conversion for logic power.
- Use listed surge protection on incoming power and field cables. Bond enclosure, surge protectors, and the site grounding system according to the applicable electrical code; have the final grounding/lightning design reviewed by a qualified installer.
- Keep sensor wiring physically and electrically separated from switched power and communications. Use shielded cable where the sensor manufacturer recommends it, and terminate shields according to the selected input-module guidance to avoid ground loops.

## Measurement and records

- Select a 4–20 mA head transmitter with an environmental rating, accuracy, cable, venting method, and wetted materials appropriate to the site. Treat under-range/over-range loop current as a fault, not a water reading.
- Use 3-wire Pt100 assemblies. The water probe must be 316L and IP68 with rated cable and connector practice. The selected air probe is Evolution Sensors P3A-S316-250-PX-3-PFXX-40-STWL: retain its 3 in, 1/4 in 316 SS sheath in a compression fitting below the enclosure, place its sensing end in a ventilated radiation shield, and route its 40 in PFA pigtail through a cable grip.
- Install a downstream-level measurement point if free-flow submergence cannot be shown by site measurements. Preserve the prescribed `Ha` and `Hb` measurement locations in installation drawings.
- Provide RTC/time synchronization, monotonically sequenced records, nonvolatile recovery of totals, watchdog recovery, and a documented calibration/maintenance procedure. Flow accounting remains disabled until the certified flume rating and submergence treatment are entered and reviewed.

## Before field installation

1. Approve a wiring diagram, enclosure layout, BOM, and sensor calibration procedure.
2. Verify every selected part's temperature, ingress, UV, surge, and supply-voltage ratings.
3. Bench-test sensor faults, power interruption recovery, display readability, and data retention.
4. Perform an enclosure thermal/condensation trial through a representative Utah summer and winter condition before relying on readings for accounting.
