#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Shader.h"

class Object
{
public:
	void writeToShader(Shader* shader);


protected:
	// Abstract class, no need to instantiate this class
	Object();
	~Object();

	glm::vec3 position;

};