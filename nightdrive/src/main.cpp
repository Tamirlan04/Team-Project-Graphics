#include <GL/glew.h>
#include <GL/freeglut.h>
#include <algorithm>
#include <cctype>
#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <fstream>
#include <sstream>
#include <string>
#include <unordered_map>
#include <vector>
#include "mathutil.h"
#include "nightdrive_hud.h"

static const char* TEAM_NAMES  = "Askaruly Temirlan, Tamerlan Murat";
static const char* TOPIC_TITLE = "Topic 7 - Low-poly night drive, clustered/forward+ street lights";

struct Mesh {
    GLuint vao = 0, vbo = 0;
    GLsizei count = 0;
    GLenum mode = GL_TRIANGLES;
};

struct Building { vec3 pos, size, color; };

struct Car {
    vec3 pos;
    float yaw = 0.f;    
    float speed = 0.f;
};

static GLuint gProgram = 0;
static std::unordered_map<std::string, GLint> gLocCache;

static Mesh gCube, gSphere, gStars;

static Car gCar;
static std::vector<vec3> gStreetLights;   
static std::vector<Building> gBuildings;

static bool gUp = false, gDown = false, gLeft = false, gRight = false, gBrake = false;
static bool gPaused = false;
static bool gStreetOn = true;
static bool gHeadOn = true;
static bool gChaseCam = true;

static float gOrbitYaw = 3.14159f, gOrbitPitch = 0.35f, gOrbitDist = 12.f;
static bool gDragging = false;
static int gLastX = 0, gLastY = 0;

static float gMoonAz = 0.4f, gMoonEl = 0.45f;   

static float gAspect = 16.f / 9.f;
static int gLastTime = 0;
static int gFrames = 0, gFpsTime = 0, gFps = 0;

static const float ROAD_HALF_WIDTH = 5.f;
static const float ROAD_LENGTH = 140.f;

static std::string readTextFile(const char* name) {
    const char* dirs[] = { "shaders/", "../shaders/", "../../shaders/" };
    for (const char* d : dirs) {
        std::ifstream in(std::string(d) + name, std::ios::in | std::ios::binary);
        if (in) {
            std::stringstream ss;
            ss << in.rdbuf();
            return ss.str();
        }
    }
    std::fprintf(stderr, "ERROR: cannot find shader file '%s' (run from the project folder)\n", name);
    return std::string();
}

static GLuint compileShader(GLenum type, const char* file) {
    std::string src = readTextFile(file);
    if (src.empty()) return 0;
    GLuint sh = glCreateShader(type);
    const char* p = src.c_str();
    glShaderSource(sh, 1, &p, nullptr);
    glCompileShader(sh);
    GLint ok = 0;
    glGetShaderiv(sh, GL_COMPILE_STATUS, &ok);
    if (!ok) {
        char log[2048];
        glGetShaderInfoLog(sh, sizeof(log), nullptr, log);
        std::fprintf(stderr, "ERROR: shader compile failed (%s):\n%s\n", file, log);
        glDeleteShader(sh);
        return 0;
    }
    return sh;
}

static bool buildProgram() {
    GLuint vs = compileShader(GL_VERTEX_SHADER, "scene.vert");
    GLuint fs = compileShader(GL_FRAGMENT_SHADER, "scene.frag");
    if (!vs || !fs) return false;
    gProgram = glCreateProgram();
    glAttachShader(gProgram, vs);
    glAttachShader(gProgram, fs);
    glLinkProgram(gProgram);
    GLint ok = 0;
    glGetProgramiv(gProgram, GL_LINK_STATUS, &ok);
    if (!ok) {
        char log[2048];
        glGetProgramInfoLog(gProgram, sizeof(log), nullptr, log);
        std::fprintf(stderr, "ERROR: program link failed:\n%s\n", log);
        return false;
    }
    glDeleteShader(vs);
    glDeleteShader(fs);
    return true;
}

