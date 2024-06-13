#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "shaders/AbstractShader.h"
#include "ImGuiEditorInterface.h"

#include "Logger.h"

class Object : public ImGuiEditorInterface
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
	virtual glm::mat4 getTransformationMatrix() const;

	// Draw this object given the shader
	virtual void draw(AbstractShader* shader);

	glm::vec3* getPositionPointer();
	glm::vec3* getRotationPointer();
	glm::vec3* getScalePointer();

	glm::vec3 getPosition() const;
	virtual glm::vec3 getRotationPoint() const;
	glm::vec3 getRotation() const;
	glm::vec3 getScale() const;

	const std::string& getName() const;
	std::string* getNamePointer();
	void setName(std::string newName);

	void setTransformation(glm::mat4& transformMatrix);

	// Get an approximation of an appropriate distance the camera should be from the object
	// after clicking the focus button.
	virtual float getAppropriateCameraFocusDistance();

protected:
	// Abstract class, no need to instantiate this class
	Object() : ImGuiEditorInterface() {}
	virtual ~Object() {}

	// Generate and get the rotation matrix
	glm::mat4 getRotationMatrix() const;

	// Transformations
	glm::vec3 position{ glm::vec3(0.0f) };
	glm::vec3 rotation{ glm::vec3(0.0f) };
	glm::vec3 scaleVector{ glm::vec3(1.0f) };

	// Whether this object's data has been modified
	bool updated{ true };

	std::string name;
};