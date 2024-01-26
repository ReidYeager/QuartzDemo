#version 450

layout(set = 0, binding = 0) uniform SceneStruct
{
    mat4 viewProj;
    int a;
    int b;
    int c;
    int d;

    vec3 v;
    uint q;
} scene;

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec2 inUv;
layout(location = 2) in vec3 inNormal;
layout(location = 3) in vec3 inTangent;
layout(location = 4) in mat3 inTBN;

layout(location = 0) out vec4 outColor;

void main()
{
    outColor = vec4(0.6, 0.12, 0.83, 1.0);
}