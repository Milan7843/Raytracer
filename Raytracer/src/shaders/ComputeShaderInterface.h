#pragma once

#include <glad/glad.h>

class ComputeShaderInterface
{
public:
	virtual void run(unsigned int numGroupsX, unsigned int numGroupsY, unsigned int numGroupsZ) const;
};

