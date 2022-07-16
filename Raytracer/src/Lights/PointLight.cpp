#pragma once

#include "PointLight.h"

PointLight::PointLight(glm::vec3 position, glm::vec3 color, float intensity)
	: Light(position, color, intensity)
{
}

PointLight::~PointLight()
{
}

void PointLight::writeToShader(AbstractShader* shader)
{
	writePositionToShader(shader);
	shader->setVector3(("pointLights[" + std::to_string(this->index) + "].color").c_str(), color);
	shader->setFloat(("pointLights[" + std::to_string(this->index) + "].intensity").c_str(), intensity);
}

void PointLight::writePositionToShader(AbstractShader* shader)
{
	shader->setVector3(("pointLights[" + std::to_string(this->index) + "].position").c_str(), CoordinateUtility::vec3ToGLSLVec3(position));
}