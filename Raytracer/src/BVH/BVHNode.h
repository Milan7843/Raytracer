#pragma once

#include <glm/glm.hpp>
#include <vector>

struct BVHData
{
	glm::vec3 pos{ glm::vec3(0.0f) };
	glm::vec3 size{ glm::vec3(1.0f) };
};

class BVHNode
{
public:

	void deleteNode();

	BVHNode* leftChild{ nullptr };
	BVHNode* rightChild{ nullptr };
	BVHData data;
	std::vector<unsigned int> triangleIndices;

	bool isModelRoot{ false };
};
