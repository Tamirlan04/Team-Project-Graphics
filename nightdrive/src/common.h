#pragma once
#include <functional>
#include "mathutil.h"

using DrawBoxFn = std::function<void(const mat4& model, const vec3& color,
                                     float spec, float shin, const vec3& emissive)>;

constexpr float ROAD_HALF_WIDTH = 5.f;
constexpr float WORLD_X_LIMIT   = 13.5f;   