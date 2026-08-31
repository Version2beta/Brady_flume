# Seasonal power-system specification

This document specifies the preliminary low-voltage interconnect for the intended April–October field monitor. It is a design specification, not evidence of current firmware capability or a completed field installation.

## Design basis

- **Battery bank:** three identical 25.6V nominal, 6Ah LiFePO4 modules in parallel: 25.6V, 18Ah, 460.8Wh nominal.
- **Module limits:** 29.2V full, stated 20V discharged, 6A maximum charge, 15A maximum continuous discharge, integral BMS.
- **Solar source:** candidate 100W nominal 24V panel with MPPT controller.
- **Load:** candidate 24V-to-12V converter supplies the controller, camera heads, RTD front ends, display, and gated IR illuminator.
- **Preliminary budget:** 75Wh/day at the battery, with a design estimate of roughly 4.9 days of autonomy at 80% depth of discharge.

The final converter, IR illuminator, and camera-node power measurements are not yet available. The specified protection ratings below are suitable only for the preliminary sub-30W peak-load estimate; revise them from measured peak and steady-state currents before procurement.

## Required interconnect components

| Function | Minimum specification | Preliminary selection / quantity | Notes |
| --- | --- | --- | --- |
| Battery modules | 25.6V, 6Ah LiFePO4 module with integral BMS | 3 | Use only identical modules of the same age and state of charge. |
| Battery-side service connector | Molex VersaBlade 6-circuit housing | 35151-0610, Qty 3 | The recovered BatteryHookup connector part; confirm gender/keying against the physical pack. |
| Harness-side mating connector | Molex VersaBlade 6-circuit housing | 35150-0610, Qty 3 | Each pack needs one mating harness connector. |
| Harness terminals | 14-AWG female VersaBlade terminal | 35746-0210, quantity determined by confirmed power-pin count | Do not populate or connect unverified BMS, sense, or enable pins. |
| Terminal retention | VersaBlade retention lock | 35150-0390, Qty 3 | Install as required by the Molex system. |
| Battery branch leads | Stranded copper, red/black, 14 AWG, 60V or greater insulation rating | 3 matched positive/negative pairs | Keep all three pairs equal in gauge and length from pack connector through fuse to bus. |
| Battery branch fuse | DC-rated 5A fuse and holder | 3 | Install one in each positive lead, as close to its pack as practical. Fuse voltage and interrupt rating must be suitable for the actual BMS-limited fault current. |
| Positive and negative bus bars | Insulated, covered, 24V DC; at least 15A continuous and fault-rated for the selected fuses | 1 pair | Provide at least five terminations per polarity: three batteries, MPPT, and load converter. |
| Main load fuse | DC-rated 5A fuse and holder | 1 | Between positive bus and the selected 24V-to-12V converter. Recalculate if the measured load or selected converter permits more than the preliminary peak. |
| MPPT-to-bus fuse | DC-rated 10A fuse and holder | 1 | Place in the MPPT positive-output lead. Final rating must match the selected controller's maximum output and conductor ampacity. |
| Solar array disconnect | Outdoor DC-rated disconnect or breaker | 1 | Between panel and MPPT; voltage, current, and DC interruption rating must match the selected panel's maximum open-circuit voltage and short-circuit current. |
| 24V-to-12V converter | 18–36V input; 12V output; low quiescent current; output capacity above measured simultaneous peak | 1 | Do not use the 15W DDR-15G-12 unless measurements show the peak 12V load remains within 15W. |
| 12V distribution | Covered fused distribution block | 1 | Separate branches for main carrier, each camera cable, IR illuminator, display/RTD hardware, and future peripherals. Final branch fuses follow each device's measured wire and load requirement. |
| Low-voltage disconnect | Adjustable 24V battery-bus low-voltage cutoff with recovery threshold | 1 | Set from the battery supplier's recommended discharge limit, not from the 20V BMS-disconnect value. |
| Enclosure feedthroughs | Strain-relieved, IP-rated glands sized for each cable | as required | Maintain enclosure sealing and drip loops. |

## Battery-bank topology

```text
Pack 1 + -- 5A fuse --+
Pack 2 + -- 5A fuse --+-- covered +24V bus -- 5A load fuse -- 24V-to-12V converter -- fused 12V branches
Pack 3 + -- 5A fuse --+           |
                                     +-- 10A fuse -- MPPT charge-controller output

Pack 1 - ------------+
Pack 2 - ------------+-- covered 0V bus
Pack 3 - ------------+              |
                                     +-- MPPT negative and converter negative

Solar panel -- outdoor DC disconnect -- MPPT input
```

Only the two confirmed battery-power contacts of each pack connector may enter this topology. The 6-circuit VersaBlade housing does not establish a safe pinout by itself.

## Assembly and commissioning requirements

1. Obtain the battery/BMS manufacturer’s written confirmation that these modules may be paralleled. If it is unavailable, do not parallel the packs.
2. Determine the pack connector pinout from the physical pack or manufacturer documentation before making a mating harness. Confirm polarity and identify every non-power contact; leave non-power contacts isolated.
3. Bring all modules to the same voltage/state of charge before paralleling. Install the branch fuses before connecting a pack to the common bus.
4. Use the equal-length 14-AWG branch harnesses and covered bus bars. Never parallel packs by daisy-chaining one pack through another.
5. Select the MPPT charge profile for 8S LiFePO4: maximum charge voltage 29.2V and no charging below 0°C unless the battery has approved heating. Verify that the controller does not apply an equalization cycle.
6. Measure 24V bus current in sleep, daylight acquisition, night acquisition with IR, storage write, and any LTE-M transmit state. Use the results to confirm converter capacity and replace preliminary fuse values if necessary.
7. Verify the low-voltage cutoff and recovery settings under load. The BMS disconnect is a last-resort protection mechanism, not an operating control.

## Open procurement items

- Battery manufacturer and BMS parallel-operation approval.
- Confirmed VersaBlade connector pinout, contact gender, and number of battery-power contacts.
- Specific panel, MPPT controller, converter, bus bars, fuses, fuse holders, disconnect, and low-voltage disconnect.
- Measured device power, voltage drop for the existing camera cable, and final 12V branch fuse ratings.
