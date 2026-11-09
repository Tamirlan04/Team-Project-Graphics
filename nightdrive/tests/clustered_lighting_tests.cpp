#include "clustered_lighting.h"

#include <iostream>
#include <stdexcept>
#include <vector>

// Always enabled in Release too (unlike assert).
#define CHECK(expr) do { if (!(expr)) { \
    throw std::runtime_error("CHECK failed: " #expr); \
} } while (false)

using nightdrive::ClusterConfig;
using nightdrive::ClusterGrid;
using nightdrive::PointLight;

void testDefaultGrid() {
    ClusterGrid grid;
    CHECK(grid.clusterCount() == 16u * 9u * 8u);
    CHECK(grid.ranges().size() == grid.clusterCount());
    CHECK(grid.bounds().size() == grid.clusterCount());
    CHECK(grid.depthSliceFor(0.1f) == 0);
    CHECK(grid.depthSliceFor(1000.0f) == 7);
    CHECK(grid.depthSliceFor(0.01f) == -1);
    CHECK(grid.depthSliceFor(1001.0f) == -1);
    CHECK(grid.clusterForPixel(0, 0, 0.1f) == 0);
    CHECK(grid.clusterForPixel(1279, 719, 1000.0f) == 1151);
    CHECK(grid.clusterForPixel(-1, 0, 10.0f) == -1);
    CHECK(grid.clusterForPixel(1280, 0, 10.0f) == -1);
}

void testLightAssignment() {
    ClusterGrid grid;
    const std::vector<PointLight> lights = {
        PointLight(vec3(0.0f, 0.0f, -5.0f), 2.0f),
        PointLight(vec3(0.0f, 0.0f, 10.0f), 1.0f), // behind camera
        PointLight(vec3(0.0f, 0.0f, -100.0f), 10.0f)
    };
    grid.buildLightLists(mat4(), lights); // identity camera
    CHECK(grid.nonEmptyClusterCount() > 0);
    CHECK(grid.nonEmptyClusterCount() < grid.clusterCount());
    CHECK(grid.maxLightsInCluster() >= 1);
    CHECK(!grid.lightIndices().empty());

    std::size_t used = 0;
    bool foundFirst = false;
    bool foundThird = false;
    for (const auto& range : grid.ranges()) {
        CHECK(range.offset == used);
        CHECK(static_cast<std::size_t>(range.offset) + range.count <=
               grid.lightIndices().size());
        for (std::size_t k = 0; k < range.count; ++k) {
            const std::uint32_t index = grid.lightIndices()[range.offset + k];
            CHECK(index < lights.size());
            CHECK(index != 1); // small light behind camera must be culled
            foundFirst = foundFirst || (index == 0);
            foundThird = foundThird || (index == 2);
        }
        used += range.count;
    }
    CHECK(used == grid.lightIndices().size());
    CHECK(foundFirst && foundThird);
}

void testMovingCameraAndReset() {
    ClusterGrid grid;
    const std::vector<PointLight> lights = {PointLight(vec3(0, 0, -5), 2.0f)};
    grid.buildLightLists(mat4(), lights);
    CHECK(!grid.lightIndices().empty());

    // Move camera 100 world units to the right; light should be out of view.
    mat4 worldToView = translate(vec3(-100.0f, 0, 0));
    grid.buildLightLists(worldToView, lights);
    CHECK(grid.lightIndices().empty());
    CHECK(grid.nonEmptyClusterCount() == 0);

    ClusterConfig resized;
    resized.viewportWidth = 800;
    resized.viewportHeight = 600;
    grid.configure(resized);
    CHECK(grid.clusterCount() == 1152);
    CHECK(grid.ranges()[0].count == 0);
    CHECK(grid.clusterForPixel(799, 599, 2.0f) >= 0);
    CHECK(grid.clusterForPixel(800, 599, 2.0f) == -1);
}

void testInvalidConfig() {
    ClusterConfig bad;
    bad.nearPlane = 0;
    bool threw = false;
    try { ClusterGrid unused(bad); }
    catch (const std::invalid_argument&) { threw = true; }
    CHECK(threw);
}

int main() {
    try {
        testDefaultGrid();
        testLightAssignment();
        testMovingCameraAndReset();
        testInvalidConfig();
        std::cout << "PASS: default grid, depth mapping, light assignment, camera, resize, invalid config\n";
        return 0;
    } catch (const std::exception& e) {
        std::cerr << "FAIL: " << e.what() << "\n";
        return 1;
    }
}
