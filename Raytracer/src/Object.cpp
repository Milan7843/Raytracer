#include "Object.h"

bool Object::writeToShader(AbstractShader* shader, unsigned int ssbo)
{
	return false;
}

void Object::writeDataToStream(std::ofstream& filestream)
{
	filestream << getName() << "\n";
	filestream << position.x << " " << position.y << " " << position.z << "\n";
	filestream << rotation.x << " " << rotation.y << " " << rotation.z << "\n";
	filestream << scaleVector.x << " " << scaleVector.y << " " << scaleVector.z << "\n";
}

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

glm::mat4 Object::getTransformationMatrix()
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

std::string& Object::getName()
{
	return name;
}

glm::mat4 Object::getRotationMatrix()
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
