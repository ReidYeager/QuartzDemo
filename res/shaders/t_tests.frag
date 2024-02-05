#version 450

layout(set = 0, binding = 0) uniform SceneStruct
{
    mat4 viewProj;
} scene;

layout(set = 1, binding = 0) uniform MaterialStruct{
    float lod;
} mat;
layout(set = 1, binding = 1) uniform sampler2D tex;

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec2 inUv;
layout(location = 2) in vec3 inNormal;
layout(location = 3) in vec3 inTangent;
layout(location = 4) in mat3 inTBN;

layout(location = 0) out vec4 outColor;

void main()
{
    vec3 c = texture(tex, inUv, mat.lod).xyz;

    outColor = vec4(c, 1.0);
}