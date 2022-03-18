#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <vector>
#include <iostream>

#include "../Shader.h"

class Light
{
public:

	void writeToShader(Shader* shader);
	void writePositionToShader(Shader* shader);

	glm::vec3 position = glm::vec3(0.0f);

	unsigned int VAO, VBO, EBO;

private:
	Light(glm::vec3 position);
	~Light();
};