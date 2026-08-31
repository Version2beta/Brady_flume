#pragma once

#include <cstddef>
#include <cstdint>

namespace dsp {

struct frame_reading_t {
    float head_ft;          // Snapped discrete 0.02 ft head value
    float confidence;       // Contrast / sharpness score (0.0 to 1.0)
    bool valid;             // Passed frame confidence gate
};

class BurstFilter {
public:
    static constexpr size_t kMaxBurstFrames = 32;

    BurstFilter();

    void reset();
    bool add_frame(float head_ft, float confidence, float min_confidence = 0.30f);

    size_t valid_count() const { return valid_count_; }
    size_t total_count() const { return total_count_; }

    float get_median() const;
    float get_alpha_trimmed_mean(float trim_ratio = 0.20f) const;

private:
    frame_reading_t frames_[kMaxBurstFrames];
    size_t total_count_;
    size_t valid_count_;
};

class StageIIRFilter {
public:
    explicit StageIIRFilter(float alpha = 0.25f, float initial_h = 0.0f);

    void reset(float initial_h = 0.0f);
    float update(float new_sample);
    float get_state() const { return state_; }

private:
    float alpha_;
    float state_;
    bool initialized_;
};

class VisionDSPPipeline {
public:
    VisionDSPPipeline(float iir_alpha = 0.25f, float max_dh_per_step = 0.10f);

    void start_burst();
    void add_frame_sample(float head_ft, float confidence);
    
    struct pipeline_output_t {
        float burst_trimmed_mean_ft;
        float burst_median_ft;
        float smoothed_head_ft;
        size_t valid_frames;
        size_t total_frames;
        bool rate_alarm;
    };

    pipeline_output_t finalize_burst();

private:
    BurstFilter burst_filter_;
    StageIIRFilter iir_filter_;
    float max_dh_per_step_;
    float last_valid_head_;
};

}  // namespace dsp
