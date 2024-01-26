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

// PBR
// ============================================================

#define PI 3.14159

float DistributionGGX(vec3 N, vec3 H, float roughness)
{
    float a      = roughness*roughness;
    float a2     = a*a;
    float NdotH  = max(dot(N, H), 0.0);
    float NdotH2 = NdotH*NdotH;
	
    float denom = (NdotH2 * (a2 - 1.0) + 1.0);
    denom = PI * denom * denom;
	
    return a2 / denom;
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

vec3 fresnelSchlick(float cosTheta, vec3 f0)
{
    // f0 = surface reflection at zero incidence (directly at surface)
    return f0 + (1.0 - f0) * pow(clamp(1.0 - cosTheta, 0.0, 1.0), 5.0);
}

// Lights
// ============================================================

vec3 DirLight(vec3 albedo, vec3 normal, float rough, float metal)
{
    LightDirectional pl = scene.lights.directional;

    vec3 dirToCam = normalize(scene.campos - inPosition);
    vec3 dirToLight = -pl.direction;
    float distToLight = length(dirToLight);
    dirToLight = normalize(dirToLight);
    vec3 halfway = normalize(dirToLight + dirToCam);

    // Metallic / grazing reflection
    vec3 metallicReflectance = mix(vec3(0.04), albedo, metal);
    metallicReflectance = fresnelSchlick(max(dot(halfway, dirToCam), 0.0), metallicReflectance);

    // Approx area exactly == halfway
    float normalDistribution = DistributionGGX(normal, halfway, rough);
    // Approx area self-shadowed
    float geometry = GeometryGgx(normal, dirToCam, dirToLight, rough);

    vec3 numerator = normalDistribution * geometry * metallicReflectance;
    float denominator = (4.0 * max(dot(normal, dirToCam), 0.0) * max(dot(normal, dirToLight), 0.0)) + 0.0001;
    vec3 specular = numerator / denominator;

    vec3 ratioOfAlbedo = (vec3(1.0) - metallicReflectance) * (1.0 - metal);

    float facingLight = max(dot(normal, dirToLight), 0.0);

    return (((ratioOfAlbedo * albedo) / PI) + specular) * pl.color * facingLight;
}

vec3 PointLight(int index, vec3 albedo, vec3 normal, float rough, float metal)
{
    LightPoint pl = scene.lights.points[index];

    vec3 dirToCam = normalize(scene.campos - inPosition);
    vec3 dirToLight = pl.position - inPosition;
    float distToLight = length(dirToLight);
    dirToLight = normalize(dirToLight);
    vec3 halfway = normalize(dirToLight + dirToCam);

    float attenuation = 1.0 / (distToLight * distToLight);

    // Metallic / grazing reflection
    vec3 metallicReflectance = mix(vec3(0.04), albedo, metal);
    metallicReflectance = fresnelSchlick(max(dot(halfway, dirToCam), 0.0), metallicReflectance);

    // Approx area exactly == halfway
    float normalDistribution = DistributionGGX(normal, halfway, rough);
    // Approx area self-shadowed
    float geometry = GeometryGgx(normal, dirToCam, dirToLight, rough);

    vec3 numerator = normalDistribution * geometry * metallicReflectance;
    float denominator = (4.0 * max(dot(normal, dirToCam), 0.0) * max(dot(normal, dirToLight), 0.0)) + 0.0001;
    vec3 specular = numerator / denominator;

    vec3 ratioOfAlbedo = (vec3(1.0) - metallicReflectance) * (1.0 - metal);

    float facingLight = max(dot(normal, dirToLight), 0.0);

    return (((ratioOfAlbedo * albedo) / PI) + specular) * pl.color * attenuation * facingLight;
}

vec3 SpotLight(int index, vec3 albedo, vec3 normal, float rough, float metal)
{
    LightSpot pl = scene.lights.spots[index];

    vec3 dirToCam = normalize(scene.campos - inPosition);
    vec3 dirToLight = pl.position - inPosition;
    float distToLight = length(dirToLight);
    dirToLight = normalize(dirToLight);
    vec3 halfway = normalize(dirToLight + dirToCam);

    float theta = dot(dirToLight, normalize(pl.direction));
    float radiusFalloff = (theta - pl.cosOuter) / (pl.cosInner - pl.cosOuter);
    radiusFalloff = min(max(radiusFalloff, 0.0), 1.0);
    float attenuation = radiusFalloff / (distToLight * distToLight);

    // Metallic / grazing reflection
    vec3 metallicReflectance = mix(vec3(0.04), albedo, metal);
    metallicReflectance = fresnelSchlick(max(dot(halfway, dirToCam), 0.0), metallicReflectance);

    // Approx area exactly == halfway
    float normalDistribution = DistributionGGX(normal, halfway, rough);
    // Approx area self-shadowed
    float geometry = GeometryGgx(normal, dirToCam, dirToLight, rough);

    vec3 numerator = normalDistribution * geometry * metallicReflectance;
    float denominator = (4.0 * max(dot(normal, dirToCam), 0.0) * max(dot(normal, dirToLight), 0.0)) + 0.0001;
    vec3 specular = numerator / denominator;

    vec3 ratioOfAlbedo = (vec3(1.0) - metallicReflectance) * (1.0 - metal);

    float facingLight = max(dot(normal, dirToLight), 0.0);

    return (((ratioOfAlbedo * albedo) / PI) + specular) * pl.color * attenuation * facingLight;
}

// Main
// ============================================================

vec3 CalculateLights(vec3 albedo, vec3 normal, float rough, float metal)
{
    vec3 totalLight = vec3(0.0);

    totalLight += DirLight(albedo, normal, rough, metal);

    for (int i = 0; i < scene.lights.pointCount; i++)
    {
        totalLight += PointLight(i, albedo, normal, rough, metal);
    }

    for (int i = 0; i < scene.lights.spotCount; i++)
    {
        totalLight += SpotLight(i, albedo, normal, rough, metal);
    }

    return totalLight;
}

void main()
{
    vec3 albedo = texture(texAlbedo, inUv).xyz;
    vec3 mats = texture(texMaps, inUv).xyz;
    float rough = 1-mats.y;
    float metal = mats.z;

    // vec3 albedo = vec3(1.0);
    // float rough = scene.lights.points[0].linear;
    // float metal = scene.lights.points[0].quadratic;

    // vec3 normal = texture(texNormal, inUv).xyz * 2.0 - 1.0;
    // normal = normalize(inTBN * normal);
    vec3 normal = normalize(inNormal);

    vec3 dirToCam = normalize(scene.campos - inPosition);
    vec3 light = CalculateLights(albedo, normal, rough, metal);

    vec3 finalColor = light * albedo;
    outColor = vec4(finalColor, 1.0);
}