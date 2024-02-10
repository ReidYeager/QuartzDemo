#version 450

layout(location = 0) in vec2 inUv;

layout(location = 0) out vec2 outColor; 

const float pi = 3.14159;

float RadicalInverseVdc(uint bits)
{
    bits = (bits << 16u) | (bits >> 16u);
    bits = ((bits & 0x55555555u) << 1u) | ((bits & 0xAAAAAAAAu) >> 1u);
    bits = ((bits & 0x33333333u) << 2u) | ((bits & 0xCCCCCCCCu) >> 2u);
    bits = ((bits & 0x0F0F0F0Fu) << 4u) | ((bits & 0xF0F0F0F0u) >> 4u);
    bits = ((bits & 0x00FF00FFu) << 8u) | ((bits & 0xFF00FF00u) >> 8u);
    return float(bits) * 2.3283064365386963e-10;
}

vec2 Hammersley(uint i, uint n)
{
    return vec2(float(i) / float(n), RadicalInverseVdc(i));
}

vec3 ImportanceSampleGgx(vec2 xi, vec3 n, float rough)
{
    float a = rough * rough;
    float phi = 2.0 * pi * xi.x;
    float cosTheta = sqrt((1.0 - xi.y) / (1.0 + (a * a - 1.0) * xi.y));
    float sinTheta = sqrt(1.0 - cosTheta * cosTheta);

    vec3 h = vec3(cos(phi) * sinTheta, sin(phi) * sinTheta, cosTheta);

    vec3 up = abs(n.z) < 0.999 ? vec3(0.0, 0.0, 1.0) : vec3(1.0, 0.0, 0.0);
    vec3 tangent = normalize(cross(up, n));
    vec3 bitan = cross(n, tangent);

    vec3 sampleVec = tangent * h.x + bitan * h.y + n * h.z;
    return normalize(sampleVec);
}

float GeometrySchlickGGX(float NdotV, float rough)
{
    float a = rough;
    float k = (a * a) / 2.0;

    float nom   = NdotV;
    float denom = NdotV * (1.0 - k) + k;

    return nom / denom;
}

float GeometrySmith(vec3 N, vec3 V, vec3 L, float rough)
{
    float NdotV = max(dot(N, V), 0.0);
    float NdotL = max(dot(N, L), 0.0);
    float ggx2 = GeometrySchlickGGX(NdotV, rough);
    float ggx1 = GeometrySchlickGGX(NdotL, rough);

    return ggx1 * ggx2;
} 

vec2 IntegrateBrdf(float ndotv, float rough)
{
    float a = 0.0;
    float b = 0.0;

    vec3 n = vec3(0.0, 0.0, 1.0);
    vec3 v = vec3(sqrt(1.0 - ndotv * ndotv), 0.0, ndotv);

    const uint sampleCount = 1024;
    for (uint i = 0; i < sampleCount; i++)
    {
        vec2 xi = Hammersley(i, sampleCount);
        vec3 h = ImportanceSampleGgx(xi, n, rough);
        vec3 l = normalize(2.0 * dot(v, h) * h - v);

        float ndotl = max(l.z, 0.0);
        float ndoth = max(h.z, 0.0);
        float vdoth = max(dot(v, h), 0.0);

        if (ndotl > 0.0)
        {
            float g = GeometrySmith(n, v, l, rough);
            float g_vis = (g * vdoth) / (ndoth * ndotv);
            float fc = pow(1.0 - vdoth, 5.0);

            a += (1.0 - fc) * g_vis;
            b += fc * g_vis;
        }
    }

    a /= sampleCount;
    b /= sampleCount;

    return vec2(a, b);
}

void main()
{
    outColor = IntegrateBrdf(inUv.x, inUv.y);
}