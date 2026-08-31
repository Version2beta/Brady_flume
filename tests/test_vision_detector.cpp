#include <array>
#include <cassert>
#include <cmath>
#include <cstdint>

#include "vision_detector.h"

namespace {

constexpr int kWidth = 120;
constexpr int kHeight = 200;
constexpr int kX1 = 40;
constexpr int kX2 = 100;

bool near(float actual, float expected)
{
    return std::fabs(actual - expected) < 0.0001f;
}

void set_high_contrast_stripe(std::array<uint8_t, kWidth * kHeight> *frame,
                              int y_center)
{
    for (int y = y_center - 2; y <= y_center + 2; ++y) {
        for (int x = kX1; x <= kX2; ++x) {
            (*frame)[y * kWidth + x] = x == kX1 ? 0 : 255;
        }
    }
}

void test_transition_levels()
{
    struct test_case_t {
        int high_contrast_stripes;
        float expected_head_ft;
        int expected_transition_row;
    };
    const test_case_t cases[] = {
        {0, 0.08f, 60},
        {1, 0.07f, 85},
        {2, 0.06f, 110},
        {3, 0.05f, 135},
        {4, 0.04f, 160},
    };
    const int stripe_rows[] = {60, 85, 110, 135};

    for (const test_case_t test_case : cases) {
        std::array<uint8_t, kWidth * kHeight> frame = {};
        for (int stripe = 0; stripe < test_case.high_contrast_stripes; ++stripe) {
            set_high_contrast_stripe(&frame, stripe_rows[stripe]);
        }

        vision::staff_mark_reading_t reading = {};
        assert(vision::detect_staff_mark(frame.data(), kWidth, kHeight, &reading));
        assert(near(reading.head_ft, test_case.expected_head_ft));
        assert(reading.transition_row == test_case.expected_transition_row);
        assert(reading.contrast_ratio >= 0.10f && reading.contrast_ratio <= 1.0f);
    }
}

void test_invalid_geometry_is_rejected()
{
    std::array<uint8_t, kWidth * kHeight> frame = {};
    vision::staff_mark_reading_t reading = {};

    assert(!vision::detect_staff_mark(nullptr, kWidth, kHeight, &reading));
    assert(!vision::detect_staff_mark(frame.data(), 100, kHeight, &reading));
    assert(!vision::detect_staff_mark(frame.data(), kWidth, 163, &reading));
    assert(!vision::detect_staff_mark(frame.data(), kWidth, kHeight, nullptr));
}

}  // namespace

int main()
{
    test_transition_levels();
    test_invalid_geometry_is_rejected();
    return 0;
}
