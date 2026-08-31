#include "flow.h"

#include <math.h>
#include <stddef.h>

bool flow_from_head(const parshall_calibration_t *calibration,
                    float head_ft,
                    float *flow_cfs)
{
    if (calibration == NULL || flow_cfs == NULL ||
        calibration->coefficient_cfs <= 0.0f || calibration->exponent <= 0.0f ||
        head_ft < calibration->minimum_head_ft) {
        return false;
    }

    *flow_cfs = calibration->coefficient_cfs * powf(head_ft, calibration->exponent);
    return isfinite(*flow_cfs) && *flow_cfs >= 0.0f;
}

float acre_feet_for_interval(float flow_cfs, uint32_t interval_seconds)
{
    // 1 acre-foot = 43,560 cubic feet.
    return flow_cfs * (float)interval_seconds / 43560.0f;
}
