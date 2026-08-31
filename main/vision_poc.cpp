#include "vision_poc.h"

#include <algorithm>
#include <cmath>
#include <cstdint>
#include <cstdio>
#include <cstdlib>

#include "esp_log.h"
#include "esp_spiffs.h"
#include "esp_timer.h"

#include "vision_dsp.h"
#include "video_frames_data.h"

namespace {

constexpr const char *TAG = "vision_poc";

// USGS / OpenChannelFlow Staff Gauge Hundredths-of-a-Foot (0.01 ft) Structure:
// - Longest Bar (Downturned Point): 0.10 ft Major Mark
// - Mid-Length Bar (Upturned Point): 0.05 ft Half-Tenth Landmark
// - Shortest Black Bars: 0.01 ft thick
// - White Spaces: 0.01 ft thick

struct staff_mark_reading_t {
    float head_ft;          // Snapped discrete 0.01 ft (hundredths) head value
    int transition_row;     // Image row of the distortion boundary
    float contrast_ratio;   // Crispness / confidence score
};

// Refined 0.01 ft Sub-Bar Edge & White Space Transition Detector
staff_mark_reading_t process_single_frame(const uint8_t *frame_crop, int width, int height) {
    const int right_x1 = 40;
    const int right_x2 = 100;

    // 1:1 full resolution y-rows for 0.01 ft hundredths increments:
    // y_crop = 60  => 0.08 ft (4th Black Bar Center)
    // y_crop = 85  => 0.07 ft (White Space / Bottom of 4th Bar)
    // y_crop = 110 => 0.06 ft (3rd Black Bar Center)
    // y_crop = 135 => 0.05 ft (Mid-Length Upturned Point Landmark)
    // y_crop = 160 => 0.04 ft (2nd Black Bar Center)
    const int y_08 = 60;
    const int y_07 = 85;
    const int y_06 = 110;
    const int y_05 = 135;
    const int y_04 = 160;

    auto eval_stripe = [&](int y_center, float &out_c, float &out_sx) {
        uint8_t min_v = 255;
        uint8_t max_v = 0;
        float sum_sobel = 0.0f;
        int count = 0;

        for (int y = y_center - 2; y <= y_center + 2; ++y) {
            for (int x = right_x1; x <= right_x2; ++x) {
                uint8_t p = frame_crop[y * width + x];
                if (p < min_v) min_v = p;
                if (p > max_v) max_v = p;

                int p_top = frame_crop[(y - 1) * width + x];
                int p_bot = frame_crop[(y + 1) * width + x];
                sum_sobel += static_cast<float>(std::abs(p_bot - p_top));
                count++;
            }
        }
        out_c = static_cast<float>(max_v - min_v);
        out_sx = sum_sobel / static_cast<float>(count);
    };

    float c_08 = 0.0f, sx_08 = 0.0f;
    float c_07 = 0.0f, sx_07 = 0.0f;
    float c_06 = 0.0f, sx_06 = 0.0f;
    float c_05 = 0.0f, sx_05 = 0.0f;
    float c_04 = 0.0f, sx_04 = 0.0f;

    eval_stripe(y_08, c_08, sx_08);
    eval_stripe(y_07, c_07, sx_07);
    eval_stripe(y_06, c_06, sx_06);
    eval_stripe(y_05, c_05, sx_05);
    eval_stripe(y_04, c_04, sx_04);

    float detected_head = 0.08f;
    int transition_y = y_08;
    float confidence = c_08 / 150.0f;

    // Evaluate top-down to find the optical transition at 0.01 ft hundredths resolution
    if (c_08 < 110.0f) {
        detected_head = 0.08f;
        transition_y = y_08;
        confidence = c_08 / 150.0f;
    } else if (c_07 < 100.0f) {
        detected_head = 0.07f;
        transition_y = y_07;
        confidence = c_07 / 150.0f;
    } else if (c_06 < 110.0f) {
        detected_head = 0.06f;
        transition_y = y_06;
        confidence = c_06 / 150.0f;
    } else if (c_05 < 100.0f) {
        detected_head = 0.05f;
        transition_y = y_05;
        confidence = c_05 / 150.0f;
    } else {
        detected_head = 0.04f;
        transition_y = y_04;
        confidence = c_04 / 150.0f;
    }

    return {
        .head_ft = detected_head,
        .transition_row = transition_y,
        .contrast_ratio = std::clamp(confidence, 0.10f, 1.0f)
    };
}

void write_annotated_bmp(const uint8_t *image, int width, int height, int red_row) {
    const esp_vfs_spiffs_conf_t config = {
        .base_path = "/images",
        .partition_label = "images",
        .max_files = 1,
        .format_if_mount_failed = true,
    };
    esp_vfs_spiffs_register(&config);

    FILE *file = fopen("/images/clean_reference.bmp", "wb");
    if (file == nullptr) {
        ESP_LOGE(TAG, "Unable to create annotated BMP image");
        return;
    }

    const int kRowBytes = width * 3;
    const int kImageBytes = kRowBytes * height;
    const uint8_t header[54] = {
        'B','M', static_cast<uint8_t>((54+kImageBytes)&255), static_cast<uint8_t>(((54+kImageBytes)>>8)&255),
        0,0,0,0, 54,0,0,0, 40,0,0,0, static_cast<uint8_t>(width),0,0,0,
        static_cast<uint8_t>(height),0,0,0, 1,0,24,0, 0,0,0,0,
        static_cast<uint8_t>(kImageBytes&255),static_cast<uint8_t>((kImageBytes>>8)&255),0,0,
        0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    };

    fwrite(header, 1, sizeof(header), file);
    for (int y = height - 1; y >= 0; --y) {
        for (int x = 0; x < width; ++x) {
            const uint8_t value = image[y * width + x];
            const uint8_t pixel[3] = {
                static_cast<uint8_t>(y == red_row ? 0 : value),
                static_cast<uint8_t>(y == red_row ? 0 : value),
                static_cast<uint8_t>(y == red_row ? 255 : value),
            };
            fwrite(pixel, 1, sizeof(pixel), file);
        }
    }
    fclose(file);
    ESP_LOGI(TAG, "Stored annotated audit BMP in SPIFFS at /images/clean_reference.bmp");
}

}  // namespace

