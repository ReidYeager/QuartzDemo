#version 450

layout(set = 1, binding = 0) uniform sampler2D testTex;

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec2 inUv;
layout(location = 2) in vec3 inNormal;

layout(location = 0) out vec4 outColor;

void main()
{
    outColor = vec4(texture(testTex, inUv).xyz, 1.0);
}