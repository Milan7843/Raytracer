#pragma once

#include "Light.h"

Light::Light(const std::string& name, glm::vec3 position, glm::vec3 color, float intensity, float shadowSoftness)
	: color(color),
	intensity(intensity),
	shadowSoftness(shadowSoftness),
	index(0)
{
	this->name = name;
	this->position = position;
}

Light::~Light()
{
}

bool Light::writeToShader(AbstractShader* shader)
{
	return false;
}

bool Light::writePositionToShader(AbstractShader* shader)
{
	return false;
}

void Light::setIndex(unsigned int index)
{
	this->index = index;
}

glm::vec3 Light::getColor() const
{
	return color;
}

float Light::getIntensity() const
{
	return intensity;
}

void Light::setIntensity(float intensity)
{
	this->intensity = intensity;
}

void Light::setColor(glm::vec3 color)
{
	this->color = color;
}
