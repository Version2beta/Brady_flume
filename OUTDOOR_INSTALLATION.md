# Outdoor installation baseline — Utah, year round

This baseline applies to the Brady Ditch Parshall-flume monitor. It establishes the design requirements for year-round outdoor operation in Utah (-30 °C to +60 °C) across irrigation seasons (April–October) and winter shutdown.

---

## Non-Contact Dual Camera & Enclosure Weatherproofing

### 1. Main Controller Solar/Battery Enclosure (NEMA 4X / IP66)
- **Housing:** Gasketed NEMA 4X / IP66 fiberglass or polycarbonate enclosure in a light-reflective color with a top rain/snow shield.
- **Pressure Equalization:** Equipped with a **Gore Hydrophobic Breathable Vent Gland** to equalize internal pressure during rapid solar heating and cooling without allowing liquid water or dust ingress.
- **Internal Condensation Management:** Replaceable 10g silica gel desiccant packs inside the enclosure absorb internal humidity. Bottom-mounted drip loops and sealed terminal blocks prevent condensation from tracking along wiring.
- **Display Window:** UV-stable impact-resistant viewing window for the 4.2" E-Ink display.

### 2. Camera Head Nodes ($H_a$ Primary & $H_b$ Submergence Gauges)
- **Enclosures:** Compact IP67 polycarbonate enclosures ($50 \times 50 \times 35\text{ mm}$) with clear front viewing lids and silicone perimeter gaskets.
- **Internal PCB Protection:** All internal circuit boards (Seeed XIAO ESP32S3 and MAX3490 RS-422 transceiver) are coated with **MG Chemicals 422B Silicone Conformal Coating** to prevent shorts or corrosion from ambient humidity.
- **Anti-Fog Window Protection:** Mini 1-gram silica gel desiccant packets sealed inside each camera junction box prevent internal moisture from fogging the clear front lid during freezing Utah winter snaps.
- **Liquid-Tight Cable Entry:** IP68 PG7 compression cable glands seal the 4-wire shielded cable (12V Power + RS-422 pair) exiting the bottom wall of the camera box. External drip loops ensure rainwater falls away from the gland.

### 3. Shared Night Optics & Off-Axis IR Spotlight
- **CRITICAL OPTICAL REQUIREMENT:** Because staff gauge face paint is intentionally retroreflective, mounting an IR light source on the exact same axis as the camera lens causes a blinding white glare reflection that completely obliterates the black tick marks.
- **Single Shared Spotlight:** Because the $H_a$ primary staff gauge and $H_b$ submergence staff gauge are only 1 to 3 feet apart on the flume wall, a **single standalone IP67 850nm IR Spotlight** with a $60^\circ$ flood beam spread illuminates **both staff gauges simultaneously**.
- **Off-Axis Mounting Position:** The spotlight is mounted **2 to 3 feet horizontally off-axis (to the side of the flume)** relative to the cameras, aimed at a $15^\circ\text{ to }30^\circ$ angle toward the flume sidewall.
- **Gated Power Control:** The shared IR spotlight is switched via a MOSFET/relay output on the main controller, energized for only 2 seconds per 15-minute nighttime logging burst to conserve solar battery capacity.

### 4. Temperature Sensors & Radiation Shield
- **Air Temperature RTD:** Evolution Sensors P3A-S316-250-PX-3-PFXX-40-STWL 3-wire Class A Pt100 probe. The 3-inch 316 SS sheath is mounted in a stainless compression fitting below the enclosure, with the sensing tip housed inside a **6-plate ventilated solar radiation shield** to prevent direct sunlight from skewing air temperature readings.
- **Water Temperature RTD:** IP68 316L SS 3-wire Pt100 probe with polyurethane cable, anchored in the flume sidewall stream.

---

## Power, Grounding, and Surge Protection

- **Power Architecture:** 24V AGM/LiFePO4 battery charged via a properly sized solar panel and MPPT solar charge controller.
- **Circuit Protection:** Fused branches on all battery and solar lines, reverse-polarity protection, and regulated DC-DC conversion for logic power.
- **Surge & Lightning Protection:** Listed DIN-rail surge protectors on incoming solar and RS-422 field cables bonded to a copper grounding rod.
- **Shielding:** 4-wire shielded twisted-pair cables for RS-422 camera communication, grounded at the main enclosure end to prevent ground loops.

---

## Pre-Deployment Weatherproofing Verification Checklist

1. Verify conformal coating coverage on all camera node PCBs.
2. Confirm fresh silica desiccant packs are installed inside main enclosure and camera cylinders.
3. Check O-ring seals, PG7 cable gland compression, and external drip loops on all cables.
4. Perform an enclosure thermal/condensation trial in a cold chamber before field deployment.
