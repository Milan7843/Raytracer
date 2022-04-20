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

	// Draw this object given the shader
	virtual void draw(Shader* shader);

	// Write this object's data to the given shader
	virtual void writeToShader(Shader* shader, unsigned int ssbo);

	// Apply all the previous transformations to the vertex data
	virtual void applyTransformations();

	std::vector<Mesh> meshes;

private:
	std::string directory;
	void loadModel(std::string path, unsigned int* meshCount, unsigned int* triangleCount);
	void processNode(aiNode* node, const aiScene* scene, unsigned int* meshCount, unsigned int* triangleCount);
	Mesh processMesh(aiMesh* mesh, const aiScene* scene, unsigned int meshCount, unsigned int* triangleCount);
	glm::vec4 aiVector3DToGLMVec4(aiVector3D v);
};