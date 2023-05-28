#pragma once
#include "../Object.h"
#include "../Shader.h"
#include "../Mesh.h"
#include <queue>

class Scene;

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
	std::vector<unsigned int> triangleIndices;

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

struct FlattenedBVHNode
{
	BVHData data;
	int leftChild;
	int rightChild;
};

enum class Axis
{
	x,
	y,
	z
};

class BVHHandler
{
public:
	BVHHandler(const char* vertexShaderPath, const char* geometryShaderPath, const char* fragmentShaderPath);
	~BVHHandler();

	// Draw this BVH for debug purposes
	void draw(Scene& scene);

	// Generate the root node to fill an entire scene
	static BVHNode* updateByScene(Scene& scene, BVHNode* oldBVHRoot);

	// Generate the BVH nodes from a given mesh
	static BVHNode* generateFromMesh(Mesh& mesh, BVHNode* oldBVHRoot);

	// Write the given BVH into two SSBOs: one for position/size/structure data and one for index data
	static void writeIntoSSBOs(BVHNode* root, unsigned int dataSSBO, unsigned int triangleSSBO);

	// Flatten the given BVH into a vector
	static void flattenBVHTreeData(BVHNode* rootNode, std::vector<BVHData>& data, bool onlyLeaves);

private:
	Shader bvhRenderShader;

	static void deleteBVH(BVHNode* node);

	static BVHData getBoundingBox(std::vector<Triangle>& triangles, std::vector<unsigned int>& indices);

	static BVHNode* generateBVHRecursively(std::vector<Triangle>& triangles, std::vector<unsigned int> indices, unsigned int depth, unsigned int shaderArrayBeginIndex);

	static void flattenBVHTreeIndices(BVHNode* rootNode, std::vector<FlattenedBVHNode>& treeStructureData, std::vector<unsigned int>& indices);

	static BVHNode* generateBVHRecursively(std::vector<BVHNode*> nodes);

	static void updateMinMax(glm::vec4& min, glm::vec4& max, const glm::vec4& val);

	static void updateMinMax(glm::vec3& min, glm::vec3& max, const glm::vec3& val);

	static BVHData getBoundingBox(std::vector<BVHNode*>& nodes);

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

