#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "../Shader.h"

#include <vector>
#include <iostream>

class Light
{
public:

	void writeToShader(Shader* shader, unsigned int ssbo);
	void writePositionToShader(Shader* shader);

	glm::vec3 position = glm::vec3(0.0f);

	unsigned int VAO, VBO, EBO;

private:
	Light(glm::vec3 position);
	~Light();
};