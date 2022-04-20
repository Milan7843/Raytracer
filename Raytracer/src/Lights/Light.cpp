#pragma once

#include "Light.h"

Light::Light(glm::vec3 position, glm::vec3 color, float intensity)
	: position(position),
	color(color),
	intensity(intensity),
	index(0)
{
}

Light::~Light()
{
}

void Light::writeToShader(Shader* shader)
{
}

void Light::writePositionToShader(Shader* shader)
{
}

void Light::setIndex(unsigned int index)
{
	this->index = index;
}
