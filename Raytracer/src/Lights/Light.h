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

	void setIndex(unsigned int index);

protected:
	Light(glm::vec3 position, glm::vec3 color, float intensity);
	~Light();
	unsigned int index = 0;
	glm::vec3 position = glm::vec3(0.0f);
	glm::vec3 color = glm::vec3(0.0f);
	float intensity = 1.0f;
};