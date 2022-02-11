#version 460 core

out vec4 FragColor;

in vec3 FragPos;
in vec4 Color;

void main()
{
    FragColor = vec4(1.);
    //FragColor = Color;
}