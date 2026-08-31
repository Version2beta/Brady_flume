#pragma once

/*
 * Brady Ditch installation configuration
 *
 * This file contains site and demonstration settings, not ESP-IDF or board
 * settings. Change it for a particular installation, review the values with
 * the installer, and rebuild the firmware. Keep the certified Parshall rating
 * out of this file until it has been supplied and independently verified.
 *
 * Head transmitter scaling
 * ------------------------
 * The transmitter is assumed linear: 4 mA represents HEAD_AT_4MA_FT and
 * 20 mA represents HEAD_AT_20MA_FT. The ADC driver will measure voltage
 * across the installed current shunt and pass that voltage and the measured
 * shunt resistance to head_from_shunt_voltage(). A reading outside 4–20 mA
 * is treated as invalid; it is not silently clamped.
 *
 * Demonstration waveform
 * ----------------------
 * The demonstrator uses a linear triangular waveform. DEMO_HEAD_MAX_FT is
 * emitted at :00, DEMO_HEAD_MIN_FT at :30, and the maximum again at the next
 * :00. It follows real hour boundaries only after an RTC or SNTP sets time.
 * Before that, its hour boundary is boot time.
 */

#define BRADY_HEAD_AT_4MA_FT 0.000f
#define BRADY_HEAD_AT_20MA_FT 1.000f

#define BRADY_DEMO_HEAD_MIN_FT 0.080f
#define BRADY_DEMO_HEAD_MAX_FT 1.000f
#define BRADY_DEMO_UPDATE_SECONDS 60
