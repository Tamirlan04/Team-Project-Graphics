#pragma once
#include <cstdint>
#include <cstddef>
#include <vector>

namespace nightdrive {

struct ClusterUploadPayload {
    std::vector<int32_t> offsetCountPairs; // RG32I: [offset0, count0, offset1, count1, ...]
    std::vector<int32_t> lightIndices;     // R32I:  [idx0, idx1, ...]
};

struct UploadValidationResult {
    bool valid = false;
    std::size_t clusterCount = 0;
    std::size_t totalLightLinks = 0;
    std::size_t bytesRanges = 0;
    std::size_t bytesIndices = 0;
};

UploadValidationResult validateAndPrepareUpload(
    const ClusterUploadPayload& payload,
    std::size_t expectedClusters = 1152,
    int32_t maxLights = 64);

} // namespace nightdrive
