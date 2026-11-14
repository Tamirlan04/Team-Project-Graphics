#include <cassert>
#include <iostream>
#include "../../src/cluster_gpu_upload.h"

int main() {
    using namespace nightdrive;
    ClusterUploadPayload frame1{};
    frame1.offsetCountPairs.resize(1152 * 2, 0);
    frame1.offsetCountPairs[0] = 0;
    frame1.offsetCountPairs[1] = 3;
    frame1.lightIndices = {0, 5, 12};

    auto check1 = validateAndPrepareUpload(frame1, 1152, 64);
    assert(check1.valid);
    assert(check1.clusterCount == 1152);
    assert(check1.bytesRanges == 1152 * 2 * sizeof(int32_t));

    // Симуляция движения камеры (кадр 2)
    frame1.offsetCountPairs[2] = 3;
    frame1.offsetCountPairs[3] = 2;
    frame1.lightIndices.push_back(15);
    frame1.lightIndices.push_back(16);

    auto check2 = validateAndPrepareUpload(frame1, 1152, 64);
    assert(check2.valid);
    assert(check2.totalLightLinks == 5);

    std::cout << "[Day 12] Uploaded clusters: " << check2.clusterCount
              << ", ranges bytes: " << check2.bytesRanges
              << ", indices bytes: " << check2.bytesIndices << "\n";
    return 0;
}