void vision_poc_run(void) {
    ESP_LOGI(TAG, "==================================================================");
    ESP_LOGI(TAG, "ESP32-S3 REFINED 0.01 FT HUNDREDTHS RESOLUTION 169-FRAME BENCHMARK");
    ESP_LOGI(TAG, "==================================================================");

    dsp::VisionDSPPipeline dsp_pipeline(0.25f, 0.10f);
    dsp_pipeline.start_burst();

    const int64_t start_time = esp_timer_get_time();

    int last_red_row = 0;
    int count_08 = 0, count_07 = 0, count_06 = 0, count_05 = 0, count_04 = 0;

    for (int frame_idx = 0; frame_idx < kNumVideoFrames; ++frame_idx) {
        const uint8_t *crop = kVideoFrames[frame_idx];
        const staff_mark_reading_t reading = process_single_frame(crop, kVideoFrameWidth, kVideoFrameHeight);

        if (reading.head_ft >= 0.079f) count_08++;
        else if (reading.head_ft >= 0.069f) count_07++;
        else if (reading.head_ft >= 0.059f) count_06++;
        else if (reading.head_ft >= 0.049f) count_05++;
        else count_04++;

        dsp_pipeline.add_frame_sample(reading.head_ft, reading.contrast_ratio);
        last_red_row = reading.transition_row;

        if ((frame_idx + 1) % 10 == 0 || frame_idx == kNumVideoFrames - 1) {
            const dsp::VisionDSPPipeline::pipeline_output_t dsp_out = dsp_pipeline.finalize_burst();

            ESP_LOGI(TAG, "Frame %3d / %d | Raw Head: %.2f ft (%.2f in) | DSP Median: %.2f ft | Trimmed Mean: %.2f ft | Smoothed: %.2f ft",
                     frame_idx + 1, kNumVideoFrames, reading.head_ft, reading.head_ft * 12.0f,
                     dsp_out.burst_median_ft, dsp_out.burst_trimmed_mean_ft, dsp_out.smoothed_head_ft);

            dsp_pipeline.start_burst();
        }
    }

    const int64_t end_time = esp_timer_get_time();
    const double total_ms = static_cast<double>(end_time - start_time) / 1000.0;
    const double fps = (static_cast<double>(kNumVideoFrames) / total_ms) * 1000.0;

    ESP_LOGI(TAG, "==================================================================");
    ESP_LOGI(TAG, "REFINED 0.01 FT HUNDREDTHS %d-FRAME BENCHMARK COMPLETE", kNumVideoFrames);
    ESP_LOGI(TAG, "0.01 ft Hundredths Reading Distribution:");
    ESP_LOGI(TAG, "  0.08 ft (0.96 in): %3d frames (%d%%)", count_08, (count_08 * 100) / kNumVideoFrames);
    ESP_LOGI(TAG, "  0.07 ft (0.84 in): %3d frames (%d%%)", count_07, (count_07 * 100) / kNumVideoFrames);
    ESP_LOGI(TAG, "  0.06 ft (0.72 in): %3d frames (%d%%)", count_06, (count_06 * 100) / kNumVideoFrames);
    ESP_LOGI(TAG, "  0.05 ft (0.60 in): %3d frames (%d%%)", count_05, (count_05 * 100) / kNumVideoFrames);
    ESP_LOGI(TAG, "  0.04 ft (0.48 in): %3d frames (%d%%)", count_04, (count_04 * 100) / kNumVideoFrames);
    ESP_LOGI(TAG, "------------------------------------------------------------------");
    ESP_LOGI(TAG, "Total Processing Time: %.2f ms across %d video frames", total_ms, kNumVideoFrames);
    ESP_LOGI(TAG, "Processing Throughput: %.1f FPS (%.3f ms per frame)", fps, total_ms / kNumVideoFrames);
    ESP_LOGI(TAG, "==================================================================");

    write_annotated_bmp(kVideoFrames[kNumVideoFrames - 1], kVideoFrameWidth, kVideoFrameHeight, last_red_row);
}
