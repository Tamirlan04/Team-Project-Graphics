#include "nightdrive_hud.h"
#include <GL/glew.h>
#include <algorithm>
#include <array>
#include <cctype>
#include <cstdio>
#include <cstdint>
#include <map>
#include <string>
#include <vector>
#include <cstddef>

namespace {
struct Color { float r, g, b, a; };
struct Vertex { float x, y, r, g, b, a; };
using Glyph = std::array<unsigned char, 7>;

GLuint hudProgram = 0, hudVao = 0, hudVbo = 0;
GLint hudSizeLocation = -1;
std::vector<Vertex> hudVertices;

const std::map<char, Glyph>& glyphs() {
    static const std::map<char, Glyph> font = {
        {'A', {{0x0E,0x11,0x11,0x1F,0x11,0x11,0x11}}},
        {'B', {{0x1E,0x11,0x11,0x1E,0x11,0x11,0x1E}}},
        {'C', {{0x0E,0x11,0x10,0x10,0x10,0x11,0x0E}}},
        {'D', {{0x1C,0x12,0x11,0x11,0x11,0x12,0x1C}}},
        {'E', {{0x1F,0x10,0x10,0x1E,0x10,0x10,0x1F}}},
        {'F', {{0x1F,0x10,0x10,0x1E,0x10,0x10,0x10}}},
        {'G', {{0x0E,0x11,0x10,0x17,0x11,0x11,0x0E}}},
        {'H', {{0x11,0x11,0x11,0x1F,0x11,0x11,0x11}}},
        {'I', {{0x1F,0x04,0x04,0x04,0x04,0x04,0x1F}}},
        {'J', {{0x07,0x02,0x02,0x02,0x12,0x12,0x0C}}},
        {'K', {{0x11,0x12,0x14,0x18,0x14,0x12,0x11}}},
        {'L', {{0x10,0x10,0x10,0x10,0x10,0x10,0x1F}}},
        {'M', {{0x11,0x1B,0x15,0x15,0x11,0x11,0x11}}},
        {'N', {{0x11,0x19,0x15,0x13,0x11,0x11,0x11}}},
        {'O', {{0x0E,0x11,0x11,0x11,0x11,0x11,0x0E}}},
        {'P', {{0x1E,0x11,0x11,0x1E,0x10,0x10,0x10}}},
        {'Q', {{0x0E,0x11,0x11,0x11,0x15,0x12,0x0D}}},
        {'R', {{0x1E,0x11,0x11,0x1E,0x14,0x12,0x11}}},
        {'S', {{0x0F,0x10,0x10,0x0E,0x01,0x01,0x1E}}},
        {'T', {{0x1F,0x04,0x04,0x04,0x04,0x04,0x04}}},
        {'U', {{0x11,0x11,0x11,0x11,0x11,0x11,0x0E}}},
        {'V', {{0x11,0x11,0x11,0x11,0x11,0x0A,0x04}}},
        {'W', {{0x11,0x11,0x11,0x15,0x15,0x15,0x0A}}},
        {'X', {{0x11,0x11,0x0A,0x04,0x0A,0x11,0x11}}},
        {'Y', {{0x11,0x11,0x0A,0x04,0x04,0x04,0x04}}},
        {'Z', {{0x1F,0x01,0x02,0x04,0x08,0x10,0x1F}}},
        {'0', {{0x0E,0x11,0x13,0x15,0x19,0x11,0x0E}}},
        {'1', {{0x04,0x0C,0x04,0x04,0x04,0x04,0x0E}}},
        {'2', {{0x0E,0x11,0x01,0x02,0x04,0x08,0x1F}}},
        {'3', {{0x1E,0x01,0x01,0x0E,0x01,0x01,0x1E}}},
        {'4', {{0x02,0x06,0x0A,0x12,0x1F,0x02,0x02}}},
        {'5', {{0x1F,0x10,0x10,0x1E,0x01,0x01,0x1E}}},
        {'6', {{0x0E,0x10,0x10,0x1E,0x11,0x11,0x0E}}},
        {'7', {{0x1F,0x01,0x02,0x04,0x08,0x08,0x08}}},
        {'8', {{0x0E,0x11,0x11,0x0E,0x11,0x11,0x0E}}},
        {'9', {{0x0E,0x11,0x11,0x0F,0x01,0x01,0x0E}}},
        {':', {{0,0x04,0x04,0,0x04,0x04,0}}},
        {'-', {{0,0,0,0x1F,0,0,0}}},
        {'/', {{0x01,0x01,0x02,0x04,0x08,0x10,0x10}}},
        {'|', {{0x04,0x04,0x04,0x04,0x04,0x04,0x04}}},
        {'.', {{0,0,0,0,0,0x0C,0x0C}}},
        {',', {{0,0,0,0,0x04,0x04,0x08}}},
        {' ', {{0,0,0,0,0,0,0}}},
        {'?', {{0x0E,0x11,0x01,0x02,0x04,0,0x04}}}
    };
    return font;
}

void rectangle(float x, float y, float w, float h, Color color) {
    const Vertex a{x, y, color.r, color.g, color.b, color.a};
    const Vertex b{x+w, y, color.r, color.g, color.b, color.a};
    const Vertex c{x+w, y+h, color.r, color.g, color.b, color.a};
    const Vertex d{x, y+h, color.r, color.g, color.b, color.a};
    hudVertices.insert(hudVertices.end(), {a,b,c,a,c,d});
}

void drawText(const std::string& text, float x, float y, float pixel, Color color) {
    float pen = x;
    for (unsigned char c : text) {
        const char uc = static_cast<char>(std::toupper(c));
        const auto it = glyphs().find(uc);
        const Glyph& glyph = (it != glyphs().end()) ? it->second : glyphs().at('?');
        for (int row = 0; row < 7; ++row) {
            for (int col = 0; col < 5; ++col) {
                if (glyph[row] & (1u << (4 - col)))
                    rectangle(pen + col * pixel, y + row * pixel, pixel, pixel, color);
            }
        }
        pen += 6.f * pixel;
    }
}

GLuint compile(GLenum shaderType, const char* source) {
    GLuint shader = glCreateShader(shaderType);
    glShaderSource(shader, 1, &source, nullptr);
    glCompileShader(shader);
    GLint ok = GL_FALSE;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &ok);
    if (!ok) {
        char log[1024] = {};
        glGetShaderInfoLog(shader, sizeof(log), nullptr, log);
        std::fprintf(stderr, "HUD shader failed: %s\n", log);
        glDeleteShader(shader);
        return 0;
    }
    return shader;
}
} // namespace

