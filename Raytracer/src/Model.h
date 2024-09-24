#pragma once

#include "Object.h"
#include "Material.h"
#include "Mesh.h"
#include "ShaderWritable.h"
#include "gui/ContextMenuSource.h"

#include <iostream>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include <glm/gtc/type_ptr.hpp>

class Scene;

class Model : public Object, public ContextMenuSource, public ShaderWritable
{
public:
	Model(std::string& name,
		const std::string& path, unsigned int* meshCount, unsigned int* triangleCount,
		unsigned int MAX_MESH_COUNT);
	Model(const std::string& path, unsigned int* meshCount, unsigned int* triangleCount,
		unsigned int MAX_MESH_COUNT);
	~Model();

	// Write this material to the stream (human readable format)
	friend std::ostream& operator<< (std::ostream& stream, const Model& model);

	// Prepare for drawing with the given shader by setting the model and rotation matrices
	void prepareForDraw(AbstractShader* shader);

	// Draw this object given the shader
	virtual void draw(AbstractShader* shader, Scene* scene);

	// Draw an interface for this model using ImGui
	virtual bool drawInterface(Scene& scene);

	// Write this object's data to the given shader
	virtual bool writeToShader(AbstractShader* shader, unsigned int ssbo);

	// Should be called when a material was deleted
	void onDeleteMaterial(unsigned int index);

	void renderContextMenuItems(Scene& scene) override;

	// Update the references from the child meshes to this model
	// Needs to be done in case the address of this model could have changed,
	// such as the vector containing it resizing.
	void updateChildPointers();

	void resetShaderIndices(unsigned int* triangleCount, unsigned int* meshCount);

	std::vector<Mesh>& getMeshes();

	std::vector<Mesh> meshes;

	bool isVertexDataChanged();
	void setVertexDataChanged(bool newValue, bool alsoSetMeshes=true);

	BVHNode* getRootNode();

	const std::string& getPath() const;

	unsigned int getTriangleCount();

	void setMaterialIndex(unsigned int materialIndex);

	// Get an approximation of an appropriate distance the camera should be from the object
	// after clicking the focus button.
	virtual float getAppropriateCameraFocusDistance() override;

private:
	std::string directory;
	std::string path;

	unsigned int subdivisionLevel{ 0 };
	void updateSubdivision(Scene& scene);

	bool vertexDataChanged{ true };

	BVHNode* bvhRootNode{ nullptr };

	unsigned int triangleCount{ 0 };

	void loadModel(std::string path, unsigned int* meshCount, unsigned int* triangleCount, unsigned int MAX_MESH_COUNT);
	void processNode(aiNode* node, const aiScene* scene, unsigned int* meshCount, unsigned int* meshIndex, unsigned int* triangleCount, unsigned int MAX_MESH_COUNT);
	Mesh processMesh(aiMesh* mesh, const aiScene* scene, unsigned int meshCount, unsigned int* triangleCount);
	glm::vec4 aiVector3DToGLMVec4(aiVector3D v);

	void calculateTangentBitangent(std::vector<Vertex>& vertices,
		unsigned int index1, unsigned int index2, unsigned int index3, 
		std::vector<glm::vec4>& tangents,
		std::vector<glm::vec4>& bitangents,
		std::vector<int>& bi_ti_samplesPerVertex);
};