idf_component_register(
    SRCS "main.c" "flow.c" "head.c" "vision_detector.cpp" "vision_poc.cpp" "vision_dsp.cpp"
    INCLUDE_DIRS "."
    REQUIRES esp_timer nvs_flash spiffs
)