bool initNightDriveHud() {
    static const char* vertexShader = R"glsl(#version 330 core
layout(location = 0) in vec2 aPosition;
layout(location = 1) in vec4 aColor;
uniform vec2 uScreenSize;
out vec4 vColor;
void main() {
    vec2 xy = vec2(aPosition.x / uScreenSize.x * 2.0 - 1.0,
                   1.0 - aPosition.y / uScreenSize.y * 2.0);
    gl_Position = vec4(xy, 0.0, 1.0);
    vColor = aColor;
}
)glsl";
    static const char* fragmentShader = R"glsl(#version 330 core
in vec4 vColor;
out vec4 FragColor;
void main() { FragColor = vColor; }
)glsl";

    GLuint vs = compile(GL_VERTEX_SHADER, vertexShader);
    GLuint fs = compile(GL_FRAGMENT_SHADER, fragmentShader);
    if (!vs || !fs) {
        if (vs) glDeleteShader(vs);
        if (fs) glDeleteShader(fs);
        return false;
    }
    hudProgram = glCreateProgram();
    glAttachShader(hudProgram, vs);
    glAttachShader(hudProgram, fs);
    glLinkProgram(hudProgram);
    glDeleteShader(vs);
    glDeleteShader(fs);
    GLint ok = GL_FALSE;
    glGetProgramiv(hudProgram, GL_LINK_STATUS, &ok);
    if (!ok) {
        char log[1024] = {};
        glGetProgramInfoLog(hudProgram, sizeof(log), nullptr, log);
        std::fprintf(stderr, "HUD program link failed: %s\n", log);
        glDeleteProgram(hudProgram);
        hudProgram = 0;
        return false;
    }
    hudSizeLocation = glGetUniformLocation(hudProgram, "uScreenSize");
    glGenVertexArrays(1, &hudVao);
    glGenBuffers(1, &hudVbo);
    glBindVertexArray(hudVao);
    glBindBuffer(GL_ARRAY_BUFFER, hudVbo);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                          reinterpret_cast<void*>(offsetof(Vertex, x)));
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                          reinterpret_cast<void*>(offsetof(Vertex, r)));
    glEnableVertexAttribArray(1);
    glBindVertexArray(0);
    return true;
}

