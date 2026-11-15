#version 330 core

// Night Drive - unified Forward+ & Brute-Force lighting shader.
// uUseClustered / uLightingMode = 0: CPU-fed brute-force lighting
// uUseClustered / uLightingMode = 1: Clustered Forward+ path via GPU TBOs

in vec3 vWorldPos;
in vec3 vViewPos;
in vec3 vNormal;

out vec4 FragColor;

const int MAX_STREET = 64;
struct PointLight {
    vec3 position;
    vec3 color;
    float intensity;
    float radius;
};

#define MAX_LIGHTS 64
uniform int uNumLights;
uniform int uUseClustered;
uniform int uLightingMode;
uniform PointLight uLights[MAX_LIGHTS];
uniform vec3 uCameraPos;
uniform vec2 uScreenSize;
uniform float uZNear;
uniform float uZFar;
uniform ivec3 uGridDims; // 16 x 9 x 8

uniform isamplerBuffer uClusterRanges;
uniform isamplerBuffer uClusterIndices;
uniform isamplerBuffer uClusterRangesTBO;
uniform isamplerBuffer uLightIndicesTBO;

int computeClusterIndex(vec2 fragCoord, float viewZ) {
    int gx = max(uGridDims.x, 16);
    int gy = max(uGridDims.y, 9);
    int gz = max(uGridDims.z, 8);
    vec2 screen = max(uScreenSize, vec2(1280.0, 720.0));
    float zNear = max(uZNear, 0.1);
    float zFar  = max(uZFar, 1000.0);

    int cx = clamp(int(fragCoord.x / (screen.x / float(gx))), 0, gx - 1);
    int cy = clamp(int(fragCoord.y / (screen.y / float(gy))), 0, gy - 1);
    float zClamped = clamp(-viewZ, zNear, zFar);
    float sliceF = log(zClamped / zNear) / log(zFar / zNear) * float(gz);
    int cz = clamp(int(sliceF), 0, gz - 1);
    return cx + cy * gx + cz * (gx * gy);
}

vec3 evalBlinnPhong(PointLight light, vec3 N, vec3 worldPos, vec3 V) {
    vec3 Lvec = light.position - worldPos;
    float dist = length(Lvec);
    if (dist >= light.radius) return vec3(0.0);

    vec3 L = Lvec / max(dist, 0.0001);
    float atten = clamp(1.0 - (dist / light.radius), 0.0, 1.0);
    atten *= atten;

    float diff = max(dot(N, L), 0.0);
    vec3 H = normalize(L + V);
    float spec = pow(max(dot(N, H), 0.0), 32.0);
    return (diff + 0.4 * spec) * light.color * light.intensity * atten;
}

void main() {
    vec3 N = normalize(vNormal);
    vec3 V = normalize(uCameraPos - vWorldPos);
    vec3 color = vec3(0.04, 0.05, 0.08);

    bool useClustered = (uUseCluster