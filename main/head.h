#pragma once

#include <stdbool.h>
#include <stdint.h>

#include "installation_config.h"

#define HEAD_SENSOR_MIN_FT BRADY_HEAD_AT_4MA_FT
#define HEAD_SENSOR_MAX_FT BRADY_HEAD_AT_20MA_FT
#define HEAD_SENSOR_MIN_MA 4.0f
#define HEAD_SENSOR_MAX_MA 20.0f
#define DEMO_HEAD_MIN_FT BRADY_DEMO_HEAD_MIN_FT
#define DEMO_HEAD_MAX_FT BRADY_DEMO_HEAD_MAX_FT

// Converts the configured linear 4–20 mA transmitter signal to head. Inputs
// outside the transmitter range are rejected rather than silently clamped.
bool head_from_current_ma(float current_ma, float *head_ft);

// Converts a shunt-resistor voltage measurement to head. shunt_ohms is the
// actual resistance used to convert the 4–20 mA loop current to voltage.
bool head_from_shunt_voltage(float voltage_v, float shunt_ohms, float *head_ft);

// Demonstration waveform: 1.00 ft on each hour and 0.08 ft at :30, with
// linear ramps between those points. elapsed_seconds may be wall-clock epoch
// seconds or a monotonic elapsed-time value.
float demo_head_at_seconds(int64_t elapsed_seconds);
