#include <cassert>
#include <iostream>
#include "../../src/cluster_update_optimizer.h"

int main() {
    using namespace nightdrive;
    ClusterUpdateOptimizer opt;
    assert(opt.shouldRebuildGrid(1280, 720, 0.1f, 1000.0f) == true);
    assert(opt.shouldRebuildGrid(1280, 720, 0.1f, 1000.0f) == false);

    FrameLightingState s1{ 10.0f, 1280, 720, 32 };
    assert(opt.shouldUploadLightLists(s1) == true);
    assert(opt.shouldUploadLightLists(s1) == false);

    FrameLightingState s2{ 11.5f, 1280, 720, 32 };
    assert(opt.shouldUploadLightLists(s2) == true);

    std::cout << "[Day 18] Cluster update optimization & regression test passed (1/1).\n";
    return 0;
}
