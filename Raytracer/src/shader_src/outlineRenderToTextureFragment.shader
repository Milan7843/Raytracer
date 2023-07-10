#version 460 core

// Writing to the texture
layout(location = 0) out vec4 colors;

in vec3 FragPos;
in vec3 Normal;

void main()
{
    colors = vec4(1.0);
}
