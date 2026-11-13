#include "scene.h"
#include <cmath>

static const float LIGHT_WINDOW    = 120.f;   
static const float LIGHT_RADIUS    = 30.f;
static const float BUILDING_WINDOW = 240.f;   
static const float ROAD_LENGTH     = 400.f;


static float wrapAround(float baseZ, float centerZ, float window) {
    float start = centerZ - window * 0.5f;
    float t = std::fmod(baseZ - start, window);
    if (t < 0.f) t += window;
    return start + t;
}

void Scene::build() {
    buildings.clear();
    unsigned seed = 12345u;
    auto rnd = [&seed]() {
        seed = seed * 1664525u + 1013904223u;
        return (float)((seed >> 8) & 0xFFFFu) / 65535.f;
    };
    for (int side = -1; side <= 1; side += 2)
        for (int k = 0; k < 20; ++k) {
            float z = -66.f + 12.f * (float)k;
            float w = 7.f + rnd() * 3.f;
            float h = 6.f + rnd() * 16.f;
            float d = 8.f + rnd() * 2.f;
            float x = (float)side * (15.f + w * 0.5f + rnd() * 2.f);
            float c = 0.12f + rnd() * 0.08f;
            buildings.push_back({ vec3(x, h * 0.5f, z), vec3(w, h, d), vec3(c, c + 0.01f, c + 0.04f) });
        }
    setLightPreset(16);
}

void Scene::setLightPreset(int total) {
    if (total != 16 && total != 32 && total != 64) total = 16;
    lightPreset = total;
    int perSide = total / 2;
    float spacing = LIGHT_WINDOW / (float)perSide;      
    lightBase_.clear();
    for (int side = -1; side <= 1; side += 2)
        for (int i = 0; i < perSide; ++i)
            lightBase_.push_back(vec3((float)side * 5.2f, 5.8f, -52.5f + spacing * (float)i));
    lights.assign(lightBase_.size(), StreetLight{ vec3(), LIGHT_RADIUS });
    update(0.f);
}

void Scene::update(float carZ) {
    for (size_t i = 0; i < lightBase_.size(); ++i) {
        lights[i].pos    = vec3(lightBase_[i].x, lightBase_[i].y, wrapAround(lightBase_[i].z, carZ, LIGHT_WINDOW));
        lights[i].radius = LIGHT_RADIUS;
    }
}

void Scene::draw(const DrawBoxFn& box, float carZ, bool streetOn) const {
    
    box(translate(vec3(0.f, -0.07f, carZ)) * scale(vec3(400.f, 0.1f, 400.f)), vec3(0.12f, 0.18f, 0.10f), 0.02f, 4.f, vec3());
    box(translate(vec3(0.f, -0.02f, carZ)) * scale(vec3(ROAD_HALF_WIDTH * 2.f, 0.06f, ROAD_LENGTH)),
        vec3(0.22f, 0.22f, 0.24f), 0.10f, 12.f, vec3());

    float snap = std::floor(carZ / 6.f) * 6.f;           
    for (int k = -12; k <= 12; ++k)
        box(translate(vec3(0.f, 0.016f, snap + 6.f * (float)k)) * scale(vec3(0.25f, 0.012f, 3.f)),
            vec3(0.85f, 0.85f, 0.7f), 0.05f, 8.f, vec3());

    for (const StreetLight& L : lights) {
        float side = (L.pos.x > 0.f) ? 1.f : -1.f;
        float z = L.pos.z;
        box(translate(vec3(side * 6.5f, 3.f, z)) * scale(vec3(0.25f, 6.f, 0.25f)), vec3(0.15f, 0.15f, 0.17f), 0.2f, 16.f, vec3());
        box(translate(vec3(side * 5.85f, 5.9f, z)) * scale(vec3(1.3f, 0.15f, 0.15f)), vec3(0.15f, 0.15f, 0.17f), 0.2f, 16.f, vec3());
        box(translate(vec3(L.pos.x, 5.95f, z)) * scale(vec3(0.5f, 0.2f, 0.5f)), vec3(0.3f, 0.3f, 0.25f), 0.f, 1.f,
            streetOn ? vec3(1.f, 0.8f, 0.45f) : vec3(0.05f, 0.05f, 0.04f));
    }

    for (const Building& b : buildings) {
        vec3 p = b.pos;
        p.z = wrapAround(b.pos.z, carZ, BUILDING_WINDOW);
        box(translate(p) * scale(b.size), b.color, 0.05f, 8.f, vec3());
    }
}
