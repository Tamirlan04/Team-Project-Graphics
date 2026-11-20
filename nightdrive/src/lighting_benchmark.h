#pragma once
#include <vector>

namespace nightdrive {

struct BenchmarkResult {
    int lightCount = 0;
    bool isClustered = false;
    double averageFps = 0.0;
    double frameTimeMs = 0.0;
};

inline BenchmarkResult simulateLightingBenchmark(int lightCount, bool isClustered) {
    BenchmarkResult res{};
    res.lightCount = lightCount;
    res.isClustered = isClustered;
    res.averageFps = isClustered
        ? (120.0 / (1.0 + static_cast<double>(lightCount) * 0.012))
        : (120.0 / (1.0 + static_cast<double>(lightCount) * 0.08));
    res.frameTimeMs = 1000.0 / res.averageFps;
    return res;
}

} // namespace nightdrive
