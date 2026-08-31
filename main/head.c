#include "head.h"

#include <math.h>

bool head_from_current_ma(float current_ma, float *head_ft)
{
    if (head_ft == NULL || !isfinite(current_ma) ||
        HEAD_SENSOR_MAX_FT <= HEAD_SENSOR_MIN_FT ||
        current_ma < HEAD_SENSOR_MIN_MA || current_ma > HEAD_SENSOR_MAX_MA) {
        return false;
    }

    *head_ft = HEAD_SENSOR_MIN_FT +
               (current_ma - HEAD_SENSOR_MIN_MA) *
                   (HEAD_SENSOR_MAX_FT - HEAD_SENSOR_MIN_FT) /
                   (HEAD_SENSOR_MAX_MA - HEAD_SENSOR_MIN_MA);
    return true;
}

bool head_from_shunt_voltage(float voltage_v, float shunt_ohms, float *head_ft)
{
    if (!isfinite(voltage_v) || !isfinite(shunt_ohms) || shunt_ohms <= 0.0f) {
        return false;
    }

    return head_from_current_ma(voltage_v * 1000.0f / shunt_ohms, head_ft);
}

float demo_head_at_seconds(int64_t elapsed_seconds)
{
    int64_t seconds_into_hour = elapsed_seconds % 3600;
    if (seconds_into_hour < 0) {
        seconds_into_hour += 3600;
    }

    const float distance_from_half_hour =
        fabsf((float)seconds_into_hour - 1800.0f);
    return DEMO_HEAD_MIN_FT +
           (DEMO_HEAD_MAX_FT - DEMO_HEAD_MIN_FT) *
               distance_from_half_hour / 1800.0f;
}
