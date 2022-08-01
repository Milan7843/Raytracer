#version 460 core

out vec4 FragColor;

in vec3 FragPos;

// Skybox
uniform sampler2D hdri;

#define PI 3.14159265359
float atan2(float x, float z)
{
    bool s = (abs(x) > abs(z));
    return mix(PI / 2.0 - atan(x, z), atan(z, x), s);
}

void main()
{
    vec3 dir = normalize(FragPos);

    // Calculating HDRI position
    float yaw = atan2(dir.x, dir.z);
    float pitch = (dir.y / 2 + 0.5);

    vec3 skyColor = texture(hdri, vec2(yaw / (2 * PI), -pitch)).rgb;

    FragColor = vec4(skyColor, 1.);
}