#version 450

layout(set = 0, binding = 0) uniform SceneStruct
{
    mat4 viewProj;
} scene;

layout(push_constant) uniform PustConstant
{
    mat4 objectTransform;
} pushConst;

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec2 inUv;
layout(location = 2) in vec3 inNormal;
layout(location = 3) in vec3 inTangent;

layout(location = 0) out vec3 outPosition;
layout(location = 1) out vec2 outUv;
layout(location = 2) out vec3 outNormal;
layout(location = 3) out vec3 outTangent;
layout(location = 4) out mat3 outTBN;

void main()
{
    vec4 worldPosition = pushConst.objectTransform * vec4(inPosition, 1.0);
    gl_Position = scene.viewProj * worldPosition;
    
    outPosition = worldPosition.xyz;
    outUv = inUv;

    mat3 normalMatrix = mat3(transpose(inverse(pushConst.objectTransform)));
    outNormal = normalize(normalMatrix * inNormal);
    outTangent = normalize(normalMatrix * inTangent);
    vec3 bitan = normalize(cross(outNormal, outTangent));
    outTBN = transpose(mat3(outTangent, bitan, outNormal));
}