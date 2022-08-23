#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "AbstractShader.h"

#include "Logger.h"

class Object
{
public:

	// Write this model to the given filestream
	virtual void writeDataToStream(std::ofstream& filestream);

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
	virtual glm::mat4 getTransformationMatrix();

	// Draw this object given the shader
	virtual void draw(AbstractShader* shader);

	// Write this object's data to the given shader
	virtual void writeToShader(AbstractShader* shader, unsigned int ssbo);

	// Get whether this object has been updated
	bool isUpdated();
	// Mark this object as being updated
	void setUpdated();
	// Mark this object as being not updated
	void setNotUpdated();

	// Get whether this object is selected
	bool isSelected();
	// Mark this object as selected or unselected
	void setSelected(bool selected);

	glm::vec3* getPositionPointer();
	glm::vec3* getRotationPointer();
	glm::vec3* getScalePointer();

	std::string& getName();

protected:
	// Abstract class, no need to instantiate this class
	Object();
	virtual ~Object() {}

	// Generate and get the rotation matrix
	glm::mat4 getRotationMatrix();

	// Transformations
	glm::vec3 position{ glm::vec3(0.0f) };
	glm::vec3 rotation{ glm::vec3(0.0f) };
	glm::vec3 scaleVector{ glm::vec3(1.0f) };

	// Whether this object's data has been modified
	bool updated{ true };

	// Whether this object is selected by the user, used for deciding whether to draw an outline
	bool selected{ false };

	std::string name;
};