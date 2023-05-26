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
};

class BVH
{
public:
	BVH(const char* vertexShaderPath, const char* geometryShaderPath, const char* fragmentShaderPath);
	~BVH();

	// Draw this BVH for debug purposes
	void draw(Scene& scene);

private:
	Shader bvhRenderShader;
	BVHNode* root = nullptr;

	// The buffers used to store the vertex data
	unsigned int VAO;
	unsigned int VBO;
};