static GLint loc(const char* name) {
    auto it = gLocCache.find(name);
    if (it != gLocCache.end()) return it->second;
    GLint l = glGetUniformLocation(gProgram, name);
    if (l < 0) std::fprintf(stderr, "WARNING: uniform '%s' not found in shader program\n", name);
    gLocCache[name] = l;
    return l;
}
static void setInt(const char* n, int v)            { glUniform1i(loc(n), v); }
static void setFloat(const char* n, float v)        { glUniform1f(loc(n), v); }
static void setVec3(const char* n, const vec3& v)   { glUniform3f(loc(n), v.x, v.y, v.z); }
static void setMat4(const char* n, const mat4& m)   { glUniformMatrix4fv(loc(n), 1, GL_FALSE, m.m); }

static void pushVertex(std::vector<float>& v, const vec3& p, const vec3& n) {
    v.push_back(p.x); v.push_back(p.y); v.push_back(p.z);
    v.push_back(n.x); v.push_back(n.y); v.push_back(n.z);
}

static Mesh makeMesh(const std::vector<float>& data, GLenum mode) {
    Mesh m;
    m.mode = mode;
    m.count = (GLsizei)(data.size() / 6);
    glGenVertexArrays(1, &m.vao);
    glBindVertexArray(m.vao);
    glGenBuffers(1, &m.vbo);
    glBindBuffer(GL_ARRAY_BUFFER, m.vbo);
    glBufferData(GL_ARRAY_BUFFER, data.size() * sizeof(float), data.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    glBindVertexArray(0);
    return m;
}

static Mesh buildCube() {
    struct Face { vec3 n, u, w; };  
    const Face faces[6] = {
        { vec3( 1, 0, 0), vec3(0, 1, 0), vec3(0, 0, 1) },
        { vec3(-1, 0, 0), vec3(0, 0, 1), vec3(0, 1, 0) },
        { vec3( 0, 1, 0), vec3(0, 0, 1), vec3(1, 0, 0) },
        { vec3( 0,-1, 0), vec3(1, 0, 0), vec3(0, 0, 1) },
        { vec3( 0, 0, 1), vec3(1, 0, 0), vec3(0, 1, 0) },
        { vec3( 0, 0,-1), vec3(0, 1, 0), vec3(1, 0, 0) },
    };
    std::vector<float> v;
    for (const Face& f : faces) {
        vec3 c = f.n * 0.5f;
        vec3 p0 = c - f.u * 0.5f - f.w * 0.5f;
        vec3 p1 = c + f.u * 0.5f - f.w * 0.5f;
        vec3 p2 = c + f.u * 0.5f + f.w * 0.5f;
        vec3 p3 = c - f.u * 0.5f + f.w * 0.5f;
        pushVertex(v, p0, f.n); pushVertex(v, p1, f.n); pushVertex(v, p2, f.n);
        pushVertex(v, p0, f.n); pushVertex(v, p2, f.n); pushVertex(v, p3, f.n);
    }
    return makeMesh(v, GL_TRIANGLES);
}

static void pushTriFlat(std::vector<float>& v, vec3 a, vec3 b, vec3 c) {
    vec3 n = cross(b - a, c - a);
    if (length(n) < 1e-6f) return;              
    vec3 centroid = (a + b + c) * (1.f / 3.f);
    if (dot(n, centroid) < 0.f) { vec3 t = b; b = c; c = t; n = -n; }
    n = normalize(n);
    pushVertex(v, a, n); pushVertex(v, b, n); pushVertex(v, c, n);
}

static vec3 spherePoint(int i, int j, int stacks, int slices) {
    float phi = 3.14159265f * (float)i / (float)stacks;
    float th = 2.f * 3.14159265f * (float)j / (float)slices;
    return vec3(0.5f * std::sin(phi) * std::cos(th), 0.5f * std::cos(phi), 0.5f * std::sin(phi) * std::sin(th));
}

static Mesh buildSphere(int stacks, int slices) {
    std::vector<float> v;
    for (int i = 0; i < stacks; ++i)
        for (int j = 0; j < slices; ++j) {
            vec3 a = spherePoint(i, j, stacks, slices);
            vec3 b = spherePoint(i + 1, j, stacks, slices);
            vec3 c = spherePoint(i + 1, j + 1, stacks, slices);
            vec3 d = spherePoint(i, j + 1, stacks, slices);
            pushTriFlat(v, a, b, c);
            pushTriFlat(v, a, c, d);
        }
    return makeMesh(v, GL_TRIANGLES);
}

static Mesh buildStars(int n) {
    std::vector<float> v;
    std::srand(7);
    for (int i = 0; i < n; ++i) {
        float y = 0.03f + 0.97f * ((float)std::rand() / (float)RAND_MAX);
        float a = 6.2831853f * ((float)std::rand() / (float)RAND_MAX);
        float r = std::sqrt(std::max(0.f, 1.f - y * y));
        pushVertex(v, vec3(r * std::cos(a), y, r * std::sin(a)) * 400.f, vec3(0, 1, 0));
    }
    return makeMesh(v, GL_POINTS);
}

static void resetCar() {
    gCar.pos = vec3(2.5f, 0.f, -55.f);   
    gCar.yaw = 0.f;
    gCar.speed = 0.f;
}

static void buildScene() {
    for (int side = -1; side <= 1; side += 2)
        for (int i = 0; i < 8; ++i) {
            float z = -52.5f + 15.f * (float)i;
            gStreetLights.push_back(vec3((float)side * 5.2f, 5.8f, z));
        }

    unsigned seed = 12345u;
    auto rnd = [&seed]() {
        seed = seed * 1664525u + 1013904223u;
        return (float)((seed >> 8) & 0xFFFFu) / 65535.f;
    };
    for (int side = -1; side <= 1; side += 2)
        for (float z = -66.f; z <= 66.f; z += 12.f) {
            float w = 7.f + rnd() * 3.f;
            float h = 6.f + rnd() * 16.f;
            float d = 8.f + rnd() * 2.f;
            float x = (float)side * (15.f + w * 0.5f + rnd() * 2.f);
            float c = 0.12f + rnd() * 0.08f;
            gBuildings.push_back({ vec3(x, h * 0.5f, z), vec3(w, h, d), vec3(c, c + 0.01f, c + 0.04f) });
        }
    resetCar();
}

static bool initGL() {
    std::printf("GL_VERSION  : %s\n", glGetString(GL_VERSION));
    std::printf("GLSL_VERSION: %s\n", glGetString(GL_SHADING_LANGUAGE_VERSION));
    if (!GLEW_VERSION_3_3) {
        std::fprintf(stderr, "ERROR: OpenGL 3.3 core is required but not available.\n");
        return false;
    }
    glEnable(GL_DEPTH_TEST);            
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glEnable(GL_PROGRAM_POINT_SIZE);
    if (!buildProgram()) return false;
    gCube = buildCube();
    gSphere = buildSphere(6, 10);
    gStars = buildStars(600);
    buildScene();
    if (!initNightDriveHud()) return false;
    return true;
}

static void drawPart(const Mesh& m, const mat4& model, const vec3& color,
                     float spec = 0.15f, float shin = 16.f, const vec3& emissive = vec3()) {
    setMat4("uModel", model);
    setVec3("uColor", color);
    setFloat("uSpec", spec);
    setFloat("uShin", shin);
    setVec3("uEmissive", emissive);
    glBindVertexArray(m.vao);
    glDrawArrays(m.mode, 0, m.count);
}

static vec3 moonDirection() {
    return vec3(std::sin(gMoonAz) * std::cos(gMoonEl), std::sin(gMoonEl), std::cos(gMoonAz) * std::cos(gMoonEl));
}

static void updateTitle() {
    char t[400];
    std::snprintf(t, sizeof(t),
        "%s | %s | FPS %d | Lighting: BRUTE-FORCE baseline, 16 street lights %s [T] | Headlights %s [F] | %s | Speed %.0f",
        TEAM_NAMES, TOPIC_TITLE, gFps, gStreetOn ? "ON" : "OFF", gHeadOn ? "ON" : "OFF",
        gPaused ? "PAUSED" : "running", gCar.speed);
    glutSetWindowTitle(t);
}

static void display() {
    glClearColor(0.01f, 0.015f, 0.05f, 1.f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    vec3 fwd(std::sin(gCar.yaw), 0.f, std::cos(gCar.yaw));
    vec3 eye, target;
    if (gChaseCam) {
        eye = gCar.pos - fwd * 9.f + vec3(0.f, 4.2f, 0.f);
        target = gCar.pos + fwd * 4.f + vec3(0.f, 1.f, 0.f);
    } else {
        target = gCar.pos + vec3(0.f, 1.f, 0.f);
        eye = target + vec3(std::cos(gOrbitPitch) * std::sin(gOrbitYaw), std::sin(gOrbitPitch),
                            std::cos(gOrbitPitch) * std::cos(gOrbitYaw)) * gOrbitDist;
    }
    mat4 view = lookAt(eye, target, vec3(0.f, 1.f, 0.f));
    mat4 proj = perspective(radians(60.f), gAspect, 0.1f, 1000.f);

    glUseProgram(gProgram);
    setMat4("uView", view);
    setMat4("uProj", proj);

    mat4 carM = translate(gCar.pos) * rotateY(gCar.yaw);
    vec3 headPos[2] = { transformPoint(carM, vec3(-0.6f, 0.65f, 2.05f)),
                        transformPoint(carM, vec3( 0.6f, 0.65f, 2.05f)) };
    vec3 headDir = normalize(vec3(fwd.x, -0.06f, fwd.z));
    vec3 moonDir = moonDirection();

    setVec3("uViewPos", eye);
    setVec3("uAmbient", vec3(0.05f, 0.06f, 0.11f));
    setVec3("uMoonDir", moonDir);
    setVec3("uMoonColor", vec3(0.20f, 0.25f, 0.40f));

    std::vector<float> flat;
    for (const vec3& p : gStreetLights) { flat.push_back(p.x); flat.push_back(p.y); flat.push_back(p.z); }
    setInt("uStreetOn", gStreetOn ? 1 : 0);
    setInt("uNumStreet", (int)gStreetLights.size());
    glUniform3fv(loc("uStreetPos[0]"), (GLsizei)gStreetLights.size(), flat.data());
    setVec3("uStreetColor", vec3(5.f, 3.9f, 2.25f));

    float hp[6] = { headPos[0].x, headPos[0].y, headPos[0].z, headPos[1].x, headPos[1].y, headPos[1].z };
    setInt("uHeadOn", gHeadOn ? 1 : 0);
    glUniform3fv(loc("uHeadPos[0]"), 2, hp);
    setVec3("uHeadDir", headDir);
    setVec3("uHeadColor", vec3(6.f, 5.7f, 4.8f));
    setFloat("uHeadCosCut", std::cos(radians(28.f)));

    setInt("uUnlit", 1);
    setVec3("uEmissive", vec3());
    drawPart(gStars, translate(eye), vec3(0.9f, 0.9f, 1.f));
    drawPart(gSphere, translate(eye + moonDir * 350.f) * scale(vec3(40.f, 40.f, 40.f)), vec3(0.95f, 0.95f, 0.85f));
    setInt("uUnlit", 0);

    drawPart(gCube, translate(vec3(0.f, -0.07f, 0.f)) * scale(vec3(400.f, 0.1f, 400.f)), vec3(0.12f, 0.18f, 0.10f), 0.02f, 4.f);
    drawPart(gCube, translate(vec3(0.f, -0.02f, 0.f)) * scale(vec3(ROAD_HALF_WIDTH * 2.f, 0.06f, ROAD_LENGTH)), vec3(0.22f, 0.22f, 0.24f), 0.10f, 12.f);
    for (float z = -66.f; z <= 66.f; z += 6.f)
        drawPart(gCube, translate(vec3(0.f, 0.016f, z)) * scale(vec3(0.25f, 0.012f, 3.f)), vec3(0.85f, 0.85f, 0.7f), 0.05f, 8.f);

    for (const vec3& L : gStreetLights) {
        float side = (L.x > 0.f) ? 1.f : -1.f;
        drawPart(gCube, translate(vec3(side * 6.5f, 3.f, L.z)) * scale(vec3(0.25f, 6.f, 0.25f)), vec3(0.15f, 0.15f, 0.17f), 0.2f, 16.f);
        drawPart(gCube, translate(vec3(side * 5.85f, 5.9f, L.z)) * scale(vec3(1.3f, 0.15f, 0.15f)), vec3(0.15f, 0.15f, 0.17f), 0.2f, 16.f);
        drawPart(gCube, translate(vec3(L.x, 5.95f, L.z)) * scale(vec3(0.5f, 0.2f, 0.5f)), vec3(0.3f, 0.3f, 0.25f), 0.f, 1.f,
                 gStreetOn ? vec3(1.f, 0.8f, 0.45f) : vec3(0.05f, 0.05f, 0.04f));
    }

    for (const Building& b : gBuildings)
        drawPart(gCube, translate(b.pos) * scale(b.size), b.color, 0.05f, 8.f);

    auto part = [&](vec3 off, vec3 size, vec3 col, float sp, float sh, vec3 em) {
        drawPart(gCube, carM * translate(off) * scale(size), col, sp, sh, em);
    };
    part(vec3(0.f, 0.6f, 0.f),   vec3(1.8f, 0.6f, 4.0f), vec3(0.65f, 0.08f, 0.08f), 0.6f, 48.f, vec3());
    part(vec3(0.f, 1.15f, -0.3f), vec3(1.5f, 0.5f, 2.1f), vec3(0.45f, 0.06f, 0.06f), 0.6f, 48.f, vec3());
    for (int sx = -1; sx <= 1; sx += 2)
        for (int sz = -1; sz <= 1; sz += 2)
            part(vec3((float)sx * 0.95f, 0.3f, (float)sz * 1.3f), vec3(0.3f, 0.6f, 0.6f), vec3(0.04f, 0.04f, 0.04f), 0.1f, 8.f, vec3());
    for (int sx = -1; sx <= 1; sx += 2) {
        part(vec3((float)sx * 0.6f, 0.65f, 2.0f),  vec3(0.35f, 0.2f, 0.08f), vec3(1.f, 1.f, 0.8f), 0.f, 1.f,
             gHeadOn ? vec3(1.f, 0.95f, 0.7f) : vec3(0.08f, 0.08f, 0.06f));
        part(vec3((float)sx * 0.6f, 0.65f, -2.0f), vec3(0.35f, 0.2f, 0.08f), vec3(0.5f, 0.f, 0.f), 0.f, 1.f, vec3(0.6f, 0.02f, 0.02f));
    }

    glBindVertexArray(0);
    drawNightDriveHud(glutGet(GLUT_WINDOW_WIDTH), glutGet(GLUT_WINDOW_HEIGHT),
                      gFps, gCar.speed, gPaused, gStreetOn, gHeadOn);
    glutSwapBuffers();

    ++gFrames;
    int now = glutGet(GLUT_ELAPSED_TIME);
    if (now - gFpsTime >= 1000) {
        gFps = gFrames * 1000 / (now - gFpsTime);
        gFrames = 0;
        gFpsTime = now;
        updateTitle();
    }
}

static float clampf(float v, float lo, float hi) { return v < lo ? lo : (v > hi ? hi : v); }

static void applyDrag(float amount) {
    if (gCar.speed > 0.f) gCar.speed = std::max(0.f, gCar.speed - amount);
    else                  gCar.speed = std::min(0.f, gCar.speed + amount);
}

static void updateCar(float dt) {
    float accel = 0.f;
    if (gUp)   accel += 14.f;
    if (gDown) accel -= 18.f;
    gCar.speed += accel * dt;
    if (!gUp && !gDown) applyDrag(5.f * dt);
    if (gBrake) applyDrag(25.f * dt);
    gCar.speed = clampf(gCar.speed, -8.f, 28.f);

    float steer = (gLeft ? 1.f : 0.f) - (gRight ? 1.f : 0.f);
    float k = clampf(gCar.speed / 6.f, -1.f, 1.f);        
    gCar.yaw += steer * 1.7f * k * dt;

    vec3 fwd(std::sin(gCar.yaw), 0.f, std::cos(gCar.yaw));
    gCar.pos = gCar.pos + fwd * (gCar.speed * dt);
}

static void idle() {            
    int now = glutGet(GLUT_ELAPSED_TIME);
    float dt = (float)(now - gLastTime) * 0.001f;
    gLastTime = now;
    if (dt > 0.05f) dt = 0.05f;
    updateCar(dt);
    glutPostRedisplay();
}

static void reshape(int w, int h) {
    if (h == 0) h = 1;
    glViewport(0, 0, w, h);
    gAspect = (float)w / (float)h;
}



static void togglePause() {
    gPaused = !gPaused;
    if (gPaused) {
        glutIdleFunc(nullptr);  
    } else {
        gLastTime = glutGet(GLUT_ELAPSED_TIME);
        glutIdleFunc(idle);
    }
}

static void keyDown(unsigned char key, int, int) {
    key = (unsigned char)std::tolower(key);
    switch (key) {
        case 27:  std::exit(0);
        case ' ': gBrake = true; break;
        case 'p': togglePause(); break;
        case 'r': resetCar(); break;
        case 'c':
            gChaseCam = !gChaseCam;
            if (!gChaseCam) gOrbitYaw = gCar.yaw + 3.14159f;   
            break;
        case 't': gStreetOn = !gStreetOn; break;
        case 'f': gHeadOn = !gHeadOn; break;
        case '[': gMoonAz -= 0.1f; break;
        case ']': gMoonAz += 0.1f; break;
        case '-': gMoonEl = clampf(gMoonEl - 0.05f, 0.05f, 1.5f); break;
        case '=':
        case '+': gMoonEl = clampf(gMoonEl + 0.05f, 0.05f, 1.5f); break;
        default: break;
    }
    updateTitle();
    glutPostRedisplay();
}

static void keyUp(unsigned char key, int, int) {
    if (key == ' ') gBrake = false;
}

static void specialDown(int key, int, int) {
    switch (key) {
        case GLUT_KEY_UP:    gUp = true; break;
        case GLUT_KEY_DOWN:  gDown = true; break;
        case GLUT_KEY_LEFT:  gLeft = true; break;
        case GLUT_KEY_RIGHT: gRight = true; break;
        default: break;
    }
}

static void specialUp(int key, int, int) {
    switch (key) {
        case GLUT_KEY_UP:    gUp = false; break;
        case GLUT_KEY_DOWN:  gDown = false; break;
        case GLUT_KEY_LEFT:  gLeft = false; break;
        case GLUT_KEY_RIGHT: gRight = false; break;
        default: break;
    }
}

static void mouse(int button, int state, int x, int y) {
    if (button == GLUT_LEFT_BUTTON) {
        gDragging = (state == GLUT_DOWN);
        gLastX = x; gLastY = y;
    } else if (state == GLUT_DOWN && button == 3) {     
        gOrbitDist = std::max(4.f, gOrbitDist - 1.f);
    } else if (state == GLUT_DOWN && button == 4) {      
        gOrbitDist = std::min(40.f, gOrbitDist + 1.f);
    }
    glutPostRedisplay();
}

static void motion(int x, int y) {
    if (!gDragging) return;
    if (gChaseCam) {                                      
        gChaseCam = false;
        gOrbitYaw = gCar.yaw + 3.14159f;
    }
    gOrbitYaw -= (float)(x - gLastX) * 0.006f;
    gOrbitPitch = clampf(gOrbitPitch + (float)(y - gLastY) * 0.006f, 0.05f, 1.4f);
    gLastX = x; gLastY = y;
    glutPostRedisplay();
}

int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitContextVersion(3, 3);
    glutInitContextProfile(GLUT_CORE_PROFILE);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
    glutInitWindowSize(1280, 720);
    glutCreateWindow("Night drive - preliminary demo");

    glewExperimental = GL_TRUE;
    GLenum err = glewInit();
    if (err != GLEW_OK) {
        std::fprintf(stderr, "ERROR: glewInit failed: %s\n", (const char*)glewGetErrorString(err));
        return 1;
    }
    glGetError();  

    if (!initGL()) return 1;

    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutKeyboardFunc(keyDown);
    glutKeyboardUpFunc(keyUp);
    glutSpecialFunc(specialDown);
    glutSpecialUpFunc(specialUp);
    glutMouseFunc(mouse);
    glutMotionFunc(motion);
    glutIgnoreKeyRepeat(1);

    gLastTime = glutGet(GLUT_ELAPSED_TIME);
    gFpsTime = gLastTime;
    updateTitle();
    glutIdleFunc(idle);
    glutMainLoop();
    return 0;
}
