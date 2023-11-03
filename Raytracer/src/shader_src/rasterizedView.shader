#version 460 core

out vec4 FragColor;

in vec3 FragPos;
in vec3 Normal;
in vec2 uv;
in mat3 tbn;

#define NUM_POINT_LIGHTS 10
#define NUM_DIR_LIGHTS 10
#define NUM_AMBIENT_LIGHTS 10
#define NUM_MATERIALS 40

uniform vec3 cameraPos;

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




struct Material
{
    vec3 color;
    float reflectiveness;
    float transparency;
    float refractiveness;
    float reflectionDiffusion;
    bool hasAlbedoTexture;
    float albedoTexture_xMin;
    float albedoTexture_xMax;
    float albedoTexture_yMin;
    float albedoTexture_yMax;
    bool hasNormalTexture;
    float normalTexture_xMin;
    float normalTexture_xMax;
    float normalTexture_yMin;
    float normalTexture_yMax;
};
uniform Material materials[NUM_MATERIALS];

uniform int materialIndex;


// Skybox
uniform sampler2D hdri;
uniform sampler2D textureAtlas;

#define PI 3.14159265359
float atan2(float x, float z)
{
    bool s = (abs(x) > abs(z));
    return mix(PI / 2.0 - atan(x, z), atan(z, x), s);
}

float rand(vec2 co)
{
    return fract(sin(dot(co, vec2(12.9898, 78.233))) * 43758.5453);
}

vec3 sampleAlbedo(vec2 uv)
{
    if (!materials[materialIndex].hasAlbedoTexture)
    {
        return materials[materialIndex].color;
    }

    uv.x = mod(uv.x + 1.0, 1.0);
    uv.y = mod(uv.y + 1.0, 1.0);

    float u = (uv.x * materials[materialIndex].albedoTexture_xMax - materials[materialIndex].albedoTexture_xMin) + materials[materialIndex].albedoTexture_xMin;
    float v = (uv.y * materials[materialIndex].albedoTexture_yMax - materials[materialIndex].albedoTexture_yMin) + materials[materialIndex].albedoTexture_yMin;

    return materials[materialIndex].color * texture(textureAtlas, vec2(u, v)).rgb;
}

vec3 sampleNormal(vec2 uv)
{
    if (!materials[materialIndex].hasNormalTexture)
    {
        return Normal;
    }

    uv.x = mod(uv.x + 1.0, 1.0);
    uv.y = mod(uv.y + 1.0, 1.0);

    float u = (uv.x * (materials[materialIndex].normalTexture_xMax - materials[materialIndex].normalTexture_xMin)) + materials[materialIndex].normalTexture_xMin;
    float v = (uv.y * (materials[materialIndex].normalTexture_yMax - materials[materialIndex].normalTexture_yMin)) + materials[materialIndex].normalTexture_yMin;

    vec3 normalFromMap = texture(textureAtlas, vec2(u, v)).xyz;

    normalFromMap = normalFromMap * 2.0 - 1.0;
    normalFromMap = normalize(tbn * normalFromMap);

    return normalFromMap;
}

vec3 sampleHDRI(vec3 direction)
{
    vec3 dir = normalize(direction);

    // Calculating HDRI base position
    float yaw = atan2(dir.x, dir.z);
    float pitch = (dir.y / 2 + 0.5);

    float randomness = materials[materialIndex].reflectionDiffusion * 0.01;

    vec3 skyColor = vec3(0.0);

    int sampleCount = 50;

    // Then for the randomness we move the pitch and yaw some random amount and then sample the HDRI
    for (int i = 0; i < sampleCount; i++)
    {
        float movedYaw = yaw + (rand(vec2(yaw + i * 3001, pitch - i * 149)) * 2.0 - 1.0) * randomness;
        float movedPitch = pitch + (rand(vec2(pitch - i * 797, yaw + i * 73)) * 2.0 - 1.0) * randomness;

        skyColor += texture(hdri, vec2(movedYaw / (2 * PI), -movedPitch)).rgb;
    }

    return skyColor / sampleCount;
}

// Defines how much the specularity should be taken into account if the reflectiveness is 0.0
// The used value will linearly increase to 1.0 as the reflectiveness increases to 1.0
#define NON_REFLECTIVE_SPECULARITY 0.4;


vec3 calculateLights(vec3 pos, vec3 normal, vec3 viewDir);

vec3 calculateDirLight(DirLight light, vec3 normal, vec3 viewDir);
vec3 calculatePointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir);
vec3 calculateAmbientLight(AmbientLight light);




void main()
{
    vec3 viewDir = normalize(cameraPos - FragPos);
    vec3 albedo = sampleAlbedo(uv);
    vec3 normal = sampleNormal(uv);

    FragColor = vec4(albedo * calculateLights(FragPos, normal, viewDir), 1.);
    //FragColor = vec4(calculateLights(FragPos, normal, viewDir), 1.);

    //FragColor = vec4(albedo, 1.0);
    //FragColor = vec4(normal, 1.0);
}

vec3 calculateLights(vec3 pos, vec3 normal, vec3 viewDir)
{
    vec3 finalLight = vec3(0.);

    /* POINT LIGHTS */
    for (int i = 0; i < pointLightCount; i++)
    {
        finalLight += calculatePointLight(pointLights[i], normal, pos, viewDir);
    }

    /* DIRECTIONAL LIGHTS */
    for (int i = 0; i < dirLightCount; i++)
    {
        finalLight += calculateDirLight(dirLights[i], normal, viewDir);
    }

    /* AMBIENT LIGHTS */
    for (int i = 0; i < ambientLightCount; i++)
    {
        finalLight += calculateAmbientLight(ambientLights[i]);
    }

    /* HDRI */
    vec3 reflectDir = reflect(-viewDir, normal);
    finalLight = finalLight + sampleHDRI(reflectDir) * materials[materialIndex].reflectiveness;

    return finalLight;
}

vec3 calculateDirLight(DirLight light, vec3 normal, vec3 viewDir)
{
    vec3 lightDir = normalize(-light.dir);

    // Single-sided light
    if (dot(lightDir, normal) < 0.0) return vec3(0.0);

    float diff = max(dot(normal, lightDir), 0.0);

    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 1.0 + materials[materialIndex].reflectiveness);

    vec3 diffuse = light.color * diff * (1.0 - materials[materialIndex].reflectiveness);
    float specReflectivenessMultiplier = NON_REFLECTIVE_SPECULARITY + materials[materialIndex].reflectiveness * NON_REFLECTIVE_SPECULARITY;
    vec3 specular = light.color * spec* specReflectivenessMultiplier;

    return (diffuse + specular) * light.intensity;
}

vec3 calculatePointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir)
{
    vec3 lightDir = normalize(light.pos - fragPos);

    // Single-sided light
    if (dot(lightDir, normal) < 0.0) return vec3(0.0);

    float diff = max(dot(normal, lightDir), 0.0);

    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 1.0 + materials[materialIndex].reflectiveness);

    // Falloff by distance
    float distance = length(light.pos - fragPos);
    float falloff = 1.0 / (distance * distance);

    vec3 diffuse = light.color * diff * (1.0 - materials[materialIndex].reflectiveness);
    float specReflectivenessMultiplier = NON_REFLECTIVE_SPECULARITY + materials[materialIndex].reflectiveness * NON_REFLECTIVE_SPECULARITY;
    vec3 specular = light.color * spec * specReflectivenessMultiplier;

    return (diffuse + specular) * falloff * light.intensity;
}

vec3 calculateAmbientLight(AmbientLight light)
{
    return light.intensity * light.color * (1.0 - materials[materialIndex].reflectiveness);
}