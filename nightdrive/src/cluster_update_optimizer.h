#pragma once
#include <cmath>

namespace nightdrive {

struct FrameLightingState {
    float viewHash = 0.0f;
    int width = 1280;
    int height = 720;
    int activeLights = 0;
};

class ClusterUpdateOptimizer {
public:
    bool shouldRebuildGrid(int w, int h, float zNear, float zFar) {
        if (w != m_w || h != m_h || std::fabs(zNear - m_near) > 1e-4f || std::fabs(zFar - m_far) > 1e-4f) {
            m_w = w; m_h = h; m_near = zNear; m_far = zFar;
            return true;
        }
        return false;
    }

    bool shouldUploadLightLists(const FrameLightingState& s) {
        if (std::fabs(s.viewHash - m_hash) > 1e-5f || s.activeLights != m_lights || s.width != m_w || s.height != m_h) {
            m_hash = s.viewHash; m_lights = s.activeLights;
            return true;
        }
        return false;
    }

private:
    int m_w = 0, m_h = 0, m_lights = -1;
    float m_near = 0.0f, m_far = 0.0f, m_hash = -1.0f;
};

} // namespace nightdrive
