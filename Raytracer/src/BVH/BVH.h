#pragma once
#include "../Object.h"
#include "../Shader.h"
#include "../Scene.h"

struct BVHData
{
	glm::vec3 pos{ glm::vec3(0.0f) };
	glm::vec3 size{ glm::vec3(1.0f) };
};

struct BVHNode
{
	BVHNode* leftChild{ nullptr };
	BVHNode* rightChild{ nullptr };
	BVHData data;

	void deleteNode()
	{
		// Deleting the left child
		if (leftChild != nullptr)
		{
			// Making it delete its children
			leftChild->deleteNode();
			// Then deleting it
			delete leftChild;
			// And making sure we don't get an invalid pointer
			leftChild = nullptr;
		}

		// Deleting the right child
		if (rightChild != nullptr)
		{
			// Making it delete its children
			rightChild->deleteNode();
			// Then deleting it
			delete rightChild;
			// And making sure we don't get an invalid pointer
			rightChild = nullptr;
		}
	}
};

enum class Axis
{
	x,
	y,
	z
};

class BVH
{
public:
	BVH(const char* vertexShaderPath, const char* geometryShaderPath, const char* fragmentShaderPath);
	~BVH();

	// Draw this BVH for debug purposes
	void draw(Scene& scene);

	// Generate the BVH nodes from a given model
	void generateFromModel(Model& model);

private:
	Shader bvhRenderShader;
	BVHNode* root = nullptr;

	void deleteCurrentBVH();

	BVHData getBoundingBox(std::vector<Triangle>& triangles);

	void updateMinMax(glm::vec4& min, glm::vec4& max, glm::vec4& val);

	BVHNode* generateBVHRecursively(std::vector<Triangle> triangles, unsigned int depth);

	void flattenBVHTreeData(BVHNode* rootNode, std::vector<BVHData>& data, bool onlyLeaves);

	/*
	static bool compareTrianglesX(const Triangle& a, const Triangle& b)
	{
		float middleA = (a.v1.x + a.v2.x + a.v3.x) / 3.0f;
		float middleB = (b.v1.x + b.v2.x + b.v3.x) / 3.0f;
		return middleA < middleB;
	}

	static bool compareTrianglesY(const Triangle& a, const Triangle& b)
	{
		float middleA = (a.v1.y + a.v2.y + a.v3.y) / 3.0f;
		float middleB = (b.v1.y + b.v2.y + b.v3.y) / 3.0f;
		return middleA < middleB;
	}

	static bool compareTrianglesZ(const Triangle& a, const Triangle& b)
	{
		float middleA = (a.v1.z + a.v2.z + a.v3.z) / 3.0f;
		float middleB = (b.v1.z + b.v2.z + b.v3.z) / 3.0f;
		return middleA < middleB;
	}*/

	// The buffers used to store the vertex data
	unsigned int VAO;
	unsigned int VBO;
};

