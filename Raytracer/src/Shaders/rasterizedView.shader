#version 460 core

out vec4 FragColor;

in vec3 FragPos;
in vec3 Color;
in vec3 Normal;

#define NUM_POINT_LIGHTS $numPointLights
#define NUM_DIR_LIGHTS 1
#define NUM_AMBIENT_LIGHTS 1


// LIGHTS
struct PointLight
{
    vec3 pos;
    vec3 color;
    float intensity;
};

uniform PointLight pointLights[NUM_POINT_LIGHTS];

struct DirLight
{
    vec3 dir;
    vec3 color;
    float intensity;
};
DirLight dirLights[NUM_DIR_LIGHTS] = DirLight[](
    //         Pos                  Color                   Intensity
    DirLight(vec3(.707, -.707, 0.), vec3(1.0, 1.0, 0.9), 0.5)
    );

struct AmbientLight
{
    vec3 color;
    float intensity;
};
AmbientLight ambientLights[1] = AmbientLight[](
    //           Color                  Intensity
    AmbientLight(vec3(0.8, 0.8, 1.0), 0.02)
    );



vec3 calculateLights(vec3 pos, vec3 normal);

void main()
{
    //FragColor = vec4(Color, 1.);
    FragColor = vec4(calculateLights(FragPos, Normal), 1.);
    //FragColor = vec4(Normal, 1.);
    //FragColor = vec4(FragPos / 4., 1.);
}

vec3 calculateLights(vec3 pos, vec3 normal)
{
    vec3 finalLight = vec3(0.);

    /* POINT LIGHTS */
    for (int i = 0; i < NUM_POINT_LIGHTS; i++)
    {
        vec3 dist = pointLights[i].pos - pos;
        vec3 dir = normalize(dist);

        float intensity = max(
            dot(dir, normal),
            0.);

        //finalLight += dir / 3.;

        float falloff = 1.0 / (dist.x * dist.x + dist.y * dist.y + dist.z * dist.z);

        finalLight += intensity * pointLights[i].color * pointLights[i].intensity * falloff;
    }

    /* DIRECTIONAL LIGHTS */
    for (int i = 0; i < NUM_DIR_LIGHTS; i++)
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
    for (int i = 0; i < NUM_AMBIENT_LIGHTS; i++)
    {
        finalLight += ambientLights[i].intensity * ambientLights[i].color;
    }

    return finalLight;
}