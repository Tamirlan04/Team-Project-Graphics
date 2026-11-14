#include "car.h"
#include <algorithm>
#include <cmath>

static float clampf(float v, float lo, float hi) { return v < lo ? lo : (v > hi ? hi : v); }

static void applyDrag(Car& car, float amount) {
    if (car.speed > 0.f) car.speed = std::max(0.f, car.speed - amount);
    else                 car.speed = std::min(0.f, car.speed + amount);
}

void resetCar(Car& car) {
    car.pos      = vec3(2.5f, 0.f, 0.f);
    car.yaw      = 0.f;
    car.speed    = 0.f;
    car.distance = 0.f;
    car.tripTime = 0.f;
}

void updateCar(Car& car, const CarInput& in, float dt) {
    float accel = 0.f;
    if (in.up)   accel += 14.f;
    if (in.down) accel -= 18.f;
    car.speed += accel * dt;
    if (!in.up && !in.down) applyDrag(car, 5.f * dt);
    if (in.brake)           applyDrag(car, 25.f * dt);
    car.speed = clampf(car.speed, -8.f, 28.f);

    float steer = (in.left ? 1.f : 0.f) - (in.right ? 1.f : 0.f);
    float k = clampf(car.speed / 6.f, -1.f, 1.f);
    car.yaw += steer * 1.7f * k * dt;

    vec3 fwd(std::sin(car.yaw), 0.f, std::cos(car.yaw));
    car.pos = car.pos + fwd * (car.speed * dt);

    
    float cx = clampf(car.pos.x, -WORLD_X_LIMIT, WORLD_X_LIMIT);
    if (cx != car.pos.x) { car.pos.x = cx; car.speed *= 0.95f; }

    car.distance += std::fabs(car.speed) * dt;
    car.tripTime += dt;
}

void getHeadlights(const Car& car, vec3 outPos[2], vec3& outDir) {
    mat4 carM = translate(car.pos) * rotateY(car.yaw);
    outPos[0] = transformPoint(carM, vec3(-0.6f, 0.65f, 2.05f));
    outPos[1] = transformPoint(carM, vec3( 0.6f, 0.65f, 2.05f));
    vec3 fwd(std::sin(car.yaw), 0.f, std::cos(car.yaw));
    outDir = normalize(vec3(fwd.x, -0.06f, fwd.z));
}

void drawCar(const Car& car, bool headOn, const DrawBoxFn& box) {
    mat4 carM = translate(car.pos) * rotateY(car.yaw);
    auto part = [&](vec3 off, vec3 size, vec3 col, float sp, float sh, vec3 em) {
        box(carM * translate(off) * scale(size), col, sp, sh, em);
    };
    part(vec3(0.f, 0.6f, 0.f),    vec3(1.8f, 0.6f, 4.0f), vec3(0.65f, 0.08f, 0.08f), 0.6f, 48.f, vec3());
    part(vec3(0.f, 1.15f, -0.3f), vec3(1.5f, 0.5f, 2.1f), vec3(0.45f, 0.06f, 0.06f), 0.6f, 48.f, vec3());
    for (int sx = -1; sx <= 1; sx += 2)
        for (int sz = -1; sz <= 1; sz += 2)
            part(vec3((float)sx * 0.95f, 0.3f, (float)sz * 1.3f), vec3(0.3f, 0.6f, 0.6f),
                 vec3(0.04f, 0.04f, 0.04f), 0.1f, 8.f, vec3());
    for (int sx = -1; sx <= 1; sx += 2) {
        part(vec3((float)sx * 0.6f, 0.65f, 2.0f), vec3(0.35f, 0.2f, 0.08f), vec3(1.f, 1.f, 0.8f), 0.f, 1.f,
             headOn ? vec3(1.f, 0.95f, 0.7f) : vec3(0.08f, 0.08f, 0.06f));
        part(vec3((float)sx * 0.6f, 0.65f, -2.0f), vec3(0.35f, 0.2f, 0.08f), vec3(0.5f, 0.f, 0.f), 0.f, 1.f,
             vec3(0.6f, 0.02f, 0.02f));
    }
}
