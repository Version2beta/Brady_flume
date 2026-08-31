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
- **Unified Cross-Arm Mounting Bar:** Both camera junction boxes ($H_a$ and $H_b$) and the single IR spotlight are mounted on a **single 3.5-foot rigid cross-arm support bar** positioned **3.5 feet ($42\text{ inches}$) out from the flume sidewall**.

```
          TOP VIEW: 3.5-FOOT CROSS-ARM BAR MOUNTING GEOMETRY

       ┌──────────────────────────────────────────────────────────────┐
       │ 3.5-Foot Cross-Arm Support Bar (3.5 ft Out from Flume Wall)  │
       └───┬──────────────────────────┬───────────────────────────┬───┘
           │                          │                           │
           ▼                          ▼                           ▼
    ┌─────────────┐         ┌───────────────────┐          ┌─────────────┐
    │ Ha Cam Box  │         │ Single Center     │          │ Hb Cam Box  │
    │ (Upstream)  │         │ 850nm Spotlight   │          │(Downstream) │
    └──────┬──────┘         └─────────┬─────────┘          └──────┬──────┘
           │ 30° Off-Axis             │ 60° Flood Beam            │ 30° Off-Axis
           │ Line of Sight            │ Shines Left & Right       │ Line of Sight
           ▼                          ▼                           ▼
    ═══════╧══════════════════════════╧═══════════════════════════╧═════════
    FLUME: Ha Gauge               Flume Throat                 Hb Gauge
    ═══════════════════════════════════════════════════════════════════════
```

- **Flume Layout Geometry:**  
  * **$H_a$ Upstream Camera Box:** Mounted at the **left/upstream end** of the cross-arm bar, directly opposite the $H_a$ primary staff gauge.
  * **$H_b$ Downstream Camera Box:** Mounted at the **right/downstream end** of the cross-arm bar, directly opposite the $H_b$ throat submergence staff gauge.
  * **Center Shared IR Spotlight:** Mounted in the **CENTER of the cross-arm bar** (directly opposite the flume throat, between the two cameras).
- **Lens Specification:** Each camera uses an **$8.0\text{ mm}$ M12 glass lens** (or $12.0\text{ mm}$ telephoto). At $3.5\text{ feet}$ distance, the 1-foot staff gauge fills $70\%$ of the vertical image frame, yielding **14.0 vertical pixels per $0.01\text{ ft}$** ($0.12\text{ in}$ hundredths resolution).
- **Enclosures:** Compact IP67 polycarbonate enclosures ($50 \times 50 \times 35\text{ mm}$) with clear front viewing lids and silicone perimeter gaskets.
- **Internal PCB Protection:** All internal circuit boards (Seeed XIAO ESP32S3 and MAX3490 RS-422 transceiver) are coated with **MG Chemicals 422B Silicone Conformal Coating** to prevent shorts or corrosion from ambient humidity.
- **Anti-Fog Window Protection:** Mini 1-gram silica gel desiccant packets sealed inside each camera junction box prevent internal moisture from fogging the clear front lid during freezing Utah winter snaps.
- **Liquid-Tight Cable Entry:** IP68 PG7 compression cable glands seal the 4-wire shielded cable (12V Power + RS-422 pair) exiting the bottom wall of the camera box. External drip loops ensure rainwater falls away from the gland.

### 3. Center Shared Night Optics & Off-Axis IR Spotlight
- **CRITICAL OPTICAL REQUIREMENT:** Because staff gauge face paint is intentionally retroreflective, mounting an IR light source on the exact same axis as the camera lens causes a blinding white glare reflection that completely obliterates the black tick marks.
- **Center-Mounted Shared Spotlight:** Mounting the single IP67 850nm IR Spotlight in the **CENTER of the cross-arm bar** places it 1.5 to 2.0 feet off-axis from **both** the upstream $H_a$ camera and downstream $H_b$ camera simultaneously.
- **Symmetrical Off-Axis Illumination:** The $60^\circ$ flood beam shines outwards from the center, casting $25^\circ\text{ to }30^\circ$ off-axis light onto both the $H_a$ and $H_b$ staff gauge faces, providing maximum black mark contrast without causing retroreflective glare in either camera lens.
- **Gated Power Control:** The shared center IR spotlight is switched via a MOSFET/relay output on the main controller, energized for only 2 seconds per 15-minute nighttime logging burst to conserve solar battery capacity.

