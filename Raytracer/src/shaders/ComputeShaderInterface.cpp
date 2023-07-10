#include "ComputeShaderInterface.h"

void ComputeShaderInterface::run(unsigned int numGroupsX, unsigned int numGroupsY, unsigned int numGroupsZ)
{
	glDispatchCompute(numGroupsX, numGroupsY, numGroupsZ);
	glMemoryBarrier(GL_ALL_BARRIER_BITS);
}