#pragma once

#include <glad/glad.h>

class ComputeShaderInterface
{
public:
	void run(unsigned int numGroupsX, unsigned int numGroupsY, unsigned int numGroupsZ);
};

