#version 450

layout(set = 0, binding = 0) uniform SceneStruct
{
    mat4 viewProj;
} scene;

layout(location = 0) in vec3 inPosition;

layout(location = 0) out vec3 outNormal;

void main()
{
    mat4 viewProjRot = mat4(mat3(scene.viewProj));
    vec4 pos = vec4(inPosition.xy, 1.0, 1.0);

    vec4 throughPixel = inverse(mat4(mat3(scene.viewProj))) * pos;
    outNormal = normalize(throughPixel.xyz);
    gl_Position = pos;
}