#version 450

layout (set = 1, binding = 0) uniform sampler2D texHdri;

layout(location = 0) in vec2 inUv;

layout(location = 0) out vec4 outColor;

const vec2 invAtan = vec2(0.1591, 0.3183);

vec3 UvToSphericalMap(vec2 uv)
{
    uv.y = 1.0 - uv.y;
    uv -= 0.5;
    uv /= invAtan;
        
    float cosY = cos(uv.y);
    return vec3(cos(uv.x) * cosY, sin(uv.y), sin(uv.x) * cosY);
}

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
    vec3 dirToTexel = UvToSphericalMap(inUv);

    vec3 normal = dirToTexel;
    vec3 right = normalize(cross(vec3(0.0, 1.0, 0.0), normal));
    vec3 up = normalize(cross(normal, right));

    const float pi = 3.14159;
    const float angleDelta = 0.0125; // Radians
    int sampleCount = 0;

    vec3 irradiance = vec3(0.0);

    // Latitude angles (full circle)
    for (float phi = 0.0; phi < 2.0 * pi; phi += angleDelta)
    {
        // Longitude angles (90 degrees)
        for (float theta = 0.0; theta < 0.5 * pi; theta += angleDelta)
        {
            // Spherical to cartesian
            vec3 tangentVector = vec3(sin(theta) * cos(phi), sin(theta) * sin(phi), cos(theta));
            // Tangent to world space
            vec3 vector = tangentVector.x * right + tangentVector.y * up + tangentVector.z * normal;
            vector = normalize(vector);

            irradiance += texture(texHdri, SphericalMapToUv(vector)).xyz * cos(theta) * sin(theta);
            sampleCount++;
        }
    }

    irradiance = (pi * irradiance) / float(sampleCount);

    outColor = vec4(irradiance, 1.0);
}