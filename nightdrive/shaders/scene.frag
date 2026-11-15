#version 330 core

// Night Drive - integration-safe lighting shader.
// uUseClustered = 0: functional CPU-fed brute-force lighting (current main.cpp).
// uUseClustered = 1: clustered path; requires GPU buffers and uniforms to be bound.

in vec3 vWorldPos;
in vec3 vViewPos;
in vec3 vNormal;
out vec4 FragColor;

// Scene/material uniforms provided by main.cpp.
uniform vec3 uColor;
uniform vec3 uEmissive;
uniform float uSpec;
uniform float uShin;
uniform int uUnlit;
uniform vec3 uViewPos;
uniform vec3 uAmbient;
uniform vec3 uMoonDir;
uniform vec3 uMoonColor;

// CPU-fed brute-force street lights and headlights.
uniform int uStreetOn;
uniform int uNumStreet;
uniform vec3 uStreetPos[64];
uniform vec3 uStreetColor;
uniform int uHeadOn;
uniform vec3 uHeadPos[2];
uniform vec3 uHeadDir;
uniform vec3 uHeadColor;
uniform float uHeadCosCut;

// Clustered Forward+ resources (not yet bound by current main.cpp).
// These names and sampler types match ClusterGpuBuffers::bind() and its GL_*UI formats.
uniform int uUseClustered;
uniform int uNumLights;
uniform vec2 uScreenSize;
uniform float uZNear;
uniform float uZFar;
uniform ivec3 uGridDims;
uniform usamplerBuffer uClusterRanges; // RG32UI: offset/count
uniform usamplerBuffer uClusterIndices; // R32UI: light indices
uniform samplerBuffer uClusterLights;   // RGBA32F: two texels per light

vec3 evaluatePoint(vec3 lightPos, vec3 lightColor, float intensity,
                   float radius, vec3 N, vec3 V) {
    vec3 toLight = lightPos - vWorldPos;
    float dist = length(toLight);
    if (dist >= radius || radius <= 0.0) return vec3(0.0);

    vec3 L = toLight / max(dist, 0.0001);
    float diffuse = max(dot(N, L), 0.0);
    vec3 H = normalize(L + V);
    float specular = uSpec * pow(max(dot(N, H), 0.0), max(uShin, 1.0));
    float falloff = 1.0 - dist / radius;
    falloff *= falloff;
    return lightColor * intensity * (diffuse + specular) * falloff;
}

int computeClusterIndex(vec2 pixel, float viewZ) {
    ivec3 dims = max(uGridDims, ivec3(1));
    vec2 size = max(uScreenSize, vec2(1.0));
    int cx = clamp(int(pixel.x / (size.x / float(dims.x))), 0, dims.x - 1);
    int cy = clamp(int(pixel.y / (size.y / float(dims.y))), 0, dims.y - 1);
    float nearP = max(uZNear, 0.0001);
    float farP = max(uZFar, nearP + 0.01);
    float depth = clamp(-viewZ, nearP, farP);
    float slice = log(depth / nearP) / log(farP / nearP) * float(dims.z);
    int cz = clamp(int(slice), 0, dims.z - 1);
    return cx + cy * dims.x + cz * dims.x * dims.y;
}

void main() {
    // Stars, moon and other explicitly unlit objects keep their original colors.
    if (uUnlit != 0) {
        FragColor = vec4(uColor + uEmissive, 1.0);
        return;
    }

    vec3 N = normalize(vNormal);
    vec3 V = normalize(uViewPos - vWorldPos);
    vec3 lightSum = uAmbient;
    lightSum += uMoonColor * max(dot(N, normalize(uMoonDir)), 0.0);

    if (uUseClustered != 0) {
        // Forward+ path is available, but main.cpp must bind/upload resources
        // and set uUseClustered=1 before this mode can be used.
        int cluster = computeClusterIndex(gl_FragCoord.xy, vViewPos.z);
        uvec2 range = texelFetch(uClusterRanges, cluster).rg;
        int count = min(int(range.y), 64);
        for (int i = 0; i < count; ++i) {
            uint id = texelFetch(uClusterIndices, int(range.x) + i).r;
            if (id < uint(uNumLights)) {
                vec4 positionRadius = texelFetch(uClusterLights, int(id) * 2);
                vec4 colorIntensity = texelFetch(uClusterLights, int(id) * 2 + 1);
                lightSum += evaluatePoint(positionRadius.xyz, colorIntensity.rgb,
                                          colorIntensity.a, positionRadius.w, N, V);
            }
        }
    } else if (uStreetOn != 0) {
        // Existing app currently supplies these uniforms, not GPU cluster buffers.
        for (int i = 0; i < min(uNumStreet, 64); ++i) {
            lightSum += evaluatePoint(uStreetPos[i], uStreetColor, 1.0,
                                      30.0, N, V);
        }
    }

    // Keep vehicle headlights functional in BOTH modes.
    if (uHeadOn != 0) {
        vec3 direction = normalize(uHeadDir);
        for (int i = 0; i < 2; ++i) {
            vec3 fromHeadlight = vWorldPos - uHeadPos[i];
            float d = length(fromHeadlight);
            if (d < 45.0 && d > 0.0001) {
                float cone = dot(normalize(fromHeadlight), direction);
                if (cone > uHeadCosCut) {
                    float spot = smoothstep(uHeadCosCut, min(1.0, uHeadCosCut + 0.12), cone);
                    lightSum += evaluatePoint(uHeadPos[i], uHeadColor, 1.0,
                                              45.0, N, V) * spot;
                }
            }
        }
    }

    vec3 result = uColor * lightSum + uEmissive;
    FragColor = vec4(max(result, vec3(0.0)), 1.0);
}
