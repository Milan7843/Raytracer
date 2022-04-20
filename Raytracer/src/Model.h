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

private:
	std::string directory;
	void loadModel(std::string path, unsigned int* meshCount, unsigned int* triangleCount);
	void processNode(aiNode* node, const aiScene* scene, unsigned int* meshCount, unsigned int* triangleCount);
	Mesh processMesh(aiMesh* mesh, const aiScene* scene, unsigned int meshCount, unsigned int* triangleCount);
	glm::vec3 aiVector3DToGLMVec3(aiVector3D v);

	unsigned int materialIndex = 0;
};