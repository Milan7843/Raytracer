#include "Object.h"

void Object::writeToShader(AbstractShader* shader, unsigned int ssbo)
{

}

void Object::rotate(glm::vec3 rotationAxis, float degrees)
{
	this->rotationMatrix = glm::rotate(rotationMatrix, degrees, rotationAxis);
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
	this->rotationMatrix = glm::mat4(1.0f);
}

glm::mat4 Object::getTransformationMatrix()
{
	// Creating an identity matrix
	glm::mat4 baseMatrix = glm::mat4(1.0f);

	// Applying the transformations
	baseMatrix = glm::translate(baseMatrix, position);
	baseMatrix = glm::scale(baseMatrix, scaleVector);

	return rotationMatrix  * baseMatrix;
}

Object::Object()
{
}

void Object::draw(AbstractShader* shader)
{
}
