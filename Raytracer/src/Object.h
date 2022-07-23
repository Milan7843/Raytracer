#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "AbstractShader.h"

class Object
{
public:

	// Applying transformations on this object
	void rotate(glm::vec3 rotation);
	void setRotation(glm::vec3 rotation);
	void move(glm::vec3 move);
	void setPosition(glm::vec3 pos);
	void scale(glm::vec3 scale);
	virtual void scale(float scale);

	// Reset the rotation of this object
	void resetRotation();

	// Get a single matrix which includes all transformations
	glm::mat4 getTransformationMatrix();

	// Draw this object given the shader
	virtual void draw(AbstractShader* shader);

	// Write this object's data to the given shader
	virtual void writeToShader(AbstractShader* shader, unsigned int ssbo);

	// Whether this object's data has been modified
	bool updated = true;

	// The index of the material this object uses
	unsigned int materialIndex = 0;

	glm::vec3* getPositionPointer();
	glm::vec3* getRotationPointer();
	glm::vec3* getScalePointer();

protected:
	// Abstract class, no need to instantiate this class
	Object();
	virtual ~Object() {}

	// Generate and get the rotation matrix
	glm::mat4 getRotationMatrix();

	// Transformations
	glm::vec3 position = glm::vec3(0.0f);
	glm::vec3 rotation = glm::vec3(0.0f);
	glm::vec3 scaleVector = glm::vec3(1.0f);
};