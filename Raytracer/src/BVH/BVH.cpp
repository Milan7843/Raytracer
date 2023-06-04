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

void BVH::flatten(std::vector<BVHData>& data, bool onlyLeaves)
{
	BVHHandler::flattenBVHTreeData(this->bvhRoot, data, onlyLeaves);
}

BVHNode* BVH::getRoot()
{
	return bvhRoot;
}

void BVH::setRoot(BVHNode* newRoot)
{
	bvhRoot = newRoot;
}

void BVH::generateBuffers()
{
	glDeleteBuffers(1, &dataSSBO);
	glDeleteBuffers(1, &triangleSSBO);


	// Generating a buffer for data
	glGenBuffers(1, &dataSSBO);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, dataSSBO);

	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 3, dataSSBO);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);


	// Generating a buffer for the triangle indices
	glGenBuffers(1, &triangleSSBO);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, triangleSSBO);

	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 4, triangleSSBO);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
}
