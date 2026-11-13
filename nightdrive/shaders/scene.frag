#version 330 core

in vec3 vWorldPos;
in vec3 vViewPos;
in vec3 vNormal;

out vec4 FragColor;

struct PointLight {
    vec3 position;
    vec3 color;
    float intensity;
    float radius;
};

#define MAX_LIGHTS 64
uniform int uNumLights;
uniform PointLight uLights[MAX_LIGHTS];
uniform vec3 uCameraPos;
uniform vec2 uScreenSize;
uniform float uZNear;
uniform float uZFar;
uniform ivec3 uGridDims; // 16 x 9 x 8

uniform isamplerBuffer uClusterRangesTBO; // RG32I: offset, count
uniform isamplerBuffer uLightIndicesTBO;  // R32I:  lightIndex

int computeClusterIndex(vec2 fragCoord, float viewZ) {
    int cx = clamp(int(fragCoord.x / (uScreenSize.x / float(uGridDims.x))), 0, uGridDims.x - 1);
    int cy = clamp(int(fragCoord.y / (uScreenSize.y / float(uGridDims.y))), 0, uGridDims.y - 1);
    float zClamped = clamp(-viewZ, uZNear, uZFar);
    float sliceF = log(zClamped / uZNear) / log(uZFar / uZNear) * float(uGridDims.z);
    int cz = clamp(int(sliceF), 0, uGridDims.z - 1);
    return cx + cy * uGridDims.x + cz * (uGridDims.x * uGridDims.y);
}

vec3 evalBlinnPhong(PointLight light, vec3 N, vec3 worldPos, vec3 V) {
    vec3 Lvec = light.position - worldPos;
    float dist = length(Lvec);
    if (dist >= light.radius) return vec3(0.0)










;

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

    int clusterIdx = computeClusterIndex(gl_FragCoord.xy, vViewPos.z);
    ivec2 range = texelFetch(uClusterRangesTBO, clusterIdx).xy;

    for (int i = 0; i < range.y; ++i) {
        int lightId = texelFetch(uLightIndicesTBO, range.x + i).x;
        if (lightId >= 0 && lightId < uNumLights) {
 








           color += evalBlinnPhong(uLights[lightId], N, vWorldPos, V);
        }
    }

    FragColor = vec4(color, 1.0);
}
