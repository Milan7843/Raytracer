#include "AmbientLight.h"

AmbientLight::AmbientLight(std::string& name, glm::vec3 color, float intensity)
	// Initialising the base class
	: Light(name, glm::vec3(0.0f), color, intensity)
{
}

AmbientLight::AmbientLight(glm::vec3 color, float intensity)
	// Initialising the base class
	: Light(std::string("Ambient light"), glm::vec3(0.0f), color, intensity)
{
}

AmbientLight::~AmbientLight()
{
}

void AmbientLight::writeToShader(AbstractShader* shader)
{
	shader->setVector3(("ambientLights[" + std::to_string(this->index) + "].color").c_str(), color);
	shader->setFloat(("ambientLights[" + std::to_string(this->index) + "].intensity").c_str(), intensity);
}