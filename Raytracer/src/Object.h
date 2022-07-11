#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "AbstractShader.h"

class Object
{
public:

	// Applying transformations on this object
	void rotate(glm::vec3 rotationAxis, float degrees);
	void move(glm::vec3 move);
	void scale(glm::vec3 scale);
	virtual void scale(float scale);

	// Draw this object given the shader
	virtual void draw(AbstractShader* shader);

	// Write this object's data to the given shader
	virtual void writeToShader(AbstractShader* shader, unsigned int ssbo);

	// Apply all the previous transformations to the vertex data
	virtual void applyTransformations();

	// Whether this object's data has been modified
	bool updated = true;

	// All transformations on this object
	glm::mat4 transformation = glm::mat4(1.0f);

	// The index of the material this object uses
	unsigned int materialIndex = 0;

protected:
	// Abstract class, no need to instantiate this class
	Object();
	virtual ~Object() {}
};