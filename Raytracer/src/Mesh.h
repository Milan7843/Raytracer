#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/euler_angles.hpp>
#include <glm/gtx/string_cast.hpp>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "AbstractShader.h"

#include "CoordinateUtility.h"

#include <vector>
#include <iostream>

struct Vertex
{
	glm::vec4 position;
	glm::vec3 normal;
};

struct Triangle
{
	glm::vec4 v1;
	glm::vec4 v2;
	glm::vec4 v3;
	glm::vec3 normal;
	float reflectiveness;
	glm::vec3 color;
	int mesh;
};

class Mesh
{
public:
	Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices, unsigned int startIndex, unsigned int meshIndex);
	~Mesh();

	void writeToShader(AbstractShader* shader, unsigned int ssbo, unsigned int materialIndex, const glm::mat4& transformation);
	void writePositionToShader(AbstractShader* shader);

	static int getTriangleSize();

	std::vector<Vertex> vertices;
	std::vector<unsigned int> indices;
	std::vector<Triangle> triangles;

	// First index for writing this mesh's data to a shader
	unsigned int shaderArraybeginIndex;
	unsigned int shaderMeshIndex;

	// Draws this mesh using the active shader
	void draw(AbstractShader* shader);

	glm::vec3 position = glm::vec3(0.0f);

	unsigned int VAO, VBO, EBO;

private:
	void setupMesh();
};