#include "clustered_lighting.h"

#include <algorithm>
#include <cmath>
#include <limits>
#include <initializer_list>
#include <stdexcept>

namespace nightdrive {
namespace {

bool validConfig(const ClusterConfig& c) {
    return c.viewportWidth > 0 && c.viewportHeight > 0 &&
           c.tilesX > 0 && c.tilesY > 0 && c.depthSlices > 0 &&
           std::isfinite(c.nearPlane) && std::isfinite(c.farPlane) &&
           c.nearPlane > 0.0f && c.farPlane > c.nearPlane &&
           std::isfinite(c.verticalFovRadians) &&
           c.verticalFovRadians > 0.0f &&
           c.verticalFovRadians < radians(179.0f);
}

bool sphereIntersectsAabb(const vec3& p, float r, const ClusterBounds& b) {
    float squaredDistance = 0.0f;
    const float point[3] = {p.x, p.y, p.z};
    const float low[3] = {b.minimum.x, b.minimum.y, b.minimum.z};
    const float high[3] = {b.maximum.x, b.maximum.y, b.maximum.z};
    for (int axis = 0; axis < 3; ++axis) {
        float d = 0.0f;
        if (point[axis] < low[axis]) d = low[axis] - point[axis];
        if (point[axis] > high[axis]) d = point[axis] - high[axis];
        squaredDistance += d * d;
    }
    return squaredDistance <= r * r;
}

float depthBoundary(const ClusterConfig& c, int index) {
    // Logarithmic depth splitting: near * (far/near)^(index/slices).
    const float t = static_cast<float>(index) /
                    static_cast<float>(c.depthSlices);
    return c.nearPlane * std::pow(c.farPlane / c.nearPlane, t);
}

} // namespace

ClusterGrid::ClusterGrid(const ClusterConfig& config) { configure(config); }

std::size_t ClusterGrid::linearIndex(int x, int y, int z) const {
    return (static_cast<std::size_t>(z) * config_.tilesY + y) *
           config_.tilesX + x;
}

void ClusterGrid::configure(const ClusterConfig& config) {
    if (!validConfig(config)) {
        throw std::invalid_argument("Invalid ClusterConfig: check viewport and projection");
    }
    const std::size_t total = static_cast<std::size_t>(config.tilesX) *
                              static_cast<std::size_t>(config.tilesY) *
                              static_cast<std::size_t>(config.depthSlices);
    if (total > static_cast<std::size_t>(std::numeric_limits<int>::max())) {
        throw std::invalid_argument("Too many clusters");
    }
    config_ = config;
    bounds_.assign(total, ClusterBounds());
    ranges_.assign(total, ClusterRange());
    lightIndices_.clear();

    const float aspect = static_cast<float>(config.viewportWidth) /
                         static_cast<float>(config.viewportHeight);
    const float tangentY = std::tan(config.verticalFovRadians * 0.5f);
    const float tangentX = tangentY * aspect;

    for (int z = 0; z < config.depthSlices; ++z) {
        const float depthNear = depthBoundary(config, z);
        const float depthFar = (z == config.depthSlices - 1)
                               ? config.farPlane : depthBoundary(config, z + 1);
        for (int y = 0; y < config.tilesY; ++y) {
            // Bottom-left origin matches gl_FragCoord.y.
            const float yNdc0 = -1.0f + 2.0f * y / config.tilesY;
            const float yNdc1 = -1.0f + 2.0f * (y + 1) / config.tilesY;
            for (int x = 0; x < config.tilesX; ++x) {
                const float xNdc0 = -1.0f + 2.0f * x / config.tilesX;
                const float xNdc1 = -1.0f + 2.0f * (x + 1) / config.tilesX;
                ClusterBounds& b = bounds_[linearIndex(x, y, z)];
                b.nearDepth = depthNear;
                b.farDepth = depthFar;
                b.minimum = vec3(std::numeric_limits<float>::max(),
                                 std::numeric_limits<float>::max(),
                                 -depthFar);
                b.maximum = vec3(std::numeric_limits<float>::lowest(),
                                 std::numeric_limits<float>::lowest(),
                                 -depthNear);

                // Bound the eight corners of this frustum cell in view space.
                for (float d : {depthNear, depthFar}) {
                    for (float ny : {yNdc0, yNdc1}) {
                        for (float nx : {xNdc0, xNdc1}) {
                            const float px = nx * d * tangentX;
                            const float py = ny * d * tangentY;
                            b.minimum.x = std::min(b.minimum.x, px);
                            b.minimum.y = std::min(b.minimum.y, py);
                            b.maximum.x = std::max(b.maximum.x, px);
                            b.maximum.y = std::max(b.maximum.y, py);
                        }
                    }
                }
            }
        }
    }
}

void ClusterGrid::buildLightLists(const mat4& viewMatrix,
                                  const std::vector<PointLight>& lights) {
    lightIndices_.clear();
    std::fill(ranges_.begin(), ranges_.end(), ClusterRange());

    std::vector<vec3> viewPositions;
    viewPositions.reserve(lights.size());
    for (const PointLight& light : lights) {
        if (!std::isfinite(light.radius) || light.radius < 0.0f ||
            !std::isfinite(light.worldPosition.x) ||
            !std::isfinite(light.worldPosition.y) ||
            !std::isfinite(light.worldPosition.z)) {
            throw std::invalid_argument("Light has invalid position or radius");
        }
        viewPositions.push_back(transformPoint(viewMatrix, light.worldPosition));
    }

    // Simple, correct CPU reference pass for Day 1. It is deliberately O(C*L).
    // Later we can accelerate light-to-cluster candidate selection.
    for (std::size_t cluster = 0; cluster < bounds_.size(); ++cluster) {
        if (lightIndices_.size() > std::numeric_limits<std::uint32_t>::max()) {
            throw std::overflow_error("Cluster index list exceeds uint32_t range");
        }
        ClusterRange& range = ranges_[cluster];
        range.offset = static_cast<std::uint32_t>(lightIndices_.size());
        for (std::size_t light = 0; light < lights.size(); ++light) {
            if (sphereIntersectsAabb(viewPositions[light], lights[light].radius,
                                     bounds_[cluster])) {
                if (light > std::numeric_limits<std::uint32_t>::max() ||
                    lightIndices_.size() >= std::numeric_limits<std::uint32_t>::max()) {
                    throw std::overflow_error("Too many light references");
                }
                lightIndices_.push_back(static_cast<std::uint32_t>(light));
                ++range.count;
            }
        }
    }
}

int ClusterGrid::depthSliceFor(float viewDepth) const {
    if (!std::isfinite(viewDepth) ||
        viewDepth < config_.nearPlane || viewDepth > config_.farPlane) {
        return -1;
    }
    if (viewDepth == config_.farPlane) return config_.depthSlices - 1;
    const float normalized = std::log(viewDepth / config_.nearPlane) /
                             std::log(config_.farPlane / config_.nearPlane);
    const int slice = static_cast<int>(normalized * config_.depthSlices);
    return std::max(0, std::min(config_.depthSlices - 1, slice));
}

int ClusterGrid::clusterForPixel(int pixelX, int pixelY, float viewDepth) const {
    if (pixelX < 0 || pixelY < 0 ||
        pixelX >= config_.viewportWidth || pixelY >= config_.viewportHeight) {
        return -1;
    }
    const int z = depthSliceFor(viewDepth);
    if (z == -1) return -1;
    const int x = static_cast<int>(static_cast<std::int64_t>(pixelX) *
                                   config_.tilesX / config_.viewportWidth);
    const int y = static_cast<int>(static_cast<std::int64_t>(pixelY) *
                                   config_.tilesY / config_.viewportHeight);
    return static_cast<int>(linearIndex(x, y, z));
}

std::size_t ClusterGrid::nonEmptyClusterCount() const {
    return static_cast<std::size_t>(std::count_if(
        ranges_.begin(), ranges_.end(),
        [](const ClusterRange& range) { return range.count > 0; }));
}

std::uint32_t ClusterGrid::maxLightsInCluster() const {
    std::uint32_t maximum = 0;
    for (const ClusterRange& range : ranges_) {
        maximum = std::max(maximum, range.count);
    }
    return maximum;
}

} // namespace nightdrive
