#pragma once
#include <cstdint>
#include <algorithm>

namespace nightdrive {

struct ScreenResizeGridParams {
    int screenWidth = 1280;
    int screenHeight = 720;
    int gridX = 16;
    int gridY = 9;
    int gridZ = 8;
};

inline float computeClusterSliceDepth(int sliceIndex, int totalZ, float zNear, float zFar) {
    float safeNear = std::max(zNear, 0.1f);
    float safeFar  = std::max(zFar, safeNear + 1.0f);
    float ratio    = safeFar / safeNear;
    float fraction = static_cast<float>(sliceIndex) / static_cast<float>(totalZ);
    return safeNear * std::pow(ratio, fraction);
}

inline bool validateClusterResizeAndBounds(const ScreenResizeGridParams& params, float zNear, float zFar) {
    if (params.screenWidth <= 0 || params.screenHeight <= 0) return false;
    if (params.gridX <= 0 || params.gridY <= 0 || params.gridZ <= 0) return false;
    float d0 = computeClusterSliceDepth(0, params.gridZ, zNear, zFar);
    float dMax = computeClusterSliceDepth(params.gridZ, params.gridZ, zNear, zFar);
    return (d0 >= zNear && dMax <= zFar + 1.0f);
}

} // namespace nightdrive
