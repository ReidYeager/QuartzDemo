#version 450

struct LightDirectional
{
    vec3 color;
    vec3 direction;
};

struct LightPoint
{
    vec3 color;
    vec3 position;

    float linear;
    float quadratic;
};

struct LightSpot
{
    vec3 color;
    vec3 position;
    vec3 direction;

    float cosInner;
    float cosOuter;

    float linear;
    float quadratic;
};

layout(set = 0, binding = 0) uniform SceneStruct
{
    mat4 viewProj;
    vec3 campos;
    vec3 camForward;
    vec3 ambient;
    LightDirectional lightDir;
    LightPoint lightPoint[4];
    LightSpot lightSpot[2];
} scene;

layout (set = 1, binding = 0) uniform sampler2D texAlbedo;
layout (set = 1, binding = 1) uniform sampler2D texNormal;
layout (set = 1, binding = 2) uniform sampler2D texMaps;

layout (set = 1, binding = 3) uniform MaterialStruct
{
    float roughness;
    float metalness;
    vec3 baseReflectivity;
} mat;

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec2 inUv;
layout(location = 2) in vec3 inNormal;
layout(location = 3) in vec3 inTangent;
layout(location = 4) in mat3 inTBN;

layout(location = 0) out vec4 outColor;

// PBR
// ============================================================

#define PI 3.14159

// Trowbridge-Reitz GGX
float DistributionGGX(vec3 N, vec3 H, float roughness)
{
    float a      = roughness*roughness;
    float a2     = a*a;
    float NdotH  = max(dot(N, H), 0.0);
    float NdotH2 = NdotH*NdotH;
	
    float num   = a2;
    float denom = (NdotH2 * (a2 - 1.0) + 1.0);
    denom = PI * denom * denom;
	
    return num / denom;
}

float GeometrySchlickGGX(float NdotV, float roughness)
{
    float r = (roughness + 1.0);
    float k = (r*r) / 8.0;

    float num   = NdotV;
    float denom = NdotV * (1.0 - k) + k;
	
    return num / denom;
}

float GeometryGgx(vec3 normal, vec3 view, vec3 light, float rough)
{
    float ndotv = max(dot(normal, view), 0.0);
    float ndotl = max(dot(normal, light), 0.0);
    return GeometrySchlickGGX(ndotv, rough) * GeometrySchlickGGX(ndotl, rough);
}

// x
// Material base reflectivity chart : https://refractiveindex.info/
vec3 fresnelSchlick(float cosTheta, vec3 f0)
{
    // f0 = surface reflection at zero incidence (directly at surface)
    return f0 + (1.0 - f0) * pow(clamp(1.0 - cosTheta, 0.0, 1.0), 5.0);
}

vec3 DirectLighting(vec3 normal, vec3 dirToCam, float metalness, float roughness, vec3 albedo)
{
    vec3 dirToLight = normalize(scene.lightPoint[0].position - inPosition);
    vec3 halfway = normalize(dirToLight + dirToCam);

    float dist = length(scene.lightPoint[0].position - inPosition);
    float attenuation = 1.0 / (dist);
    vec3 radiance = scene.lightPoint[0].color * attenuation;
    
    vec3 f0 = mix(vec3(0.04), mat.baseReflectivity, metalness);
    vec3 f = fresnelSchlick(max(dot(halfway, dirToCam), 0.0), f0);

    float ndf = DistributionGGX(normal, halfway, roughness);
    float g = GeometryGgx(normal, dirToCam, dirToLight, roughness);

    vec3 num = ndf * g * f;
    float denom = 4.0 * max(dot(normal, dirToCam), 0.0) * max(dot(normal, dirToLight), 0.0) + 0.0001;
    vec3 specular = num / denom;

    vec3 ks = f;
    vec3 kd = vec3(1.0) - ks;
    kd *= 1.0 - metalness;

    float ndotl = max(dot(normal, dirToLight), 0.0);
    return (kd * albedo / PI + specular) * radiance * ndotl;
}

// Main
// ============================================================

void main()
{
    // vec3 tintedMetal = mix(mat.baseReflectivity.xxx, mat.baseReflectivity, mat.metalness);
    vec3 albedo = texture(texAlbedo, inUv).xyz;
    vec3 mats = texture(texMaps, inUv).xyz;
    float rough = 1-mats.y;
    float metal = mats.z;

    vec3 normal = texture(texNormal, inUv).xyz * 2.0 - 1.0;
    normal = normalize(inTBN * normal);
    // normal = normalize(inNormal);

    vec3 dirToCam = normalize(scene.campos - inPosition);
    vec3 direct = DirectLighting(normal, dirToCam, metal, rough, albedo);

    vec3 light = direct;
    light *= mats.x;

    vec3 finalColor = light;

    outColor = vec4(finalColor, 1.0);
}