### 4. Temperature Sensors & Radiation Shield
- **Air Temperature RTD:** Evolution Sensors P3A-S316-250-PX-3-PFXX-40-STWL 3-wire Class A Pt100 probe. The 3-inch 316 SS sheath is mounted in a stainless compression fitting below the enclosure, with the sensing tip housed inside a **6-plate ventilated solar radiation shield** to prevent direct sunlight from skewing air temperature readings.
- **Water Temperature RTD:** IP68 316L SS 3-wire Pt100 probe with polyurethane cable, anchored in the flume sidewall stream.

---

## Power, Grounding, and Surge Protection

- **Solar Battery Bank:** 24V AGM or LiFePO4 battery bank charged via a 24V MPPT solar charge controller. Raw solar battery voltage fluctuates between $22.0\text{V}$ (discharged) and $29.2\text{V}$ (charging peak).
- **Central Regulated 24V-to-12V Industrial DC-DC Converter (Selected):**  
  A DIN-rail mounted industrial DC-DC buck converter (**Mean Well DDR-15G-12** or **Victron Orion 24/12-10**) sits immediately downstream of the battery fuses.
  * **Input:** $18\text{V to }36\text{V DC}$ (accepts raw solar battery fluctuation).
  * **Output:** Clean, short-circuit and surge-protected **$12.0\text{V DC}$ regulated power bus**.
  * **Distribution:** Supplies clean 12V DC power to the main ESP32-S3 controller board, E-Ink display, 3-wire RTD front ends, RS-422 bus, camera head nodes over the 6-conductor cable, and gated 850nm IR spotlight.
  * **Local Node Step-Down:** Each Seeed XIAO ESP32S3 camera head node steps down the clean 12V DC rail to $3.3\text{V DC}$ locally using its onboard buck regulator.
- **Circuit Protection:** Fused branches on all 24V and 12V lines, reverse-polarity diode protection, and thermal shutdown.
- **Surge & Lightning Protection:** Listed DIN-rail surge protectors on incoming solar and RS-422 field cables bonded to a copper grounding rod.
- **Cable Shielding:** 6-conductor (3 twisted-pair) 22 AWG outdoor shielded cable carries full-duplex RS-422 data pairs and clean 12V power to the cross-arm assembly, grounded at the main enclosure end to prevent ground loops.

---

## Measurement and Audit Records

- **Visual Audit Image Retention:** For every 15-minute logging burst, the ESP32 selects the single representative median frame, annotates it with a red waterline at the $0.01\text{ ft}$ mark, and saves it as a 15 KB JPEG (`/images/audit_YYYYMMDD_HHMMSS_0.08ft.jpg`).
- **Primary SD Card Storage ($32\text{ GB}$):** Retains over 100 years of visual audit photos (300 MB per 7-month irrigation season).
- **Internal Flash Fallback (4 MB SPIFFS):** FIFO ring buffer retaining the last 250 audit photos (~2.5 days) if no SD card is present.
- **Plug-and-Play USB Mass Storage (MSC) Retrieval:** When a technician plugs a USB-C cable from a laptop into the ESP32-S3, the MicroSD card mounts directly as a standard USB flash drive volume (`BRADY_FLUME/`). Technicians drag-and-drop CSV flow logs and JPEG audit photos onto their laptop without needing terminal commands or custom software.
- **Temperature Sensors:** Air temperature Pt100 RTD in a 6-plate solar radiation shield; water temperature Pt100 RTD in flume sidewall.
- **Time Synchronization & Flow Accounting:** RTC timekeeping with nonvolatile total recovery. Flow accounting remains disabled until certified Parshall rating coefficients are entered.

---

## Pre-Deployment Weatherproofing Verification Checklist

1. Verify conformal coating coverage on all camera node PCBs.
2. Confirm fresh silica desiccant packs are installed inside main enclosure and camera junction boxes.
3. Check silicone gasket seals, PG7 cable gland compression, and external drip loops on all cables.
4. Perform an enclosure thermal/condensation trial in a cold chamber before field deployment.
