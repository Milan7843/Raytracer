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

void Light::writeDataToStream(std::ofstream& filestream)
{
	filestream << name << "\n";
	filestream << color.r << " " << color.g << " " << color.b << "\n";
	filestream << intensity << "\n";
}

void Light::setIndex(unsigned int index)
{
	this->index = index;
}

glm::vec3 Light::getColor()
{
	return color;
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

float* Light::getShadowSoftnessPointer()
{
	return &shadowSoftness;
}
