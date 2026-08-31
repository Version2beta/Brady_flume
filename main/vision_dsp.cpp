#include "vision_dsp.h"

#include <algorithm>
#include <cmath>

namespace dsp {

BurstFilter::BurstFilter() : total_count_(0), valid_count_(0) {
    reset();
}

void BurstFilter::reset() {
    total_count_ = 0;
    valid_count_ = 0;
    for (size_t i = 0; i < kMaxBurstFrames; ++i) {
        frames_[i] = {0.0f, 0.0f, false};
    }
}

bool BurstFilter::add_frame(float head_ft, float confidence, float min_confidence) {
    if (total_count_ >= kMaxBurstFrames || !std::isfinite(min_confidence) ||
        min_confidence < 0.0f || min_confidence > 1.0f) {
        return false;
    }

    const bool is_valid = std::isfinite(head_ft) && std::isfinite(confidence) &&
                          confidence >= min_confidence && confidence <= 1.0f;
    frames_[total_count_] = {
        .head_ft = head_ft,
        .confidence = confidence,
        .valid = is_valid
    };

    total_count_++;
    if (is_valid) {
        valid_count_++;
    }

    return is_valid;
}

float BurstFilter::get_median() const {
    if (valid_count_ == 0) {
        return 0.0f;
    }

    float valid_heads[kMaxBurstFrames];
    size_t count = 0;

    for (size_t i = 0; i < total_count_; ++i) {
        if (frames_[i].valid) {
            valid_heads[count++] = frames_[i].head_ft;
        }
    }

    std::sort(valid_heads, valid_heads + count);
    if (count % 2 == 1) {
        return valid_heads[count / 2];
    } else {
        return (valid_heads[(count / 2) - 1] + valid_heads[count / 2]) * 0.5f;
    }
}

float BurstFilter::get_alpha_trimmed_mean(float trim_ratio) const {
    if (valid_count_ == 0) {
        return 0.0f;
    }

    float valid_heads[kMaxBurstFrames];
    size_t count = 0;

    for (size_t i = 0; i < total_count_; ++i) {
        if (frames_[i].valid) {
            valid_heads[count++] = frames_[i].head_ft;
        }
    }

    if (count < 3) {
        // Not enough samples to trim; return simple mean
        float sum = 0.0f;
        for (size_t i = 0; i < count; ++i) sum += valid_heads[i];
        return sum / static_cast<float>(count);
    }

    std::sort(valid_heads, valid_heads + count);

    const size_t trim_count = static_cast<size_t>(std::floor(static_cast<float>(count) * trim_ratio));
    const size_t start_idx = trim_count;
    const size_t end_idx = count - trim_count;

    if (start_idx >= end_idx) {
        return valid_heads[count / 2];
    }

    float sum = 0.0f;
    size_t trimmed_samples = 0;
    for (size_t i = start_idx; i < end_idx; ++i) {
        sum += valid_heads[i];
        trimmed_samples++;
    }

    return sum / static_cast<float>(trimmed_samples);
}

StageIIRFilter::StageIIRFilter(float alpha, float initial_h)
    : alpha_(alpha), state_(initial_h), initialized_(false) {}

void StageIIRFilter::reset(float initial_h) {
    state_ = initial_h;
    initialized_ = false;
}

float StageIIRFilter::update(float new_sample) {
    if (!initialized_) {
        state_ = new_sample;
        initialized_ = true;
        return state_;
    }

    state_ = (alpha_ * new_sample) + ((1.0f - alpha_) * state_);
    return state_;
}

VisionDSPPipeline::VisionDSPPipeline(float iir_alpha, float max_dh_per_step)
    : iir_filter_(iir_alpha, 0.0f),
      max_dh_per_step_(max_dh_per_step),
      last_valid_head_(0.0f) {}

void VisionDSPPipeline::start_burst() {
    burst_filter_.reset();
}

void VisionDSPPipeline::add_frame_sample(float head_ft, float confidence) {
    burst_filter_.add_frame(head_ft, confidence);
}

VisionDSPPipeline::pipeline_output_t VisionDSPPipeline::finalize_burst() {
    pipeline_output_t out = {};
    out.total_frames = burst_filter_.total_count();
    out.valid_frames = burst_filter_.valid_count();

    if (out.valid_frames == 0) {
        out.burst_median_ft = last_valid_head_;
        out.burst_trimmed_mean_ft = last_valid_head_;
        out.smoothed_head_ft = iir_filter_.get_state();
        out.rate_alarm = true;
        return out;
    }

    out.burst_median_ft = burst_filter_.get_median();
    out.burst_trimmed_mean_ft = burst_filter_.get_alpha_trimmed_mean(0.20f);

    // Rate-of-change sanity check
    bool alarm = false;
    if (last_valid_head_ > 0.001f) {
        const float dh = std::abs(out.burst_trimmed_mean_ft - last_valid_head_);
        if (dh > max_dh_per_step_) {
            alarm = true;
        }
    }

    out.rate_alarm = alarm;

    if (!alarm) {
        out.smoothed_head_ft = iir_filter_.update(out.burst_trimmed_mean_ft);
        last_valid_head_ = out.smoothed_head_ft;
    } else {
        out.smoothed_head_ft = iir_filter_.get_state();
    }

    return out;
}

}  // namespace dsp
