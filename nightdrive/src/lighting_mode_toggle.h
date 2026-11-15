#pragma once
#include <cstdint>

namespace nightdrive {

enum class LightingMode : int32_t {
    BruteForce = 0,
    Clustered  = 1
};

class LightingModeController {
public:
    void handleKeyPress(unsigned char key);
    LightingMode currentMode() const;
    int32_t uniformValue() const;
    const char* modeName() const;
private:
    LightingMode m_mode = LightingMode::Clustered;
};

float computeSmoothAttenuation(float distance, float radius);

} // namespace nightdrive
