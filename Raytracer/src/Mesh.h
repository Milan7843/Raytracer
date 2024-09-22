#pragma once

#include "shaders/AbstractShader.h"
#include "gui/ContextMenuSource.h"
#include "SubdivisionSurface.h"

#include "CoordinateUtility.h"
#include "Logger.h"
#include "Object.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/euler_angles.hpp>
#include <glm/gtx/string_cast.hpp>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <vector>
#include <iostream>

class Scene;

struct Vertex
{
	glm::vec4 position = glm::vec4(0.0f);
	glm::vec4 normal = glm::vec4(0.0f);
	glm::vec4 uv = glm::vec4(0.0f);
	glm::vec4 tangent = glm::vec4(0.0f);
	glm::vec4 bitangent = glm::vec4(0.0f);

	inline Vertex operator+ (const Vertex& b) const
	{
		Vertex result;
		result.position = this->position + b.position;
		result.normal = this->normal + b.normal;
		result.uv = this->uv + b.uv;
		result.tangent = this->tangent + b.tangent;
		result.bitangent = this->bitangent + b.bitangent;
		return result;
	}

	inline Vertex operator* (const float t) const
	{
		Vertex result;
		result.position = this->position * t;
		result.normal = this->normal * t;
		result.uv = this->uv * t;
		result.tangent = this->tangent * t;
		result.bitangent = this->bitangent * t;
		return result;
	}
};

struct Triangle
{
	glm::vec4 v1;
	glm::vec4 v2;
	glm::vec4 v3;
	glm::vec4 n1;
	glm::vec4 n2;
	glm::vec4 n3;
	glm::vec4 uv1;
	glm::vec4 uv2;
	glm::vec4 uv3;
	glm::vec4 t1;
	glm::vec4 t2;
	glm::vec4 t3;
	glm::vec4 b1;
	glm::vec4 b2;
	glm::vec4 b3;
	glm::vec3 normal;
	float reflectiveness;
	glm::vec3 color;
	int mesh;
};

class BVHNode;
class BVH;

class Model;

class Mesh : public Object, public ContextMenuSource
{
public:
	Mesh(std::string& name, std::vector<Vertex> vertices, std::vector<unsigned int> indices, glm::vec3 position,
		unsigned int startIndex, unsigned int meshIndex, unsigned int modelID, Model* model);
	~Mesh();

	void writeToShader(AbstractShader* shader, unsigned int ssbo, const glm::mat4& transformation);
	void writePositionToShader(AbstractShader* shader);

	// Draw an interface for this model using ImGui
	virtual bool drawInterface(Scene& scene);

	static int getTriangleSize();

	std::vector<Vertex> vertices;
	std::vector<unsigned int> indices;
	std::vector<Triangle> triangles;

	// First index for writing this mesh's data to a shader
	unsigned int shaderArraybeginIndex;
	unsigned int shaderMeshIndex;

	// Get the number of triangles in this single mesh
	unsigned int getTriangleCount();

	// Set the index of the mesh this triangle belongs to
	void setShaderMeshIndex(unsigned int shaderMeshIndex);

	// Should be called when a material was deleted
	void onDeleteMaterial(unsigned int index);

	// Set the active material index
	void setMaterialIndex(unsigned int index);

	// Draws this mesh using the active shader
	void draw(AbstractShader* shader, Scene* scene, glm::mat4& modelTransformation);

	unsigned int VAO, VBO, EBO;

	unsigned int* getMaterialIndexPointer();

	// Get the index of the material this mesh uses
	unsigned int getMaterialIndex() const;

	unsigned int getModelID();

	void renderContextMenuItems(Scene& scene) override;

	void setModel(Model* model);
	Model* getModel();

	// Get the average vertex position from the origin of the model
	glm::vec3 getAverageVertexPosition() const;

	glm::vec3 getRotationPoint() const override;

	bool isVertexDataChanged();
	void setVertexDataChanged(bool newValue);

	BVHNode* getRootNode(Model& model);

	// Get an approximation of an appropriate distance the camera should be from the object
	// after clicking the focus button.
	virtual float getAppropriateCameraFocusDistance() override;

private:
	void setupMesh();

	// The index of the material this mesh uses
	unsigned int materialIndex{ 0 };

	glm::vec3 averageVertexPosition;

	glm::vec3 boundingBoxSize{ glm::vec3(0.0f) };

	unsigned int modelID{ 0 };
	Model* model;

	bool vertexDataChanged{ true };

	BVHNode* bvhRootNode{ nullptr };
};