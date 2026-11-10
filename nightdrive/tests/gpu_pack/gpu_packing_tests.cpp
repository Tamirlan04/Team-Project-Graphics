#include "cluster_gpu_packing.h"
#include <cstdlib>
#define assert(expr) do { if (!(expr)) { std::cerr << "Check failed: " #expr << " at line " << __LINE__ << "\n"; std::exit(1); } } while(false)
#include <cmath>
#include <iostream>
#include <stdexcept>
using namespace nightdrive;

int main() {
    ClusterGrid grid;
    assert(grid.clusterCount() == 1152);
    const std::vector<PointLight> lights = {
        PointLight(vec3(0.f, 5.f, -10.f), 20.f),
        PointLight(vec3(3.f, 5.f, -30.f), 15.f)
    };
    mat4 view;  // Identity view is sufficient for this layout test.
    grid.buildLightLists(view, lights);
    const auto packed = packClusterGpuData(grid, lights);
    assert(packed.clusterRangesRG.size() == 1152u * 2u);
    assert(packed.lightIndicesR == grid.lightIndices());
    assert(packed.lightsRGBA.size() == lights.size() * 8u);
    assert(packed.lightsRGBA[0] == 0.f);
    assert(packed.lightsRGBA[1] == 5.f);
    assert(packed.lightsRGBA[2] == -10.f);
    assert(packed.lightsRGBA[3] == 20.f);
    assert(packed.lightsRGBA[4] == 1.f);
    assert(packed.lightsRGBA[7] == 1.f);
    for (std::size_t i = 0; i < grid.ranges().size(); ++i) {
        assert(packed.clusterRangesRG[2*i] == grid.ranges()[i].offset);
        assert(packed.clusterRangesRG[2*i+1] == grid.ranges()[i].count);
    }
    grid.buildLightLists(view, {});
    const auto empty = packClusterGpuData(grid, {});
    assert(empty.lightsRGBA.empty());
    assert(empty.lightIndicesR.empty());
    bool caught = false;
    try { (void)packClusterGpuData(grid, lights, vec3(1,1,1), -1.f); }
    catch (const std::invalid_argument&) { caught = true; }
    assert(caught);
    std::cout << "GPU packing data validated: 1152 ranges, "
              << packed.lightIndicesR.size() << " references, "
              << lights.size() << " lights.\n";
    return 0;
}
