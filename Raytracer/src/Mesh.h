#pragma once

#include "shaders/AbstractShader.h"

#include "CoordinateUtility.h"
#include "Logger.h"
#include "ImGuiEditorInterface.h"

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
	glm::vec4 position;
	glm::vec4 normal;
};

struct Triangle
{
	glm::vec4 v1;
	glm::vec4 v2;
	glm::vec4 v3;
	glm::vec4 n1;
	glm::vec4 n2;
	glm::vec4 n3;
	glm::vec3 normal;
	float reflectiveness;
	glm::vec3 color;
	int mesh;
};

class BVHNode;
class BVH;

class Model;

class Mesh : public ImGuiEditorInterface
{
public:
	Mesh(std::string& name, std::vector<Vertex> vertices, std::vector<unsigned int> indices,
		unsigned int startIndex, unsigned int meshIndex, unsigned int materialIndex, unsigned int modelID);
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

	// Draws this mesh using the active shader
	void draw(AbstractShader* shader, Scene* scene);

	glm::vec3 position{ glm::vec3(0.0f) };

	unsigned int VAO, VBO, EBO;

	unsigned int* getMaterialIndexPointer();

	std::string& getName();

	// Get the index of the material this mesh uses
	unsigned int getMaterialIndex() const;

	unsigned int getModelID();

private:
	void setupMesh();

	// The index of the material this mesh uses
	unsigned int materialIndex;

	std::string name;
	unsigned int modelID{ 0 };
};