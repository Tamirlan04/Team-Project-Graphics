#pragma once

// Day 10: Pure C++ packing for GL 3.3 texture-buffer uploads.
// No GL dependency: testable on a machine without GLEW/FreeGLUT.
#include "clustered_lighting.h"
#include <cstdint>
#include <vector>

namespace nightdrive {

struct PackedClusterGpuData {
    // One RG32UI texel per cluster: [offset, count].
    std::vector<std::uint32_t> clusterRangesRG;
    // One R32UI texel per cluster-light reference: light index.
    std::vector<std::uint32_t> lightIndicesR;
    // Two RGBA32F texels per light: [x,y,z,radius], [r,g,b,intensity].
    std::vector<float> lightsRGBA;
};

// Call grid.buildLightLists(cameraViewMatrix, lights) BEFORE packing.
// Street lamp color and intensity are uniform for this initial implementation.
PackedClusterGpuData packClusterGpuData(const ClusterGrid& grid,
                                        const std::vector<PointLight>& lights,
                                        const vec3& streetColor = vec3(1.0f, 0.84f, 0.58f),
                                        float intensity = 1.0f);
} // namespace nightdrive
