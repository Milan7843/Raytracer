#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Shader.h"

class Object
{
public:

	// Applying transformations on this object
	void rotate(glm::vec3 rotationAxis, float degrees);
	void move(glm::vec3 move);
	void scale(glm::vec3 scale);
	virtual void scale(float scale);

	// Draw this object given the shader
	virtual void draw(Shader* shader);

	// Write this object's data to the given shader
	virtual void writeToShader(Shader* shader, unsigned int ssbo);

	// Apply all the previous transformations to the vertex data
	virtual void applyTransformations();

	// Whether this object's data has been modified
	bool updated = true;

	// All transformations on this object
	glm::mat4 transformation = glm::mat4(2.0f);

protected:
	// Abstract class, no need to instantiate this class
	Object();
	virtual ~Object() {}

	// The index of the material this object uses
	unsigned int materialIndex = 0;
};