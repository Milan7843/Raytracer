#pragma once

#include "PointLight.h"

PointLight::PointLight(glm::vec3 position, glm::vec3 color, float intensity)
	: Light(position, color, intensity)
{
}

PointLight::~PointLight()
{
}

void PointLight::writeToShader(Shader* shader)
{
	std::cout << "setting: " << ("pointLights[" + std::to_string(this->index) + "].pos").c_str() << std::endl;
	shader->setVector3(("pointLights[" + std::to_string(this->index) + "].pos").c_str(), position);
	shader->setVector3(("pointLights[" + std::to_string(this->index) + "].color").c_str(), color);
	shader->setFloat(("pointLights[" + std::to_string(this->index) + "].intensity").c_str(), intensity);
}

void PointLight::writePositionToShader(Shader* shader)
{
	shader->setVector3(("pointLights[" + std::to_string(this->index) + "].position").c_str(), position);
}