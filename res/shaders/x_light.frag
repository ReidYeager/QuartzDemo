#version 450

layout(set = 1, binding = 0) uniform MaterialStruct
{
    vec3 color;
} mat;

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec2 inUv;
layout(location = 2) in vec3 inNormal;

layout(location = 0) out vec4 outColor;

void main()
{
    outColor = vec4(mat.color, 1.0);
}