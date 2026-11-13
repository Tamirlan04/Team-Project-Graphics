# Day 12 Report — Upload Per-frame Light Lists (12 Nov 2026)
- **Engineer:** Tamerlan (Lighting / Forward+)
- **Task:** Upload offsets/counts and light indices per-frame on camera move; verify buffer bounds and byte sizes for 16x9x8 (1152) clusters.
- **Commit:** `feat(lighting): upload cluster light lists`
- **Verification:** CTest `GpuUploadValidationTest` passed (1/1), 9216 bytes ranges buffer validated.
