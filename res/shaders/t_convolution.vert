#version 450

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec2 inUv;
layout(location = 2) in vec3 inNormal;
layout(location = 3) in vec3 inTangent;

layout(location = 0) out vec2 outUv;

void main()
{
    outUv = inUv;
    gl_Position = vec4(inPosition, 1.0);
}