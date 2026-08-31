#include <inttypes.h>
#include <stdio.h>
#include <time.h>

#include "esp_log.h"
#include "esp_timer.h"
#include "nvs_flash.h"

#include "flow.h"
#include "head.h"
#include "vision_poc.h"

#define DEMO_UPDATE_INTERVAL_US \
    ((int64_t)BRADY_DEMO_UPDATE_SECONDS * 1000LL * 1000LL)
#define MIN_VALID_EPOCH_SECONDS 1704067200LL  // 2024-01-01 UTC

static const char *TAG = "brady_ditch";

// Placeholder only: do not deploy until the Parshall flume's certified
// geometry and operating regime establish these values.
static const parshall_calibration_t calibration = {
    .coefficient_cfs = 0.0f,
    .exponent = 0.0f,
    .minimum_head_ft = 0.0f,
};

static void log_demo_head(void *arg)
{
    (void)arg;

    const time_t wall_time = time(NULL);
    int64_t phase_seconds = (int64_t)wall_time;
    const char *time_source = "wall clock";

    // Until an RTC or SNTP sets the clock, retain a useful demonstrator whose
    // top-of-hour phase is the boot time rather than claiming an incorrect
    // civil time.
    if (phase_seconds < MIN_VALID_EPOCH_SECONDS) {
        phase_seconds = esp_timer_get_time() / 1000000LL;
        time_source = "uptime (clock unset)";
    }

    ESP_LOGI(TAG, "Demo head: %.3f ft (%s)",
             demo_head_at_seconds(phase_seconds), time_source);
}

void app_main(void)
{
    // Never erase NVS automatically: this monitor will eventually contain
    // accounting data. Recovery must be an explicit maintenance operation.
    ESP_ERROR_CHECK(nvs_flash_init());

    ESP_LOGI(TAG, "Brady Ditch monitor starting");
    ESP_LOGI(TAG, "ESP-IDF flow engine ready; ADC, RTC, display, and logging drivers are pending.");

    const esp_timer_create_args_t demo_timer_args = {
        .callback = log_demo_head,
        .name = "demo_head",
    };
    esp_timer_handle_t demo_timer;
    ESP_ERROR_CHECK(esp_timer_create(&demo_timer_args, &demo_timer));
    log_demo_head(NULL);
    ESP_ERROR_CHECK(esp_timer_start_periodic(demo_timer, DEMO_UPDATE_INTERVAL_US));

    vision_poc_run();

    float flow_cfs;
    if (!flow_from_head(&calibration, 1.0f, &flow_cfs)) {
        ESP_LOGW(TAG, "Flow conversion is disabled until Parshall calibration is configured.");
    }

    ESP_LOGI(TAG, "Boot timestamp: %" PRId64 " us", esp_timer_get_time());
}
