#include "cluster_gpu_upload.h"

namespace nightdrive {

UploadValidationResult validateAndPrepareUpload(
    const ClusterUploadPayload& payload,
    std::size_t expectedClusters,
    int32_t maxLights)
{
    UploadValidationResult res{};
    if (payload.offsetCountPairs.size() != expectedClusters * 2) {
        return res;
    }

    res.clusterCount = expectedClusters;
    res.totalLightLinks = payload.lightIndices.size();
    res.bytesRanges = payload.offsetCountPairs.size() * sizeof(int32_t);
    res.bytesIndices = payload.lightIndices.size() * sizeof(int32_t);

    for (std::size_t i = 0; i < expectedClusters; ++i) {
        int32_t offset = payload.offsetCountPairs[i * 2];
        int32_t count  = payload.offsetCountPairs[i * 2 + 1];
        if (offset < 0 || count < 0) return res;
        if (static_cast<std::size_t>(offset + count) > payload.lightIndices.size()) return res;
    }

    for (int32_t lightId : payload.lightIndices) {
        if (lightId < 0 || lightId >= maxLights) return res;
    }

    res.valid = true;
    return res;
}

}





 // namespace nightdrive
