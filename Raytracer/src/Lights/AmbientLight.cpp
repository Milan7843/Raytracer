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

void AmbientLight::drawInterface(Scene& scene)
{
	bool anyPropertiesChanged{ false };
	anyPropertiesChanged |= ImGui::InputText("##", &getName());
	anyPropertiesChanged |= ImGui::ColorEdit3("Color", (float*)getColorPointer());
	anyPropertiesChanged |= ImGui::DragFloat("Intensity", getIntensityPointer(), 0.01f, 0.0f, 10.0f, "%.2f");

	// If anything changed, no shader will have the updated data
	if (anyPropertiesChanged)
	{
		clearShaderWrittenTo();
	}
}

bool AmbientLight::writeToShader(AbstractShader* shader)
{
	if (hasWrittenToShader(shader))
	{
		// No data was updated
		return false;
	}

	//Logger::log("wrote ambient light to shader");

	shader->setVector3(("ambientLights[" + std::to_string(this->index) + "].color").c_str(), color);
	shader->setFloat(("ambientLights[" + std::to_string(this->index) + "].intensity").c_str(), intensity);

	// The given shader now has updated data
	markShaderAsWrittenTo(shader);

	// New data was written
	return true;
}