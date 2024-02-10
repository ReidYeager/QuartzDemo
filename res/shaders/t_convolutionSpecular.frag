#version 450

layout (set = 1, binding = 0) uniform sampler2D texHdri;
layout(set = 1, binding = 1) uniform MaterialStruct
{
    float roughness;
} mat;

layout(location = 0) in vec2 inUv;

layout(location = 0) out vec4 outColor; 

const vec2 invAtan = vec2(0.1591, 0.3183);
const float pi = 3.14159;

vec3 UvToSphericalMap(vec2 uv)
{
    uv.y = 1.0 - uv.y;
    uv -= 0.5;
    uv /= invAtan;
        
    float cosY = cos(uv.y);
    return vec3(cos(uv.x) * cosY, sin(uv.y), sin(uv.x) * cosY);
}

vec2 SphericalMapToUv(vec3 vector)
{
    vec2 uv = vec2(atan(vector.z, vector.x), asin(vector.y));
    uv *= invAtan;
    uv += 0.5;
    uv.y = 1.0 - uv.y;
    return uv;
}

float RadicalInverseVdc(uint bits)
{
    bits = (bits << 16u) | (bits >> 16u);
    bits = ((bits & 0x55555555u) << 1u) | ((bits & 0xAAAAAAAAu) >> 1u);
    bits = ((bits & 0x33333333u) << 2u) | ((bits & 0xCCCCCCCCu) >> 2u);
    bits = ((bits & 0x0F0F0F0Fu) << 4u) | ((bits & 0xF0F0F0F0u) >> 4u);
    bits = ((bits & 0x00FF00FFu) << 8u) | ((bits & 0xFF00FF00u) >> 8u);
    return float(bits) * 2.3283064365386963e-10;
}

vec2 Hammersley(uint i, uint n)
{
    return vec2(float(i) / float(n), RadicalInverseVdc(i));
}

vec3 ImportanceSampleGgx(vec2 xi, vec3 n, float rough)
{
    float a = rough * rough;
    float phi = 2.0 * pi * xi.x;
    float cosTheta = sqrt((1.0 - xi.y) / (1.0 + (a * a - 1.0) * xi.y));
    float sinTheta = sqrt(1.0 - cosTheta * cosTheta);

    vec3 h = vec3(cos(phi) * sinTheta, sin(phi) * sinTheta, cosTheta);

    vec3 up = abs(n.z) < 0.999 ? vec3(0.0, 0.0, 1.0) : vec3(1.0, 0.0, 0.0);
    vec3 tangent = normalize(cross(up, n));
    vec3 bitan = cross(n, tangent);

    vec3 sampleVec = tangent * h.x + bitan * h.y + n * h.z;
    return normalize(sampleVec);
}

void main()
{
    vec3 n = normalize(UvToSphericalMap(inUv));
    vec3 r = n;
    vec3 v = n;
    float rough = mat.roughness;

    const uint sampleCount = 4096;
    float totalWeight = 0.0;
    vec3 prefilteredColor = vec3(0.0);
    for (uint i = 0; i < sampleCount; i++)
    {
        vec2 xi = Hammersley(i, sampleCount);
        vec3 h = ImportanceSampleGgx(xi, n, rough);
        vec3 l = normalize(2.0 * dot(v, h) * h - v);
        
        float ndotl = max(dot(n, l), 0.0);
        if (ndotl > 0.0)
        {
            prefilteredColor += texture(texHdri, SphericalMapToUv(normalize(l))).rgb * ndotl;
            totalWeight += ndotl;
        }
    }
    prefilteredColor /= totalWeight;

    outColor = vec4(prefilteredColor, 1.0);
}