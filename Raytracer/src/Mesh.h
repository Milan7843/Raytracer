#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "Shader.h"

#include <vector>
#include <iostream>

struct Vertex
{
	glm::vec3 position;
	glm::vec3 normal;
};

class Mesh
{
public:
	Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices, unsigned int startIndex);
	~Mesh();

	void writeToShader(Shader* shader);

	std::vector<Vertex> vertices;
	std::vector<unsigned int> indices;

	// First index for writing this mesh's data to a shader
	unsigned int shaderArraybeginIndex;

	// Draws this mesh using the active shader
	void draw();

	unsigned int VAO, VBO, EBO;
private:

	void setupMesh();

};