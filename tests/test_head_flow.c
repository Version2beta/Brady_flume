#include <assert.h>
#include <math.h>
#include <stddef.h>

#include "flow.h"
#include "head.h"

static void test_head_conversion(void)
{
    float head_ft = -1.0f;

    assert(head_from_current_ma(4.0f, &head_ft));
    assert(fabsf(head_ft - HEAD_SENSOR_MIN_FT) < 0.0001f);

    assert(head_from_current_ma(20.0f, &head_ft));
    assert(fabsf(head_ft - HEAD_SENSOR_MAX_FT) < 0.0001f);

    assert(!head_from_current_ma(3.99f, &head_ft));
    assert(!head_from_current_ma(20.01f, &head_ft));
    assert(!head_from_current_ma(NAN, &head_ft));
    assert(!head_from_current_ma(12.0f, NULL));

    assert(head_from_shunt_voltage(1.8f, 150.0f, &head_ft));
    assert(fabsf(head_ft - 0.5f) < 0.0001f);
    assert(!head_from_shunt_voltage(1.8f, 0.0f, &head_ft));
}

static void test_demo_head(void)
{
    assert(fabsf(demo_head_at_seconds(0) - DEMO_HEAD_MAX_FT) < 0.0001f);
    assert(fabsf(demo_head_at_seconds(1800) - DEMO_HEAD_MIN_FT) < 0.0001f);
    assert(fabsf(demo_head_at_seconds(3600) - DEMO_HEAD_MAX_FT) < 0.0001f);
    assert(fabsf(demo_head_at_seconds(-1800) - DEMO_HEAD_MIN_FT) < 0.0001f);
}

static void test_flow_conversion(void)
{
    const parshall_calibration_t calibration = {
        .coefficient_cfs = 2.0f,
        .exponent = 1.5f,
        .minimum_head_ft = 0.1f,
    };
    float flow_cfs = -1.0f;

    assert(flow_from_head(&calibration, 1.0f, &flow_cfs));
    assert(fabsf(flow_cfs - 2.0f) < 0.0001f);
    assert(!flow_from_head(&calibration, 0.09f, &flow_cfs));
    assert(!flow_from_head(NULL, 1.0f, &flow_cfs));
    assert(!flow_from_head(&calibration, 1.0f, NULL));

    const parshall_calibration_t disabled = {0};
    assert(!flow_from_head(&disabled, 1.0f, &flow_cfs));
    assert(fabsf(acre_feet_for_interval(1.0f, 43560) - 1.0f) < 0.0001f);
}

int main(void)
{
    test_head_conversion();
    test_demo_head();
    test_flow_conversion();
    return 0;
}
