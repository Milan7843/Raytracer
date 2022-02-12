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
	static unsigned int triangleCount;
	Model(const std::string& path);
	~Model();
	void draw(Shader* shader);

	void writeToShader(Shader* shader);

	std::vector<Mesh> meshes;
private:
	std::string directory;
	void loadModel(std::string path);
	void processNode(aiNode* node, const aiScene* scene);
	Mesh processMesh(aiMesh* mesh, const aiScene* scene);
	glm::vec3 aiVector3DToGLMVec3(aiVector3D v);
};