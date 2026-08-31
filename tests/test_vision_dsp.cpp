#include <cassert>
#include <cmath>
#include <limits>

#include "vision_dsp.h"

namespace {

bool near(float actual, float expected)
{
    return std::fabs(actual - expected) < 0.0001f;
}

void test_burst_filter()
{
    dsp::BurstFilter filter;
    assert(filter.total_count() == 0);
    assert(filter.valid_count() == 0);
    assert(near(filter.get_median(), 0.0f));

    assert(filter.add_frame(0.10f, 0.9f));
    assert(!filter.add_frame(9.0f, 0.1f));
    assert(filter.add_frame(0.20f, 0.9f));
    assert(filter.add_frame(0.30f, 0.9f));

    assert(filter.total_count() == 4);
    assert(filter.valid_count() == 3);
    assert(near(filter.get_median(), 0.20f));
    assert(near(filter.get_alpha_trimmed_mean(), 0.20f));
}

void test_burst_filter_rejects_invalid_samples()
{
    dsp::BurstFilter filter;

    assert(!filter.add_frame(std::numeric_limits<float>::quiet_NaN(), 0.9f));
    assert(!filter.add_frame(0.10f, std::numeric_limits<float>::infinity()));
    assert(!filter.add_frame(0.10f, 1.01f));
    assert(filter.total_count() == 3);
    assert(filter.valid_count() == 0);
    assert(!filter.add_frame(0.10f, 0.9f, -0.1f));
    assert(filter.total_count() == 3);
}

void test_iir_filter()
{
    dsp::StageIIRFilter filter(0.25f);

    assert(near(filter.update(0.20f), 0.20f));
    assert(near(filter.update(0.40f), 0.25f));
    filter.reset(0.0f);
    assert(near(filter.update(0.30f), 0.30f));
}

void test_pipeline_holds_last_valid_state()
{
    dsp::VisionDSPPipeline pipeline(0.25f, 0.10f);

    pipeline.start_burst();
    pipeline.add_frame_sample(0.20f, 0.9f);
    auto output = pipeline.finalize_burst();
    assert(!output.rate_alarm);
    assert(near(output.smoothed_head_ft, 0.20f));

    pipeline.start_burst();
    pipeline.add_frame_sample(0.50f, 0.9f);
    output = pipeline.finalize_burst();
    assert(output.rate_alarm);
    assert(near(output.smoothed_head_ft, 0.20f));

    pipeline.start_burst();
    pipeline.add_frame_sample(0.25f, 0.1f);
    output = pipeline.finalize_burst();
    assert(output.rate_alarm);
    assert(output.valid_frames == 0);
    assert(near(output.smoothed_head_ft, 0.20f));
}

}  // namespace

int main()
{
    test_burst_filter();
    test_burst_filter_rejects_invalid_samples();
    test_iir_filter();
    test_pipeline_holds_last_valid_state();
    return 0;
}
