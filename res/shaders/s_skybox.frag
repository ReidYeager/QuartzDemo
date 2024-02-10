#version 450

layout(set = 0, binding = 0) uniform SceneStruct
{
    mat4 viewProj;
} scene;

layout (set = 1, binding = 0) uniform sampler2D texHdri;

layout(location = 0) in vec3 inNormal;

layout(location = 0) out vec4 outColor;

const vec2 invAtan = vec2(0.1591, 0.3183);
vec2 SphericalMapToUv(vec3 vector)
{
    vec2 uv = vec2(atan(vector.z, vector.x), asin(vector.y));
    uv *= invAtan;
    uv += 0.5;
    uv.y = 1.0 - uv.y;
    return uv;
}

void main()
{
    vec3 n = normalize(inNormal);
    vec3 c = texture(texHdri, SphericalMapToUv(n)).xyz;
    outColor = vec4(c, 1.0);
}