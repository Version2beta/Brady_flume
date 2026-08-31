#include "vision_detector.h"

#include <algorithm>
#include <cmath>

namespace vision {
namespace {

constexpr int kRightX1 = 40;
constexpr int kRightX2 = 100;
constexpr int kStripeHalfHeight = 2;
constexpr int kY08 = 60;
constexpr int kY07 = 85;
constexpr int kY06 = 110;
constexpr int kY05 = 135;
constexpr int kY04 = 160;
constexpr int kMinimumWidth = kRightX2 + 1;
constexpr int kMinimumHeight = kY04 + kStripeHalfHeight + 2;

void evaluate_stripe(const uint8_t *frame_crop, int width, int y_center,
                     float *contrast, float *sobel_mean)
{
    uint8_t min_value = 255;
    uint8_t max_value = 0;
    float sobel_sum = 0.0f;
    int count = 0;

    for (int y = y_center - kStripeHalfHeight;
         y <= y_center + kStripeHalfHeight; ++y) {
        for (int x = kRightX1; x <= kRightX2; ++x) {
            const uint8_t pixel = frame_crop[y * width + x];
            min_value = std::min(min_value, pixel);
            max_value = std::max(max_value, pixel);

            const int top_pixel = frame_crop[(y - 1) * width + x];
            const int bottom_pixel = frame_crop[(y + 1) * width + x];
            sobel_sum += static_cast<float>(std::abs(bottom_pixel - top_pixel));
            ++count;
        }
    }

    *contrast = static_cast<float>(max_value - min_value);
    *sobel_mean = sobel_sum / static_cast<float>(count);
}

}  // namespace

bool detect_staff_mark(const uint8_t *frame_crop, int width, int height,
                       staff_mark_reading_t *reading)
{
    if (frame_crop == nullptr || reading == nullptr || width < kMinimumWidth ||
        height < kMinimumHeight) {
        return false;
    }

    float c_08 = 0.0f;
    float sx_08 = 0.0f;
    float c_07 = 0.0f;
    float sx_07 = 0.0f;
    float c_06 = 0.0f;
    float sx_06 = 0.0f;
    float c_05 = 0.0f;
    float sx_05 = 0.0f;
    float c_04 = 0.0f;
    float sx_04 = 0.0f;

    evaluate_stripe(frame_crop, width, kY08, &c_08, &sx_08);
    evaluate_stripe(frame_crop, width, kY07, &c_07, &sx_07);
    evaluate_stripe(frame_crop, width, kY06, &c_06, &sx_06);
    evaluate_stripe(frame_crop, width, kY05, &c_05, &sx_05);
    evaluate_stripe(frame_crop, width, kY04, &c_04, &sx_04);

    float detected_head = 0.08f;
    int transition_y = kY08;
    float confidence = c_08 / 150.0f;

    if (c_08 < 110.0f) {
        detected_head = 0.08f;
        transition_y = kY08;
        confidence = c_08 / 150.0f;
    } else if (c_07 < 100.0f) {
        detected_head = 0.07f;
        transition_y = kY07;
        confidence = c_07 / 150.0f;
    } else if (c_06 < 110.0f) {
        detected_head = 0.06f;
        transition_y = kY06;
        confidence = c_06 / 150.0f;
    } else if (c_05 < 100.0f) {
        detected_head = 0.05f;
        transition_y = kY05;
        confidence = c_05 / 150.0f;
    } else {
        detected_head = 0.04f;
        transition_y = kY04;
        confidence = c_04 / 150.0f;
    }

    *reading = {
        .head_ft = detected_head,
        .transition_row = transition_y,
        .contrast_ratio = std::clamp(confidence, 0.10f, 1.0f),
    };
    return true;
}

}  // namespace vision
