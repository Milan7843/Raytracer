#version 460 core

out vec4 FragColor;

in vec3 FragPos;
in vec3 Color;
in vec3 Normal;

#define NUM_POINT_LIGHTS 10
#define NUM_DIR_LIGHTS 10
#define NUM_AMBIENT_LIGHTS 10


// LIGHTS
struct PointLight
{
    vec3 pos;
    vec3 color;
    float intensity;
};
uniform PointLight pointLights[NUM_POINT_LIGHTS];
uniform int pointLightCount;

struct DirLight
{
    vec3 dir;
    vec3 color;
    float intensity;
};
uniform DirLight dirLights[NUM_DIR_LIGHTS];
uniform int dirLightCount;

struct AmbientLight
{
    vec3 color;
    float intensity;
};
uniform AmbientLight ambientLights[NUM_AMBIENT_LIGHTS];
uniform int ambientLightCount;


vec3 calculateLights(vec3 pos, vec3 normal);

void main()
{
    FragColor = vec4(Color * calculateLights(FragPos, Normal), 1.);
}

vec3 calculateLights(vec3 pos, vec3 normal)
{
    vec3 finalLight = vec3(0.);

    /* POINT LIGHTS */
    for (int i = 0; i < pointLightCount; i++)
    {
        vec3 dist = pointLights[i].pos - pos;
        vec3 dir = normalize(dist);

        float intensity = max(
            dot(dir, normal),
            0.);

        float falloff = 1.0 / (dist.x * dist.x + dist.y * dist.y + dist.z * dist.z);

        finalLight += intensity * pointLights[i].color * pointLights[i].intensity * falloff;
    }

    /* DIRECTIONAL LIGHTS */
    for (int i = 0; i < dirLightCount; i++)
    {
        vec3 dir = -dirLights[i].dir;
        dir = normalize(dir);

        // Works somehow??
        float intensity = max(min(
            dot(dir, normal),
            1.), 0.);

        finalLight += intensity * dirLights[i].color * dirLights[i].intensity;
    }

    /* AMBIENT LIGHTS */
    for (int i = 0; i < ambientLightCount; i++)
    {
        finalLight += ambientLights[i].intensity * ambientLights[i].color;
    }

    return finalLight;
}