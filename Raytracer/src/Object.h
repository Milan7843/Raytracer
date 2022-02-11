#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Shader.h"

class Object
{
public:
	void writeToShader(Shader* shader);

	glm::vec3 getPosition();
	void setPosition(glm::vec3 pos);

	float vertices[1];

	int indices[1];


protected:
	// Abstract class, no need to instantiate this class
	Object();
	~Object();

	glm::vec3 position;

};