void drawNightDriveHud(int width, int height, int fps, float speed,
                       bool paused, bool streetLightsOn, bool headlightsOn) {
    if (!hudProgram || width <= 0 || height <= 0) return;
    const float zoom = std::min(1.f, std::min((width - 24.f) / 630.f, (height - 20.f) / 210.f));
    if (zoom <= 0.f) return;
    const float left = 14.f, top = 14.f;
    const auto X = [&](float x) { return left + x * zoom; };
    const auto Y = [&](float y) { return top + y * zoom; };
    const Color background{0.025f, 0.05f, 0.1f, 0.86f};
    const Color white{0.97f, 0.98f, 1.f, 1.f};
    const Color muted{0.78f, 0.84f, 0.91f, 1.f};
    const Color blue{0.24f, 0.66f, 0.93f, 1.f};
    const Color amber{1.f, 0.83f, 0.42f, 1.f};
    const Color green{0.6f, 0.95f, 0.77f, 1.f};
    hudVertices.clear();
    rectangle(X(0), Y(0), 610.f * zoom, 188.f * zoom, background);
    rectangle(X(0), Y(0), 5.f * zoom, 188.f * zoom, blue);
    rectangle(X(18), Y(54), 573.f * zoom, 1.f * zoom, Color{0.3f,0.4f,0.58f,0.7f});
    drawText("NIGHT DRIVE", X(18), Y(15), 3.f * zoom, white);
    drawText("TOPIC 7", X(405), Y(25), 2.f * zoom, amber);
    drawText("TEAM: ASKARULY TEMIRLAN / TAMERLAN MURAT", X(18), Y(68), 1.8f * zoom, muted);
    drawText("TEAM: TAMIRLAN", X(18), Y(90), 1.8f * zoom, muted);

    char stats[120] = {};
    std::snprintf(stats, sizeof(stats), "FPS: %d   SPEED: %.0f", fps, speed);
    drawText(stats, X(18), Y(119), 2.6f * zoom, white);
    std::string status = paused ? "PAUSED" : "RUNNING";
    status += streetLightsOn ? "  STREET: ON" : "  STREET: OFF";
    status += headlightsOn ? "  HEAD: ON" : "  HEAD: OFF";
    drawText(status, X(18), Y(162), 1.8f * zoom, paused ? amber : green);

    glDisable(GL_DEPTH_TEST);
    glDisable(GL_CULL_FACE);
    glDepthMask(GL_FALSE);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glUseProgram(hudProgram);
    glUniform2f(hudSizeLocation, static_cast<float>(width), static_cast<float>(height));
    glBindVertexArray(hudVao);
    glBindBuffer(GL_ARRAY_BUFFER, hudVbo);
    glBufferData(GL_ARRAY_BUFFER,
                 static_cast<GLsizeiptr>(hudVertices.size() * sizeof(Vertex)),
                 hudVertices.data(), GL_STREAM_DRAW);
    glDrawArrays(GL_TRIANGLES, 0, static_cast<GLsizei>(hudVertices.size()));
    glBindVertexArray(0);
    glUseProgram(0);
    glDisable(GL_BLEND);
    glDepthMask(GL_TRUE);
    glEnable(GL_CULL_FACE);
    glEnable(GL_DEPTH_TEST);
}
