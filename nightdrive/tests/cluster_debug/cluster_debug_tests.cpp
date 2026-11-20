#include <cassert>
#include <iostream>
#include "../../src/cluster_debug.h"

int main() {
    using namespace nightdrive;
    ScreenResizeGridParams params{1920, 1080, 16, 9, 8};
    bool ok = validateClusterResizeAndBounds(params, 1.0f, 200.0f);
    assert(ok);

    float slice0 = computeClusterSliceDepth(0, 8, 1.0f, 200.0f);
    float sliceLast = computeClusterSliceDepth(8, 8, 1.0f, 200.0f);
    assert(slice0 >= 1.0f);
    assert(sliceLast <= 201.0f);

    std::cout << "[Day 16] Cluster debugging resize & depth slice validation passed (1/1).\n";
    return 0;
}
