#pragma once

#include "Mesh.h"
#include <iostream>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include <glm/gtc/type_ptr.hpp>

#include "Object.h"
#include "Material.h"

class Model : public Object
{
public:
	Model(std::string& name, const std::string& path, unsigned int* meshCount, unsigned int* triangleCount, unsigned int materialIndex,
		unsigned int MAX_MESH_COUNT);
	Model(const std::string& path, unsigned int* meshCount, unsigned int* triangleCount, unsigned int materialIndex,
		unsigned int MAX_MESH_COUNT);
	~Model();

	// Write this model to the given filestream
	virtual void writeDataToStream(std::ofstream& filestream);

	// Write this material to the stream (human readable format)
	friend std::ostream& operator<< (std::ostream& stream, const Model& model);

	// Draw this object given the shader
	virtual void draw(AbstractShader* shader, Material* material);

	// Write this object's data to the given shader
	virtual void writeToShader(AbstractShader* shader, unsigned int ssbo);

	std::vector<Mesh> meshes;

private:
	std::string directory;
	std::string path;

	void loadModel(std::string path, unsigned int* meshCount, unsigned int* triangleCount, unsigned int MAX_MESH_COUNT);
	void processNode(aiNode* node, const aiScene* scene, unsigned int* meshCount, unsigned int* triangleCount, unsigned int MAX_MESH_COUNT);
	Mesh processMesh(aiMesh* mesh, const aiScene* scene, unsigned int meshCount, unsigned int* triangleCount);
	glm::vec4 aiVector3DToGLMVec4(aiVector3D v);
};