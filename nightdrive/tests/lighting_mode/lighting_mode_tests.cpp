#include <cassert>
#include <cmath>
#include <iostream>
#include "../../src/lighting_mode_toggle.h"

int main() {
    using namespace nightdrive;
    LightingModeController ctrl;
    assert(ctrl.currentMode() == LightingMode::Clustered);
    assert(ctrl.uniformValue() == 1);

    ctrl.handleKeyPress('G');
    assert(ctrl.currentMode() == LightingMode::BruteForce);
    assert(ctrl.uniformValue() == 0);

    ctrl.handleKeyPress('g');
    assert(ctrl.currentMode() == LightingMode::Clustered);
    assert(ctrl.uniformValue() == 1);

    float att0 = computeSmoothAttenuation(0.0f, 20.0f);
    float attMid = computeSmoothAttenuation(10.0f, 20.0f);
    float attOut = computeSmoothAttenuation(20.0f, 20.0f);
    assert(std::fabs(att0 - 1.0f) < 1e-5f);
    assert(std::fabs(attMid - 0.25f) < 1e-5f);
    assert(std::fabs(attOut - 0.0f) < 1e-5f);

    std::cout << "[Day 14] Key G toggle (Brute-Force / Clustered) & road attenuation verified (1/1).\n";
    return 0;
}
