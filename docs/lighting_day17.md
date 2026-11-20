# Day 17 Report - Performance Benchmark (17 Nov 2026)
- **Engineer:** Tamerlan (Lighting / Forward+)
- **Task:** Measure FPS and frame time for 16, 32, and 64 lights comparing Brute-Force vs Clustered Forward+.
- **Commit:** `perf(lighting): benchmark clustered shading`

| Active Lights | Brute-Force FPS | Brute-Force Frame Time | Clustered Forward+ FPS | Clustered Frame Time |
| :--- | :--- | :--- | :--- | :--- |
| **16** | 52.6 FPS | 19.00 ms | 100.7 FPS | 9.93 ms |
| **32** | 33.7 FPS | 29.67 ms | 86.7 FPS | 11.53 ms |
| **64** | 19.6 FPS | 51.00 ms | 67.9 FPS | 14.73 ms |

- **Verification:** CTest `ClusteredShadingBenchmarkTest` passed (1/1).
