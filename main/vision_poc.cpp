#include "vision_poc.h"

#include <cstdint>
#include <cstdio>
#include <cstdlib>

#include "esp_log.h"
#include "esp_spiffs.h"
#include "esp_timer.h"

#include "vision_detector.h"
#include "vision_dsp.h"
#include "video_frames_data.h"

namespace {

constexpr const char *TAG = "vision_poc";

bool write_bytes(FILE *file, const void *data, size_t size) {
    return fwrite(data, 1, size, file) == size;
}

bool write_annotated_bmp(const uint8_t *image, int width, int height, int red_row) {
    const esp_vfs_spiffs_conf_t config = {
        .base_path = "/images",
        .partition_label = "images",
        .max_files = 1,
        .format_if_mount_failed = false,
    };
    const esp_err_t mount_result = esp_vfs_spiffs_register(&config);
    if (mount_result != ESP_OK) {
        ESP_LOGE(TAG, "Unable to mount audit SPIFFS partition: %s",
                 esp_err_to_name(mount_result));
        return false;
    }

    FILE *file = fopen("/images/clean_reference.bmp", "wb");
    if (file == nullptr) {
        ESP_LOGE(TAG, "Unable to create annotated BMP image");
        esp_vfs_spiffs_unregister(config.partition_label);
        return false;
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

    bool write_ok = write_bytes(file, header, sizeof(header));
    for (int y = height - 1; write_ok && y >= 0; --y) {
        for (int x = 0; write_ok && x < width; ++x) {
            const uint8_t value = image[y * width + x];
            const uint8_t pixel[3] = {
                static_cast<uint8_t>(y == red_row ? 0 : value),
                static_cast<uint8_t>(y == red_row ? 0 : value),
                static_cast<uint8_t>(y == red_row ? 255 : value),
            };
            write_ok = write_bytes(file, pixel, sizeof(pixel));
        }
    }

    const bool close_ok = fclose(file) == 0;
    esp_vfs_spiffs_unregister(config.partition_label);
    if (!write_ok || !close_ok) {
        ESP_LOGE(TAG, "Unable to complete annotated BMP image");
        return false;
    }

    ESP_LOGI(TAG, "Stored annotated audit BMP in SPIFFS at /images/clean_reference.bmp");
    return true;
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
        vision::staff_mark_reading_t reading;
        if (!vision::detect_staff_mark(crop, kVideoFrameWidth, kVideoFrameHeight,
                                       &reading)) {
            ESP_LOGE(TAG, "Embedded frame %d does not satisfy detector geometry", frame_idx);
            return;
        }

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
