#include "cluster_gpu_packing.h"
#include <cmath>
#include <limits>
#include <stdexcept>

namespace nightdrive {
namespace {
bool finite(float v) { return std::isfinite(v); }
}

PackedClusterGpuData packClusterGpuData(const ClusterGrid& grid,
                                        const std::vector<PointLight>& lights,
                                        const vec3& streetColor,
                                        float intensity) {
    if (!finite(streetColor.x) || !finite(streetColor.y) || !finite(streetColor.z) ||
        !finite(intensity) || intensity < 0.0f) {
        throw std::invalid_argument("Invalid street light color/intensity");
    }
    if (grid.ranges().size() != grid.clusterCount()) {
        throw std::logic_error("Cluster range count does not match grid");
    }
    const auto& refs = grid.lightIndices();
    PackedClusterGpuData output;
    output.clusterRangesRG.reserve(grid.ranges().size() * 2);
    output.lightIndicesR = refs;
    output.lightsRGBA.reserve(lights.size() * 8);
    for (const auto& range : grid.ranges()) {
        const std::uint64_t end = std::uint64_t(range.offset) + range.count;
        if (end > refs.size()) {
            throw std::logic_error("Cluster range refers outside light-index array");
        }
        output.clusterRangesRG.push_back(range.offset);
        output.clusterRangesRG.push_back(range.count);
    }
    for (std::uint32_t index : refs) {
        if (index >= lights.size()) {
            throw std::logic_error("Cluster references an unknown light");
        }
    }
    for (const auto& light : lights) {
        const vec3& p = light.worldPosition;
        if (!finite(p.x) || !finite(p.y) || !finite(p.z) ||
            !finite(light.radius) || light.radius < 0.0f) {
            throw std::invalid_argument("Invalid point light");
        }
        output.lightsRGBA.insert(output.lightsRGBA.end(),
                                 {p.x, p.y, p.z, light.radius,
                                  streetColor.x, streetColor.y, streetColor.z, intensity});
    }
    return output;
}
} // namespace nightdrive
