#pragma once

#include "Light.h"

Light::Light(const std::string& name, glm::vec3 position, glm::vec3 color, float intensity)
	: name(name),
	position(position),
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

std::string& Light::getName()
{
	return name;
}
