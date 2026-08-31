# Intended data and audit records

This document describes the planned field record, image-retention, and technician-retrieval workflow. It is not current firmware behavior.

> **Current firmware status — proof of concept:** the application processes an embedded 169-frame corpus and reports writing one BMP at `/images/clean_reference.bmp` to a pre-provisioned SPIFFS partition. It does not capture camera images, create timestamped JPEGs, maintain a ring buffer, write CSV records, mount MicroSD, provide USB MSC, or use an RTC.

---

## Intended record cadence and contents

The field monitor is intended to produce one record every 15 minutes (96 per day), including timestamp, upstream and downstream head, flow, air and water temperature, battery and sensor health. Flow accounting must remain disabled until certified Parshall coefficients are configured.

Each camera burst should retain one representative audit image: the frame whose calculated level matches the burst median. The image should be annotated with the detected waterline and written as:

```
/images/audit_YYYYMMDD_HHMMSS_0.08ft.jpg
```

This retains visual evidence without storing every image from a burst.

## Intended storage tiers

1. **Primary — 32 GB MicroSD:** At approximately 15 KB per image, 96 images/day use about 1.44 MB/day and about 300 MB over an April–October irrigation season.
2. **Fallback — 4 MB SPIFFS:** If MicroSD is unavailable, the intended design is a FIFO ring buffer retaining roughly 250 audit images (about 2.5 days). The partition exists today; ring-buffer behavior does not.

CSV records and images are intended to be wear-managed and timestamped. The record schema, retention policy, and failure behavior remain to be specified before field deployment.

## Intended field retrieval

Technicians should connect a laptop to the monitor's native USB service port and mount the MicroSD filesystem as `BRADY_FLUME/` for drag-and-drop CSV and JPEG retrieval. USB MSC is a planned feature and has not been implemented or validated in the current firmware.
