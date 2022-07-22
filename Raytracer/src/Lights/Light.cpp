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

void Light::writeToShader(AbstractShader* shader)
{
}

void Light::writePositionToShader(AbstractShader* shader)
{
}

void Light::setIndex(unsigned int index)
{
	this->index = index;
}

glm::vec3* Light::getPositionPointer()
{
	return &position;
}

glm::vec3* Light::getColorPointer()
{
	return &color;
}

float* Light::getIntensityPointer()
{
	return &intensity;
}
