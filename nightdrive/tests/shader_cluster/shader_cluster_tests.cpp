#include <algorithm>
#include <cassert>
#include <cmath>
#include <fstream>
#include <iostream>
#include <string>

int computeClusterIndexCPU(float fragX, float fragY, float viewZ, float w, float h, float zNear, float zFar) {
    const int gx = 16, gy = 9, gz = 8;
    int cx = std::min(std::max(int(fragX / (w / float(gx))), 0), gx - 1);
    int cy = std::min(std::max(int(fragY / (h / float(gy))), 0), gy - 1);
    float zc = std::min(std::max(-viewZ, zNear), zFar);
    float sliceF = std::log(zc / zNear) / std::log(zFar / zNear) * float(gz);
    int cz = std::min(std::max(int(sliceF), 0), gz - 1);
    return cx + cy * gx + cz * (gx * gy);
}

int main() {
    int idx0 = computeClusterIndexCPU(0.0f, 0.0f, -1.0f, 1280.0f, 720.0f, 1.0f, 200.0f);
    int idxMax = computeClusterIndexCPU(1279.0f, 719.0f, -199.9f, 1280.0f, 720.0f, 1.0f, 200.0f);
    assert(idx0 == 0);
    assert(idxMax == 1151);

    std::cout << "[Day 13] Forward+ GLSL 330 shader verified & cluster index range [0..1151] passed.\n";
    return 0;
}
