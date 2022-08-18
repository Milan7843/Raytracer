#include "DirectionalLight.h"

DirectionalLight::DirectionalLight()
	: Light("Directional light", glm::vec3(0.0f), glm::vec3(1.0f), 1.0f),
	direction(-1.0f, -1.0f, 1.0f)
{

}

DirectionalLight::DirectionalLight(std::string& name, glm::vec3 direction, glm::vec3 color, float intensity)
// Initialising the base class
	: Light(name, glm::vec3(0.0f), color, intensity)
	, direction(direction)
{
}

DirectionalLight::DirectionalLight(glm::vec3 direction, glm::vec3 color, float intensity)
	// Initialising the base class
	: Light("Directional light", glm::vec3(0.0f), color, intensity)
	, direction(direction)
{
}

DirectionalLight::~DirectionalLight()
{
}

void DirectionalLight::writeDataToStream(std::ofstream& filestream)
{
	// Writing basic data using the base class
	Light::writeDataToStream(filestream);

	// Then point light specific data
	filestream << direction.x << " " << direction.y << " " << direction.z << "\n";
}

void DirectionalLight::writeToShader(AbstractShader* shader, bool useGlslCoordinates)
{
	shader->setVector3(("dirLights[" + std::to_string(this->index) + "].dir").c_str(),
		useGlslCoordinates ? CoordinateUtility::vec3ToGLSLVec3(direction) : direction);
	shader->setVector3(("dirLights[" + std::to_string(this->index) + "].color").c_str(), color);
	shader->setFloat(("dirLights[" + std::to_string(this->index) + "].intensity").c_str(), intensity);
}

glm::vec3* DirectionalLight::getDirectionPointer()
{
	return &direction;
}
