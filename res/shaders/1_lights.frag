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

layout (set = 1, binding = 0) uniform MaterialStruct
{
    vec3 baseReflectivity;
    float roughness;
    float metalness;
} mat;
// layout (set = 1, binding = 1) uniform sampler2D tex;

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec2 inUv;
layout(location = 2) in vec3 inNormal;

layout(location = 0) out vec4 outColor;

float CalculateSpecularity(vec3 dirToLight, vec3 normal)
{
    vec3 dirToView = normalize(scene.campos - inPosition);
    vec3 reflectDir = normalize(reflect(-dirToLight, normal));
    return pow(max(dot(dirToView, reflectDir), 0.0), 32);
}

float CalculateAttenuation(vec3 lightPos, float linear, float quadratic)
{
    float dist = length(lightPos - inPosition);
    float attenuation = 1.0 + (linear * dist) + (quadratic * dist * dist);
    return 1.0 / attenuation;
}

vec3 CalculateDirectionalLight()
{
    float diffuse = max(-dot(inNormal, normalize(scene.lightDir.direction)), 0.0);
    float spec = CalculateSpecularity(-scene.lightDir.direction, inNormal);
    return scene.lightDir.color * (diffuse + spec);
}

vec3 CalculatePointLight(int index)
{
    vec3 dirToLight = normalize(scene.lightPoint[index].position - inPosition);
    
    float diffuse = max(dot(inNormal, dirToLight), 0.0);
    float spec = CalculateSpecularity(dirToLight, inNormal);
    
    float attenuation = CalculateAttenuation(scene.lightPoint[index].position, scene.lightPoint[index].linear, scene.lightPoint[index].quadratic);
    
    return scene.lightPoint[index].color * (diffuse + spec) * attenuation;
}

vec3 CalculateSpotLight(int index)
{
    vec3 dirToLight = normalize(scene.lightSpot[index].position - inPosition);

    float spec = CalculateSpecularity(dirToLight, inNormal);
    float diffuse = max(dot(inNormal, dirToLight), 0.0);

    float theta = dot(dirToLight, normalize(scene.lightSpot[index].direction));
    float radiusFalloff = (theta - scene.lightSpot[index].cosOuter) / (scene.lightSpot[index].cosInner - scene.lightSpot[index].cosOuter);
    radiusFalloff = max(radiusFalloff, 0.0);
    float attenuation = CalculateAttenuation(scene.lightSpot[index].position, scene.lightSpot[index].linear, scene.lightSpot[index].quadratic);

    return scene.lightSpot[index].color * (diffuse + spec) * radiusFalloff * attenuation;
}

void main()
{
    //vec3 albedo = texture(inTex, inUv);

    vec3 light = scene.ambient;
    light += CalculateDirectionalLight();
    light += CalculatePointLight(0);
    light += CalculateSpotLight(0);

    outColor = vec4(light * vec3(mat.roughness / 5.0, mat.metalness / 5.0, 0.0), 1.0);
}