#include "scene.h"
#include <cmath>

static const float LIGHT_WINDOW = 120.f;
static const float LIGHT_RADIUS = 30.f;
static const float BUILDING_WINDOW = 240.f;
static const float ROAD_LENGTH = 900.f;

static float wrapAround(float baseZ, float centerZ, float window) {
    float start = centerZ - window * 0.5f;
    float t = std::fmod(baseZ - start, window);
    if (t < 0.f) t += window;
    return start + t;
}

static float hash01(unsigned s) {
    s ^= s >> 16; s *= 0x7feb352dU; s ^= s >> 15; s *= 0x846ca68bU; s ^= s >> 16;
    return (float)(s & 0xFFFFFFu) / 16777216.f;
}

void Scene::build() {
    buildings.clear();
    trees.clear();
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
            unsigned bs = 2166136261u ^ (unsigned)((side + 2) * 7919 + k * 104729);
            buildings.push_back({ vec3(x, h * 0.5f, z), vec3(w, h, d), vec3(c, c + 0.01f, c + 0.04f), bs });
        }


    for (int side = -1; side <= 1; side += 2)
        for (int k = 0; k < 20; ++k) {
            unsigned ts = 40503u * (unsigned)(k + 1) + (unsigned)(side + 2) * 9176u;
            if (hash01(ts) < 0.30f) continue;
            float z = -66.f + 12.f * (float)k + (hash01(ts + 1u) - 0.5f) * 5.f;
            float x = (float)side * (10.f + hash01(ts + 2u) * 2.f);
            trees.push_back({ vec3(x, 0.f, z), 0.8f + hash01(ts + 3u) * 0.6f });
        }
    setLightPreset(16);
}

void Scene::setLightPreset(int total) {
    if (total != 16 && total != 32 && total != 64) total = 16;
    lightPreset = total;
    int perSide = total / 2;
    float spacing = LIGHT_WINDOW / (float)perSide;
    lightBase_.clear();


    for (int side = -1; side <= 1; side += 2) {
        float offset = (side > 0) ? spacing * 0.5f : 0.f;
        for (int i = 0; i < perSide; ++i)
            lightBase_.push_back(vec3((float)side * 5.2f, 5.8f, -52.5f + offset + spacing * (float)i));
    }
    lights.assign(lightBase_.size(), StreetLight{ vec3(), LIGHT_RADIUS });
    update(0.f);
}

void Scene::update(float carZ) {
    for (size_t i = 0; i < lightBase_.size(); ++i) {
        lights[i].pos = vec3(lightBase_[i].x, lightBase_[i].y, wrapAround(lightBase_[i].z, carZ, LIGHT_WINDOW));
        lights[i].radius = LIGHT_RADIUS;
    }
}

void Scene::draw(const DrawBoxFn& box, float carZ, bool streetOn) const {

    box(translate(vec3(0.f, -0.07f, carZ)) * scale(vec3(900.f, 0.1f, 900.f)), vec3(0.12f, 0.18f, 0.10f), 0.02f, 4.f, vec3());
    box(translate(vec3(0.f, -0.02f, carZ)) * scale(vec3(ROAD_HALF_WIDTH * 2.f, 0.06f, ROAD_LENGTH)),
        vec3(0.22f, 0.22f, 0.24f), 0.10f, 12.f, vec3());
    for (int side = -1; side <= 1; side += 2) {

        box(translate(vec3((float)side * 6.5f, 0.05f, carZ)) * scale(vec3(3.f, 0.16f, ROAD_LENGTH)),
            vec3(0.30f, 0.30f, 0.32f), 0.05f, 8.f, vec3());
        box(translate(vec3((float)side * 5.1f, 0.06f, carZ)) * scale(vec3(0.2f, 0.18f, ROAD_LENGTH)),
            vec3(0.42f, 0.42f, 0.44f), 0.05f, 8.f, vec3());

        box(translate(vec3((float)side * 4.5f, 0.016f, carZ)) * scale(vec3(0.15f, 0.012f, ROAD_LENGTH)),
            vec3(0.80f, 0.80f, 0.80f), 0.05f, 8.f, vec3());
    }

    float snap = std::floor(carZ / 6.f) * 6.f;
    for (int k = -20; k <= 20; ++k)
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

        float topY = b.size.y;
        if (b.size.y > 14.f) {
            vec3 ts(b.size.x * 0.7f, b.size.y * 0.25f, b.size.z * 0.7f);
            box(translate(vec3(p.x, topY + ts.y * 0.5f, p.z)) * scale(ts),
                b.color * 1.15f, 0.05f, 8.f, vec3());
            topY += ts.y;
        }

        float ox = (hash01(b.seed + 11u) - 0.5f) * b.size.x * 0.4f;
        float oz = (hash01(b.seed + 12u) - 0.5f) * b.size.z * 0.4f;
        box(translate(vec3(p.x + ox, topY + 0.5f, p.z + oz)) * scale(vec3(1.6f, 1.f, 1.6f)),
            vec3(0.10f, 0.10f, 0.12f), 0.05f, 8.f, vec3());


        float fs = (p.x > 0.f) ? -1.f : 1.f;
        float faceX = p.x + fs * (b.size.x * 0.5f + 0.03f);
        int row = 0;
        for (float y = 2.5f; y < b.size.y - 1.5f; y += 3.2f, ++row) {
            float r = hash01(b.seed + 100u + (unsigned)row * 977u);
            if (r < 0.45f) continue;
            float warm = hash01(b.seed + 500u + (unsigned)row);
            vec3 em = (warm < 0.7f) ? vec3(0.85f, 0.65f, 0.35f) : vec3(0.45f, 0.62f, 0.85f);
            box(translate(vec3(faceX, y, p.z)) * scale(vec3(0.06f, 0.7f, b.size.z * 0.8f)),
                vec3(0.05f, 0.05f, 0.05f), 0.f, 1.f, em);
        }
    }

    for (const Tree& t : trees) {
        vec3 p = t.pos;
        p.z = wrapAround(t.pos.z, carZ, BUILDING_WINDOW);
        float k = t.scale;
        box(translate(vec3(p.x, 0.6f * k, p.z)) * scale(vec3(0.3f * k, 1.2f * k, 0.3f * k)),
            vec3(0.18f, 0.10f, 0.05f), 0.f, 1.f, vec3());
        mat4 rot = rotateY(0.785f);
        box(translate(vec3(p.x, 1.7f * k, p.z)) * rot * scale(vec3(2.2f * k, 1.0f * k, 2.2f * k)),
            vec3(0.05f, 0.18f, 0.08f), 0.02f, 4.f, vec3());
        box(translate(vec3(p.x, 2.6f * k, p.z)) * rot * scale(vec3(1.6f * k, 1.0f * k, 1.6f * k)),
            vec3(0.06f, 0.21f, 0.09f), 0.02f, 4.f, vec3());
        box(translate(vec3(p.x, 3.4f * k, p.z)) * rot * scale(vec3(0.9f * k, 0.9f * k, 0.9f * k)),
            vec3(0.07f, 0.24f, 0.10f), 0.02f, 4.f, vec3());
    }
}
