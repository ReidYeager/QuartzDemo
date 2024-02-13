#version 450

struct LightDirectional
{
    vec3 color;
    float intensity;
    vec3 direction;
};

struct LightPoint
{
    vec3 color;
    float intensity;
    vec3 position;
};

struct LightSpot
{
    vec3 color;
    float intensity;
    vec3 position;
    vec3 direction;

    float cosInner;
    float cosOuter;
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

layout (set = 1, binding = 0) uniform sampler2D texAlbedo;
layout (set = 1, binding = 1) uniform sampler2D texNormal;
layout (set = 1, binding = 2) uniform sampler2D texMaps;
layout (set = 1, binding = 3) uniform sampler2D texHdriDiffuse;
layout (set = 1, binding = 4) uniform sampler2D texHdriSpecular;
layout (set = 1, binding = 5) uniform sampler2D texHdriBrdf;
layout (set = 1, binding = 6) uniform MaterialBuffer
{
    float gamma;
    float exposure;
} mat;

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec2 inUv;
layout(location = 2) in vec3 inNormal;
layout(location = 3) in vec3 inTangent;
layout(location = 4) in mat3 inTBN;

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

float GeometrySchlickGGX(float cosNormalToX, float roughness)
{
    float r = (roughness + 1.0);
    float k = (r*r) / 8.0;

    float num   = cosNormalToX;
    float denom = (cosNormalToX * (1.0 - k)) + k;
	
    return num / denom;
}

float GeometryGgx(vec3 normal, vec3 dirToCam, vec3 dirToLight, float rough)
{
    float cosNormalToCam = max(dot(normal, dirToCam), 0.0);
    float cosNormalToLight = max(dot(normal, dirToLight), 0.0);
    float ggx2 = GeometrySchlickGGX(cosNormalToCam, rough);
    float ggx1 = GeometrySchlickGGX(cosNormalToLight, rough);
    return ggx1 * ggx2;
}

vec3 fresnelSchlick(float cosTheta, vec3 f0)
{
    // f0 = surface reflection at zero incidence (directly at surface)
    return f0 + (1.0 - f0) * pow(clamp(1.0 - cosTheta, 0.0, 1.0), 5.0);
}

vec3 fresnelSchlickRoughness(float cosTheta, vec3 f0, float roughness)
{
    // f0 = surface reflection at zero incidence (directly at surface)
    return f0 + (max(vec3(1.0 - roughness), f0) - f0) * pow(clamp(1.0 - cosTheta, 0.0, 1.0), 5.0);
}

vec3 Pbr(vec3 lightColor, vec3 dirToLight, vec3 albedo, vec3 normal, float rough, float metal)
{
    vec3 dirToCam = normalize(scene.campos - inPosition);
    vec3 halfway = normalize(dirToLight + dirToCam);

    vec3 f0 = vec3(0.04);
    f0 = mix(f0, albedo, metal);

    // Approx area exactly == halfway
    float NDF = DistributionGGX(normal, halfway, rough);
    // Approx area self-shadowed
    float G = GeometryGgx(normal, dirToCam, dirToLight, rough);
    // Metallic / grazing reflection
    vec3 F = fresnelSchlick(max(dot(halfway, dirToCam), 0.0), f0);    

    vec3 ks = F;
    vec3 kd = vec3(1.0) - ks;
    kd *= 1.0 - metal;

    vec3 numerator = NDF * G * F;
    float denominator = max((4.0 * max(dot(normal, dirToCam), 0.0) * max(dot(normal, dirToLight), 0.0)), 0.0001);
    vec3 specular = numerator / denominator;

    float ndotl = max(dot(normal, dirToLight), 0.0);
    return (kd * albedo / PI + specular) * lightColor * ndotl;
}

// Lights
// ============================================================

vec3 DirLight(vec3 albedo, vec3 normal, float rough, float metal)
{
    LightDirectional light = scene.lights.directional;

    vec3 dirToLight = normalize(-light.direction);

    return Pbr(light.color, dirToLight, albedo, normal, rough, metal) * light.intensity;
}

vec3 PointLight(int index, vec3 albedo, vec3 normal, float rough, float metal)
{
    LightPoint pl = scene.lights.points[index];

    vec3 dirToLight = pl.position - inPosition;
    float distToLight = length(dirToLight);
    dirToLight = normalize(dirToLight);

    float attenuation = min(1.0 / (distToLight * distToLight), 1.0);

    return Pbr(pl.color, dirToLight, albedo, normal, rough, metal) * attenuation * pl.intensity;
}

vec3 SpotLight(int index, vec3 albedo, vec3 normal, float rough, float metal)
{
    LightSpot pl = scene.lights.spots[index];

    vec3 dirToLight = pl.position - inPosition;
    float distToLight = length(dirToLight);
    dirToLight = normalize(dirToLight);

    float theta = dot(-dirToLight, normalize(pl.direction));
    float radiusFalloff = (theta - pl.cosOuter) / (pl.cosInner - pl.cosOuter);
    radiusFalloff = min(max(radiusFalloff, 0.0), 1.0);
    float attenuation = min(radiusFalloff / (distToLight * distToLight), 1.0);

    return Pbr(pl.color, dirToLight, albedo, normal, rough, metal) * attenuation * pl.intensity;
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
    float ao = 1;
    float rough = mats.y;
    float metal = mats.z;

    vec3 normal = normalize(inNormal);
    normal = texture(texNormal, inUv).xyz * 2.0 - 1.0;
    normal = normalize(inTBN * normal);
    vec3 dirToCam = normalize(scene.campos - inPosition);
    vec3 R = reflect(-dirToCam, normal);

    vec3 light = CalculateLights(albedo, normal, rough, metal) + scene.ambient;

    // hdri diffuse
    vec3 f0 = vec3(0.04); 
    f0 = mix(f0, albedo, metal);
    vec3 F = fresnelSchlickRoughness(max(dot(normal, dirToCam), 0.0), f0, rough);
    vec3 ks = F;
    vec3 kd = 1.0 - ks;
    kd *= 1.0 - metal;

    vec3 irradiance = texture(texHdriDiffuse, SphericalMapToUv(normalize(normal))).xyz;
    vec3 diffuse = irradiance * albedo;

    // hdri specular
    const float maxReflectionLod = 4.0;
    vec3 prefilteredColor = textureLod(texHdriSpecular, SphericalMapToUv(normalize(R)), rough * maxReflectionLod).rgb;
    vec2 envBrdf = texture(texHdriBrdf, vec2(max(dot(normal, dirToCam), 0.0), rough)).rg;
    vec3 specular = prefilteredColor * (F * envBrdf.x + envBrdf.y);

    vec3 ambient = (kd * diffuse + specular) * ao;

    vec3 finalColor = light + ambient;

    vec3 mapped = vec3(1.0) - exp(-finalColor * mat.exposure);
    mapped = pow(mapped, vec3(1.0 / mat.gamma));

    outColor = vec4(mapped, 1.0);
}