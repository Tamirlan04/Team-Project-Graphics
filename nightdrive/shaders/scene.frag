#version 330 core
in vec3 vWorldPos;
in vec3 vNormal;
out vec4 FragColor;

const int MAX_STREET = 64;

uniform vec3  uColor;      
uniform vec3  uEmissive;   
uniform float uSpec;       
uniform float uShin;       
uniform int   uUnlit;      

uniform vec3  uViewPos;
uniform vec3  uAmbient;

uniform vec3  uMoonDir;    
uniform vec3  uMoonColor;

uniform int   uStreetOn;
uniform int   uNumStreet;
uniform vec3  uStreetPos[MAX_STREET];
uniform vec3  uStreetColor;

uniform int   uHeadOn;
uniform vec3  uHeadPos[2];
uniform vec3  uHeadDir;    
uniform vec3  uHeadColor;
uniform float uHeadCosCut; 


vec3 shade(vec3 N, vec3 V, vec3 L, vec3 lightColor, float atten)
{
    float ndl = max(dot(N, L), 0.0);
    vec3 H = normalize(L + V);
    float spec = (ndl > 0.0) ? pow(max(dot(N, H), 0.0), uShin) * uSpec : 0.0;
    return lightColor * atten * (uColor * ndl + vec3(spec));
}


float pointAtten(float d, float range)
{
    float a = 1.0 / (1.0 + 0.09 * d + 0.032 * d * d);
    float w = clamp(1.0 - pow(d / range, 4.0), 0.0, 1.0);
    return a * w * w;
}

void main()
{
    if (uUnlit == 1) {
        FragColor = vec4(uColor, 1.0);
        return;
    }

    vec3 N = normalize(vNormal);
    vec3 V = normalize(uViewPos - vWorldPos);

    vec3 color = uAmbient * uColor + uEmissive;
    color += shade(N, V, normalize(uMoonDir), uMoonColor, 1.0);

    if (uStreetOn == 1) {
        for (int i = 0; i < MAX_STREET; ++i) {
            if (i >= uNumStreet) break;
            vec3 toLight = uStreetPos[i] - vWorldPos;
            float d = length(toLight);
            color += shade(N, V, toLight / d, uStreetColor, pointAtten(d, 30.0));
        }
    }

    if (uHeadOn == 1) {
        for (int i = 0; i < 2; ++i) {
            vec3 toLight = uHeadPos[i] - vWorldPos;
            float d = length(toLight);
            vec3 L = toLight / d;
            float cosA = dot(-L, uHeadDir);
            if (cosA > uHeadCosCut) {
                float cone = pow(cosA, 12.0);
                color += shade(N, V, L, uHeadColor, pointAtten(d, 45.0) * cone);
            }
        }
    }

    FragColor = vec4(clamp(color, 0.0, 1.0), 1.0);
}
