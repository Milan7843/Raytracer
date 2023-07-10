#version 460 core
layout(location = 0) in vec3 aPos;

out vec3 FragPos;

uniform mat4 view;
uniform mat4 projection;

void main()
{
	FragPos = aPos, 1.0;
	gl_Position = projection * view * vec4(FragPos, 1.0);
}