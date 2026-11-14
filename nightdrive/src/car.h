#pragma once
#include "common.h"

struct Car {
    vec3  pos;
    float yaw      = 0.f;
    float speed    = 0.f;
    float distance = 0.f;   
    float tripTime = 0.f;   
};

struct CarInput { bool up = false, down = false, left = false, right = false, brake = false; };

constexpr float GOAL_DISTANCE = 2000.f;

void resetCar(Car& car);
void updateCar(Car& car, const CarInput& in, float dt);


void getHeadlights(const Car& car, vec3 outPos[2], vec3& outDir);

void drawCar(const Car& car, bool headOn, const DrawBoxFn& box);
