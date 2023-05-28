#pragma once

#include "BVHHandler.h"

class BVH
{
public:
	BVH();
	~BVH();

	// Generate the root node to fill an entire scene
	void updateByScene(Scene& scene);

	// Flatten this BVH into the given data array
	void flatten(std::vector<BVHData>& data, bool onlyLeaves);

private:

	BVHNode* bvhRoot = nullptr;

	void generateBuffers();

	// The buffer for storing the BVH data
	unsigned int dataSSBO{ 0 };
	// The buffer for storing the index data
	unsigned int triangleSSBO{ 0 };
};
