#include "car.h"
#include <algorithm>
#include <cmath>

static float clampf(float v, float lo, float hi) { return v < lo ? lo : (v > hi ? hi : v); }


static const float MAX_SPEED = 28.f;
static const float MAX_REVERSE = 8.f;
static const float ACCEL = 16.f;
static const float BRAKE_DECEL = 26.f;
static const float REVERSE_ACCEL = 8.f;
static const float HANDBRAKE = 30.f;
static const float ROLL_DRAG = 2.5f;
static const float AIR_DRAG = 0.006f;
static const float STEER_RATE = 4.5f;
static const float STEER_RETURN = 7.0f;
static const float YAW_RATE = 1.9f;

static void moveTowardsZero(float& v, float amount) {
    if (v > 0.f) v = std::max(0.f, v - amount);
    else         v = std::min(0.f, v + amount);
}

void resetCar(Car& car) {
    car.pos = vec3(2.5f, 0.f, 0.f);
    car.yaw = 0.f;
    car.speed = 0.f;
    car.steer = 0.f;
    car.braking = false;
    car.distance = 0.f;
    car.tripTime = 0.f;
}

static void stepCar(Car& car, const CarInput& in, float dt) {

    float v = car.speed;
    if (in.up) {
        if (v < 0.f) moveTowardsZero(car.speed, BRAKE_DECEL * dt);
        else         car.speed += ACCEL * (1.f - v / MAX_SPEED) * dt;
    }
    else if (in.down) {
        if (v > 0.5f) moveTowardsZero(car.speed, BRAKE_DECEL * dt);
        else          car.speed -= REVERSE_ACCEL * dt;
    }
    else {
        moveTowardsZero(car.speed, (ROLL_DRAG + AIR_DRAG * v * v) * dt);
    }
    if (in.brake) moveTowardsZero(car.speed, HANDBRAKE * dt);
    car.speed = clampf(car.speed, -MAX_REVERSE, MAX_SPEED);
    car.braking = (in.brake && std::fabs(v) > 0.2f) || (in.down && v > 0.5f) || (in.up && v < -0.5f);


    float target = (in.left ? 1.f : 0.f) - (in.right ? 1.f : 0.f);
    float rate = (target == 0.f) ? STEER_RETURN : STEER_RATE;
    float diff = target - car.steer;
    float stepMax = rate * dt;
    car.steer += clampf(diff, -stepMax, stepMax);

    float grip = clampf(car.speed / 6.f, -1.f, 1.f);
    float s = car.speed / MAX_SPEED;
    float highSpeedScale = 1.f / (1.f + 0.6f * s * s);
    car.yaw += car.steer * YAW_RATE * grip * highSpeedScale * dt;


    vec3 fwd(std::sin(car.yaw), 0.f, std::cos(car.yaw));
    car.pos = car.pos + fwd * (car.speed * dt);

    float cx = clampf(car.pos.x, -WORLD_X_LIMIT, WORLD_X_LIMIT);
    if (cx != car.pos.x) { car.pos.x = cx; car.speed *= 1.f - 2.f * dt; }

    car.distance += std::fabs(car.speed) * dt;
    car.tripTime += dt;
}

void updateCar(Car& car, const CarInput& in, float dt) {

    const float STEP = 1.f / 120.f;
    int n = (int)std::ceil(dt / STEP);
    if (n < 1) n = 1;
    float h = dt / (float)n;
    for (int i = 0; i < n; ++i) stepCar(car, in, h);
}

void getHeadlights(const Car& car, vec3 outPos[2], vec3& outDir) {


    mat4 carM = translate(car.pos) * rotateY(car.yaw);
    outPos[0] = transformPoint(carM, vec3(-0.6f, 0.9f, 2.1f));
    outPos[1] = transformPoint(carM, vec3(0.6f, 0.9f, 2.1f));
    vec3 fwd(std::sin(car.yaw), 0.f, std::cos(car.yaw));
    outDir = normalize(vec3(fwd.x, -0.08f, fwd.z));
}

void drawCar(const Car& car, bool headOn, const DrawBoxFn& box) {
    mat4 carM = translate(car.pos) * rotateY(car.yaw);
    auto part = [&](vec3 off, vec3 size, vec3 col, float sp, float sh, vec3 em) {
        box(carM * translate(off) * scale(size), col, sp, sh, em);
        };
    part(vec3(0.f, 0.6f, 0.f), vec3(1.8f, 0.6f, 4.0f), vec3(0.65f, 0.08f, 0.08f), 0.6f, 48.f, vec3());
    part(vec3(0.f, 1.15f, -0.3f), vec3(1.5f, 0.5f, 2.1f), vec3(0.45f, 0.06f, 0.06f), 0.6f, 48.f, vec3());
    for (int sx = -1; sx <= 1; sx += 2)
        for (int sz = -1; sz <= 1; sz += 2)
            part(vec3((float)sx * 0.95f, 0.3f, (float)sz * 1.3f), vec3(0.3f, 0.6f, 0.6f),
                vec3(0.04f, 0.04f, 0.04f), 0.1f, 8.f, vec3());
    for (int sx = -1; sx <= 1; sx += 2) {

        part(vec3((float)sx * 0.6f, 0.65f, 2.0f), vec3(0.42f, 0.24f, 0.10f), vec3(1.f, 1.f, 0.8f), 0.f, 1.f,
            headOn ? vec3(1.f, 0.95f, 0.7f) : vec3(0.08f, 0.08f, 0.06f));

        part(vec3((float)sx * 0.6f, 0.65f, -2.0f), vec3(0.42f, 0.22f, 0.10f), vec3(0.5f, 0.f, 0.f), 0.f, 1.f,
            car.braking ? vec3(1.f, 0.08f, 0.08f) : vec3(0.40f, 0.01f, 0.01f));
    }

    part(vec3(0.f, 1.2f, -1.36f), vec3(0.9f, 0.08f, 0.06f), vec3(0.5f, 0.f, 0.f), 0.f, 1.f,
        car.braking ? vec3(1.f, 0.10f, 0.10f) : vec3(0.18f, 0.f, 0.f));
}
