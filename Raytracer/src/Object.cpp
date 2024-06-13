#include "Object.h"

void Object::rotate(glm::vec3 rotation)
{
	this->rotation += rotation;
}

void Object::setRotation(glm::vec3 rotation)
{
	this->rotation = rotation;
}

void Object::move(glm::vec3 move)
{
	this->position = position + move;
}

void Object::setPosition(glm::vec3 pos)
{
	this->position = pos;
}

void Object::scale(glm::vec3 scale)
{
	this->scaleVector = scaleVector * scale;
}

void Object::scale(float scale)
{
	this->scale(glm::vec3(1.0f) * scale);
}

void Object::resetRotation()
{
	this->rotation = glm::vec3(0.0f);
}

glm::mat4 Object::getTransformationMatrix() const
{
	// Creating an identity matrix
	glm::mat4 baseMatrix = glm::mat4(1.0f);

	// Applying the transformations
	baseMatrix = glm::translate(baseMatrix, position);
	baseMatrix *= getRotationMatrix();
	baseMatrix = glm::scale(baseMatrix, scaleVector);

	return baseMatrix;
}

glm::vec3* Object::getPositionPointer()
{
	return &position;
}

glm::vec3* Object::getRotationPointer()
{
	return &rotation;
}

glm::vec3* Object::getScalePointer()
{
	return &scaleVector;
}

glm::vec3 Object::getPosition() const
{
	return position;
}

glm::vec3 Object::getRotationPoint() const
{
	return getPosition();
}

glm::vec3 Object::getRotation() const
{
	return rotation;
}

glm::vec3 Object::getScale() const
{
	return scaleVector;
}

const std::string& Object::getName() const
{
	return name;
}

std::string* Object::getNamePointer()
{
	return &name;
}

void Object::setName(std::string newName)
{
	this->name = newName;
}

void Object::setTransformation(glm::mat4& transformMatrix)
{
	// Extract position
	position = glm::vec3(transformMatrix[3]);

	// Extract scale
	scaleVector.x = glm::length(glm::vec3(transformMatrix[0]));
	scaleVector.y = glm::length(glm::vec3(transformMatrix[1]));
	scaleVector.z = glm::length(glm::vec3(transformMatrix[2]));

	// Extract rotation
	glm::mat3 rotationMatrix(transformMatrix);
	glm::quat rotationQuat(rotationMatrix);
	rotation = glm::eulerAngles(rotationQuat);
}

float Object::getAppropriateCameraFocusDistance()
{
	return 0.5f;
}

glm::mat4 Object::getRotationMatrix() const
{
	// Creating an identity matrix
	glm::mat4 rotationMatrix(1.0f);

	// Applying z-rotation
	rotationMatrix = glm::rotate(rotationMatrix, rotation.z, glm::vec3(0.0f, 0.0f, 1.0f));
	// Applying y-rotation
	rotationMatrix = glm::rotate(rotationMatrix, rotation.y, glm::vec3(0.0f, 1.0f, 0.0f));
	// Applying x-rotation
	rotationMatrix = glm::rotate(rotationMatrix, rotation.x, glm::vec3(1.0f, 0.0f, 0.0f));

	return rotationMatrix;
}

void Object::draw(AbstractShader* shader)
{
}
