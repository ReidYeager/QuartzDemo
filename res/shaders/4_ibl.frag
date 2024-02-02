#version 450

struct LightDirectional
{
    vec3 color;
    float intensity;
    vec3 direction;
};

struct LightPoint
{
    vec3 color;
    float intensity;
    vec3 position;
};

struct LightSpot
{
    vec3 color;
    float intensity;
    vec3 position;
    vec3 direction;

    float cosInner;
    float cosOuter;
};

struct LightGroup
{
    LightDirectional directional;

    uint pointCount;
    LightPoint points[4];

    uint spotCount;
    LightSpot spots[2];
};

layout(set = 0, binding = 0) uniform SceneStruct
{
    mat4 viewProj;
    vec3 campos;
    vec3 camForward;
    vec3 ambient;
    LightGroup lights;
} scene;

layout (set = 1, binding = 0) uniform MaterialStruct
{
    vec3 baseReflectivity;
    float roughness;
    float metalness;
} mat;
layout (set = 1, binding = 1) uniform sampler2D texAlbedo;
layout (set = 1, binding = 2) uniform sampler2D texNormal;
layout (set = 1, binding = 3) uniform sampler2D texMaps;

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec2 inUv;
layout(location = 2) in vec3 inNormal;
layout(location = 3) in vec3 inTangent;
layout(location = 4) in mat3 inTBN;

layout(location = 0) out vec4 outColor;

// Main
// ============================================================

void main()
{
    outColor = vec4(1.0);
}
