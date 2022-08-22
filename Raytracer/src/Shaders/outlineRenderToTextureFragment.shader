#version 460 core

// Writing to the texture
layout(location = 0) out vec3 colors;

in vec3 FragPos;
in vec3 Normal;

void main()
{
    colors = vec3(1.0);
}
