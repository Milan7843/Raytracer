#version 460 core
layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aSize;

out vec3 pos;
out vec3 size;

void main()
{
	pos = aPos;
	size = aSize;
}