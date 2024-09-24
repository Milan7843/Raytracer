#include "BVH.h"

BVH::BVH()
{
	generateBuffers();
}

BVH::~BVH()
{
}

void BVH::updateByScene(Scene& scene)
{
	this->bvhRoot = BVHHandler::updateByScene(scene, bvhRoot);

	BVHHandler::writeIntoSSBOs(bvhRoot, dataSSBO, triangleSSBO);
}

void BVH::flatten(std::vector<BVHData>& data, bool onlyLeaves) const
{
	BVHHandler::flattenBVHTreeData(this->bvhRoot, data, onlyLeaves);
}

BVHNode* BVH::getRoot() const
{
	return bvhRoot;
}

void BVH::setRoot(BVHNode* newRoot)
{
	bvhRoot = newRoot;
}

void BVH::generateBuffers()
{
	// Generate the buffer if it didn't exist yet
	if (dataSSBO == 0)
	{
		glGenBuffers(1, &dataSSBO);
	}

	// Generate the buffer if it didn't exist yet
	if (triangleSSBO == 0)
	{
		glGenBuffers(1, &triangleSSBO);
	}


	// Generating a buffer for data
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, dataSSBO);

	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 3, dataSSBO);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);


	// Generating a buffer for the triangle indices
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, triangleSSBO);

	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 4, triangleSSBO);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
}
