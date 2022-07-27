#include "DirectionalLight.h"

DirectionalLight::DirectionalLight(glm::vec3 direction, glm::vec3 color, float intensity)
	// Initialising the base class
	: Light(glm::vec3(0.0f), color, intensity)
	, direction(direction)
{
}

DirectionalLight::~DirectionalLight()
{
}

void DirectionalLight::writeToShader(AbstractShader* shader)
{
	shader->setVector3(("dirLights[" + std::to_string(this->index) + "].dir").c_str(), direction);
	shader->setVector3(("dirLights[" + std::to_string(this->index) + "].color").c_str(), color);
	shader->setFloat(("dirLights[" + std::to_string(this->index) + "].intensity").c_str(), intensity);
}

glm::vec3* DirectionalLight::getDirectionPointer()
{
	return &direction;
}
