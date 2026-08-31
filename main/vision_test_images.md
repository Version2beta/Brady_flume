# Embedded staff-gauge test-image provenance

`vision_test_images.h` contains grayscale crops used only by the ESP32-S3 deterministic machine-vision proof of concept. It does **not** contain the original full-size photographs.

## Crop procedure

- Conversion: OpenCV BGR-to-grayscale conversion (`cv2.IMREAD_GRAYSCALE`).
- Crop rectangle in the original 768 × 1024 images: `x=320`, `y=760`, `width=160`, `height=100` pixels.
- The crop intentionally ends before the printed staff scale. Gauge ticks are for registration only and must never enter the waterline-detection ROI.
- Original-source paths are recorded below for local reproducibility. The original images remain in the Photos library and should not be altered by this project.

| Embedded array | Source path | SHA-256 of source JPEG | Result |
| --- | --- | --- | --- |
| `VISION_TEST_IMAGE_1` | `/Users/rob/Pictures/Photos Library.photoslibrary/resources/derivatives/1/128DFC09-1488-4401-904C-5387610B3182_1_105_c.jpeg` | `022b56161a2ec6923c87a43f96a280083232c7d465251e0fe801be04280a8665` | ESP32 candidate row 809 after excluding the staff-scale ROI. |
| `VISION_TEST_IMAGE_2` | `/Users/rob/Pictures/Photos Library.photoslibrary/resources/derivatives/6/6C480DF0-B487-42D1-A37B-B1B074A95925_1_105_c.jpeg` | `856c6b9264ee1f9548292dbc5be35f41b703f92685d31a6c6599b250c4ae0e39` | ESP32 candidate row 795 after excluding the staff-scale ROI. |
| `VISION_TEST_IMAGE_3` | `testdata/clean_gauge_reference.png` | `238afa2468aa22610169d96cc63add5faa2bd5d913b109460686464a9b4dbd07` | Cleaned reference used for the ESP32 calibrated-reading attempt. |

The known failure is retained deliberately. It prevents future work from presenting this solarization/edge-only method as a reliable level measurement.
