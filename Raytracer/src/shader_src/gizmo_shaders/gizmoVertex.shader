#version 460 core
layout(location = 0) in vec3 inPosition; // Vertex position attribute
layout(location = 1) in vec3 color; // Vertex position attribute
out vec3 toGeo_color;

uniform mat4 projection;
uniform mat4 view;

void main()
{
    vec4 viewPosition = view * vec4(inPosition, 1.0);
    gl_Position = projection * viewPosition;
    toGeo_color = color;
}