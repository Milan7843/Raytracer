#version 460 core
layout(local_size_x = 1, local_size_y = 1, local_size_z = 1) in;

layout(std430, binding = 3) buffer Pixels
{
	vec4 colors[];
};

uniform int size;

// Returns a random value between 0 and 1 [0, 1)
float rand(float seed)
{
    return fract(sin(seed) * 10000.);
}

void main()
{
	// 2 dimensional indices
	int cx = int(gl_GlobalInvocationID.x);
	int cy = int(gl_GlobalInvocationID.y);

	// Calculating the total index, used to map the 2D indices to a 1D array
	int i = int(cx + size * cy);

	colors[i] = vec4(rand(i), 0.0, 0.0, 1.0);
}