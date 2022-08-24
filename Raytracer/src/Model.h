#pragma once

#include "Object.h"
#include "Material.h"
#include "Mesh.h"

#include <iostream>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include <glm/gtc/type_ptr.hpp>

class Scene;

class Model : public Object
{
public:
	Model(std::string& name, std::vector<unsigned int>& meshMaterialIndices,
		const std::string& path, unsigned int* meshCount, unsigned int* triangleCount,
		unsigned int MAX_MESH_COUNT);
	Model(unsigned int meshMaterialIndex,
		const std::string& path, unsigned int* meshCount, unsigned int* triangleCount,
		unsigned int MAX_MESH_COUNT);
	~Model();

	// Write this model to the given filestream
	virtual void writeDataToStream(std::ofstream& filestream);

	// Write this material to the stream (human readable format)
	friend std::ostream& operator<< (std::ostream& stream, const Model& model);

	// Draw this object given the shader
	virtual void draw(AbstractShader* shader, Scene* scene);

	// Draw an interface for this model using ImGui
	virtual void drawInterface(Scene& scene);

	// Write this object's data to the given shader
	virtual void writeToShader(AbstractShader* shader, unsigned int ssbo);


	void resetShaderIndices(unsigned int* triangleCount, unsigned int* meshCount);

	std::vector<Mesh>& getMeshes();

	std::vector<Mesh> meshes;

private:
	std::string directory;
	std::string path;

	void loadModel(std::string path, std::vector<unsigned int>& meshMaterialIndices, unsigned int* meshCount, unsigned int* triangleCount, unsigned int MAX_MESH_COUNT);
	void loadModel(std::string path, unsigned int meshMaterialIndex, unsigned int* meshCount, unsigned int* triangleCount, unsigned int MAX_MESH_COUNT);
	void processNode(aiNode* node, const aiScene* scene, unsigned int meshMaterialIndex, unsigned int* meshCount, unsigned int* triangleCount, unsigned int MAX_MESH_COUNT);
	void processNode(aiNode* node, const aiScene* scene, std::vector<unsigned int>& meshMaterialIndices, unsigned int* meshCount, unsigned int* triangleCount, unsigned int MAX_MESH_COUNT);
	Mesh processMesh(aiMesh* mesh, const aiScene* scene, unsigned int meshCount, unsigned int* triangleCount);
	Mesh processMesh(aiMesh* mesh, const aiScene* scene, unsigned int materialIndex, unsigned int meshCount, unsigned int* triangleCount);
	glm::vec4 aiVector3DToGLMVec4(aiVector3D v);
};