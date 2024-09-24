#include "DirectionalLight.h"

DirectionalLight::DirectionalLight()
	: Light("Directional light", glm::vec3(0.0f), glm::vec3(1.0f), 1.0f, 0.0f),
	direction(-1.0f, -1.0f, 1.0f)
{
	setType(DIRECTIONAL_LIGHT);
}

DirectionalLight::DirectionalLight(std::string& name, glm::vec3 direction, glm::vec3 color, float intensity, float shadowSoftness)
// Initialising the base class
	: Light(name, glm::vec3(0.0f), color, intensity, shadowSoftness)
	, direction(direction)
{
	setType(DIRECTIONAL_LIGHT);
}

DirectionalLight::DirectionalLight(glm::vec3 direction, glm::vec3 color, float intensity, float shadowSoftness)
	// Initialising the base class
	: Light("Directional light", glm::vec3(0.0f), color, intensity, shadowSoftness)
	, direction(direction)
{
	setType(DIRECTIONAL_LIGHT);
}

DirectionalLight::~DirectionalLight()
{
}

bool DirectionalLight::drawInterface(Scene& scene)
{
	bool anyPropertiesChanged{ false };
	anyPropertiesChanged |= ImGui::InputText("##", &name);
	anyPropertiesChanged |= ImGui::ColorEdit3("Color", (float*)&color);
	anyPropertiesChanged |= ImGui::DragFloat("Intensity", &intensity, 0.01f, 0.0f, 10.0f, "%.2f");
	anyPropertiesChanged |= ImGui::DragFloat3("Direction", (float*)&direction, 0.01f);
	anyPropertiesChanged |= ImGui::DragFloat("Shadow softness", &shadowSoftness, 0.01f, 0.0f, 10.0f, "%.2f");

	// If anything changed, no shader will have the updated data
	if (anyPropertiesChanged)
	{
		clearShaderWrittenTo();
	}

	if (anyPropertiesChanged)
	{
		markUnsavedChanges();
	}

	return anyPropertiesChanged;
}

bool DirectionalLight::writeToShader(AbstractShader* shader, bool useGlslCoordinates)
{
	if (hasWrittenToShader(shader))
	{
		// No data was updated
		return false;
	}

	//Logger::log("wrote directional light to shader");

	shader->setVector3(("dirLights[" + std::to_string(this->index) + "].dir").c_str(),
		useGlslCoordinates ? CoordinateUtility::vec3ToGLSLVec3(direction) : direction);
	shader->setVector3(("dirLights[" + std::to_string(this->index) + "].color").c_str(), color);
	shader->setFloat(("dirLights[" + std::to_string(this->index) + "].intensity").c_str(), intensity);
	shader->setFloat(("dirLights[" + std::to_string(this->index) + "].shadowSoftness").c_str(), shadowSoftness);

	// The given shader now has updated data
	markShaderAsWrittenTo(shader);

	// New data was written
	return true;
}

glm::vec3 DirectionalLight::getDirection() const
{
	return direction;
}

void DirectionalLight::setDirection(glm::vec3 newDirection)
{
	this->direction = newDirection;
}
