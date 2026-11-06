#pragma once
#include <cmath>

struct vec3 {
    float x, y, z;
    vec3(float x_ = 0.f, float y_ = 0.f, float z_ = 0.f) : x(x_), y(y_), z(z_) {}
    vec3 operator+(const vec3& o) const { return vec3(x + o.x, y + o.y, z + o.z); }
    vec3 operator-(const vec3& o) const { return vec3(x - o.x, y - o.y, z - o.z); }
    vec3 operator*(float s) const { return vec3(x * s, y * s, z * s); }
    vec3 operator-() const { return vec3(-x, -y, -z); }
};
inline float dot(const vec3& a, const vec3& b) { return a.x * b.x + a.y * b.y + a.z * b.z; }
inline vec3 cross(const vec3& a, const vec3& b) {
    return vec3(a.y * b.z - a.z * b.y, a.z * b.x - a.x * b.z, a.x * b.y - a.y * b.x);
}
inline float length(const vec3& v) { return std::sqrt(dot(v, v)); }
inline vec3 normalize(const vec3& v) {
    float l = length(v);
    return l > 1e-8f ? v * (1.f / l) : vec3(0.f, 0.f, 0.f);
}
inline float radians(float deg) { return deg * 3.14159265358979f / 180.f; }

struct mat4 {
    float m[16];  // m[col * 4 + row]
    mat4() { for (int i = 0; i < 16; ++i) m[i] = (i % 5 == 0) ? 1.f : 0.f; }
};

inline mat4 operator*(const mat4& a, const mat4& b) {
    mat4 r;
    for (int c = 0; c < 4; ++c)
        for (int row = 0; row < 4; ++row) {
            float s = 0.f;
            for (int k = 0; k < 4; ++k) s += a.m[k * 4 + row] * b.m[c * 4 + k];
            r.m[c * 4 + row] = s;
        }
    return r;
}

inline mat4 translate(const vec3& t) {
    mat4 r;
    r.m[12] = t.x; r.m[13] = t.y; r.m[14] = t.z;
    return r;
}
inline mat4 scale(const vec3& s) {
    mat4 r;
    r.m[0] = s.x; r.m[5] = s.y; r.m[10] = s.z;
    return r;
}

inline mat4 rotateY(float a) {
    mat4 r;
    float c = std::cos(a), s = std::sin(a);
    r.m[0] = c;  r.m[8] = s;
    r.m[2] = -s; r.m[10] = c;
    return r;
}
inline vec3 transformPoint(const mat4& M, const vec3& p) {
    return vec3(M.m[0] * p.x + M.m[4] * p.y + M.m[8] * p.z + M.m[12],
                M.m[1] * p.x + M.m[5] * p.y + M.m[9] * p.z + M.m[13],
                M.m[2] * p.x + M.m[6] * p.y + M.m[10] * p.z + M.m[14]);
}

inline mat4 lookAt(const vec3& eye, const vec3& center, const vec3& up) {
    vec3 f = normalize(center - eye);
    vec3 s = normalize(cross(f, up));
    vec3 u = cross(s, f);
    mat4 r;
    r.m[0] = s.x; r.m[4] = s.y; r.m[8]  = s.z;
    r.m[1] = u.x; r.m[5] = u.y; r.m[9]  = u.z;
    r.m[2] = -f.x; r.m[6] = -f.y; r.m[10] = -f.z;
    r.m[12] = -dot(s, eye);
    r.m[13] = -dot(u, eye);
    r.m[14] = dot(f, eye);
    return r;
}

inline mat4 perspective(float fovy, float aspect, float zNear, float zFar) {
    float f = 1.f / std::tan(fovy * 0.5f);
    mat4 r;
    for (int i = 0; i < 16; ++i) r.m[i] = 0.f;
    r.m[0] = f / aspect;
    r.m[5] = f;
    r.m[10] = (zFar + zNear) / (zNear - zFar);
    r.m[11] = -1.f;
    r.m[14] = 2.f * zFar * zNear / (zNear - zFar);
    return r;
}
