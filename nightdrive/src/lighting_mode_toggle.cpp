#include "lighting_mode_toggle.h"
#include <algorithm>

namespace nightdrive {

void LightingModeController::handleKeyPress(unsigned char key) {
    if (key == 'g' || key == 'G') {
        m_mode = (m_mode == LightingMode::Clustered)
            ? LightingMode::BruteForce
            : LightingMode::Clustered;
    }
}

LightingMode LightingModeController::currentMode() const {
    return m_mode;
}

int32_t LightingModeController::uniformValue() const {
    return static_cast<int32_t>(m_mode);
}

const char* LightingModeController::modeName() const {
    return (m_mode == LightingMode::Clustered) ? "Clustered Forward+" : "Brute-Force";
}

float computeSmoothAttenuation(float distance, float radius) {
    if (radius <= 0.0f || distance >= radius) {
        return 0.0f;
    }
    float ratio = std::clamp(1.0f - (distance / radius), 0.0f, 1.0f);
    return ratio * ratio;
}

} // namespace nightdrive
