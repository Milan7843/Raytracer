#pragma once

#include "Mesh.h"
#include <iostream>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "Object.h"

class Model : public Object
{
public:
	Model(const std::string& path, unsigned int* meshCount, unsigned int* triangleCount, unsigned int materialIndex);
	~Model();

	void draw(Shader* shader);

	void writeToShader(Shader* shader, unsigned int ssbo);

	bool updated = true;

	std::vector<Mesh> meshes;

	void rotate(glm::vec3 rotationAxis, float degrees);
	void move(glm::vec3 move);
	void scale(glm::vec3 scale);
	void scale(float scale);
	void applyTransformations();

private:
	std::string directory;
	void loadModel(std::string path, unsigned int* meshCount, unsigned int* triangleCount);
	void processNode(aiNode* node, const aiScene* scene, unsigned int* meshCount, unsigned int* triangleCount);
	Mesh processMesh(aiMesh* mesh, const aiScene* scene, unsigned int meshCount, unsigned int* triangleCount);
	glm::vec4 aiVector3DToGLMVec4(aiVector3D v);

	glm::mat4 transformation = glm::mat4(1.0f);

	unsigned int materialIndex = 0;
};