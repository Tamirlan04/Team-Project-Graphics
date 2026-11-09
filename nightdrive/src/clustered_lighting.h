#pragma once

// CPU-side clustered-lighting preparation for Night Drive (OpenGL 3.3 Core).
// This module has no OpenGL dependency. GPU upload and shader integration
// will be added in a later task.

#include "mathutil.h"

#include <cstddef>
#include <cstdint>
#include <vector>

namespace nightdrive {

struct PointLight {
    vec3 worldPosition;
    float radius;

    PointLight(const vec3& position = vec3(), float influenceRadius = 30.0f)
        : worldPosition(position), radius(influenceRadius) {}
};

struct ClusterConfig {
    int viewportWidth = 1280;
    int viewportHeight = 720;
    int tilesX = 16;
    int tilesY = 9;
    int depthSlices = 8;
    float nearPlane = 0.1f;
    float farPlane = 1000.0f;
    float verticalFovRadians = radians(60.0f);
};

// Conservative axis-aligned bounds in view space (the camera looks along -Z).
struct ClusterBounds {
    vec3 minimum;
    vec3 maximum;
    float nearDepth = 0.0f; // positive distance from camera
    float farDepth = 0.0f;
};

// Packed CPU layout intended for an integer GPU texture buffer later.
struct ClusterRange {
    std::uint32_t offset = 0;
    std::uint32_t count = 0;
};

class ClusterGrid {
public:
    explicit ClusterGrid(const ClusterConfig& config = ClusterConfig());

    // Call again when viewport dimensions or projection change.
    void configure(const ClusterConfig& config);

    // Rebuild light-index lists when the camera or the lights move.
    // viewMatrix is the world-to-view transform, identical to camera lookAt.
    void buildLightLists(const mat4& viewMatrix,
                         const std::vector<PointLight>& lights);

    // Pixel origin is bottom-left, as in GLSL gl_FragCoord.
    // viewDepth is positive: -viewSpacePosition.z.
    // Returns -1 for pixels or depths outside the view frustum.
    int clusterForPixel(int pixelX, int pixelY, float viewDepth) const;
    int depthSliceFor(float viewDepth) const;

    const ClusterConfig& config() const { return config_; }
    const std::vector<ClusterBounds>& bounds() const { return bounds_; }
    const std::vector<ClusterRange>& ranges() const { return ranges_; }
    const std::vector<std::uint32_t>& lightIndices() const { return lightIndices_; }

    std::size_t clusterCount() const { return bounds_.size(); }
    std::size_t nonEmptyClusterCount() const;
    std::uint32_t maxLightsInCluster() const;

private:
    ClusterConfig config_;
    std::vector<ClusterBounds> bounds_;
    std::vector<ClusterRange> ranges_;
    std::vector<std::uint32_t> lightIndices_;

    std::size_t linearIndex(int x, int y, int z) const;
};

} // namespace nightdrive
