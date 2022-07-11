#include "Object.h"

void Object::writeToShader(AbstractShader* shader, unsigned int ssbo)
{

}

void Object::applyTransformations()
{
}

void Object::rotate(glm::vec3 rotationAxis, float degrees)
{
	this->transformation = glm::rotate(transformation, degrees, rotationAxis);
}

void Object::move(glm::vec3 move)
{
	this->transformation = glm::translate(transformation, move);
}

void Object::scale(glm::vec3 scale)
{
	this->transformation = glm::scale(transformation, scale);
}

void Object::scale(float scale)
{
	this->scale(glm::vec3(1.0f) * scale);
}

Object::Object()
{
}

void Object::draw(AbstractShader* shader)
{
}
