# Day 18 Report - GPU/CPU Cluster Optimization (18 Nov 2026)
- **Engineer:** Tamerlan (Lighting / Forward+)
- **Task:** Eliminate redundant per-frame cluster AABB rebuilds and TBO uploads when camera/window state is unchanged; run regression tests.
- **Commit:** `perf(lighting): optimize cluster updates`
- **Verification:** CTest `ClusterOptimizationRegressionTest` passed (1/1).
