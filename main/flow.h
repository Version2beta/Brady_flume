#pragma once

#include <stdbool.h>
#include <stdint.h>

typedef struct {
    // Determined from the Parshall flume's certified geometry/calibration.
    float coefficient_cfs;
    float exponent;
    float minimum_head_ft;
} parshall_calibration_t;

typedef struct {
    int64_t timestamp_us;
    float head_ft;
    float water_temp_c;
    float air_temp_c;
    float flow_cfs;
} water_reading_t;

bool flow_from_head(const parshall_calibration_t *calibration,
                    float head_ft,
                    float *flow_cfs);

// Integrate one constant-flow sample interval into an annual total.
float acre_feet_for_interval(float flow_cfs, uint32_t interval_seconds);
