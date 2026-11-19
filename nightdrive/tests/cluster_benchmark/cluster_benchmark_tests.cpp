#include <cassert>
#include <iostream>
#include <vector>
#include "../../src/lighting_benchmark.h"

int main() {
    using namespace nightdrive;
    std::vector<int> counts = {16, 32, 64};
    for (int cnt : counts) {
        auto bf = simulateLightingBenchmark(cnt, false);
        auto cl = simulateLightingBenchmark(cnt, true);
        assert(cl.averageFps >= bf.averageFps);
        std::cout << "[Benchmark] Lights: " << cnt
                  << " | Brute-Force: " << bf.averageFps << " FPS (" << bf.frameTimeMs << " ms)"
                  << " | Clustered: " << cl.averageFps << " FPS (" << cl.frameTimeMs << " ms)\n";
    }
    std::cout << "[Day 17] Performance benchmark passed (1/1).\n";
    return 0;
}
