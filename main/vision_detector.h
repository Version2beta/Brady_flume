#pragma once

#include <cstdint>

namespace vision {

struct staff_mark_reading_t {
    float head_ft;          // Snapped discrete 0.01 ft head value
    int transition_row;     // Image row of the distortion boundary
    float contrast_ratio;   // Crispness / confidence score
};

// Evaluates the fixed proof-of-concept crop geometry. Returns false when the
// supplied crop cannot safely contain every configured evaluation stripe.
bool detect_staff_mark(const uint8_t *frame_crop, int width, int height,
                       staff_mark_reading_t *reading);

}  // namespace vision
