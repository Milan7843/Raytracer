#pragma once

#include "PointLight.h"

PointLight::PointLight(std::string& name, glm::vec3 position, glm::vec3 color, float intensity, float shadowSoftness)
	: Light(name, position, color, intensity, shadowSoftness)
{
	setType(POINT_LIGHT);
}

PointLight::PointLight(glm::vec3 position, glm::vec3 color, float intensity, float shadowSoftness)
	: Light("Point light", position, color, intensity, shadowSoftness)
{
	setType(POINT_LIGHT);
}

PointLight::~PointLight()
{
}

bool PointLight::drawInterface(Scene& scene)
{
	bool anyPropertiesChanged{ false };
	anyPropertiesChanged |= ImGui::InputText("##", &getName());
	anyPropertiesChanged |= ImGui::ColorEdit3("Color", (float*)getColorPointer());
	anyPropertiesChanged |= ImGui::DragFloat("Intensity", getIntensityPointer(), 0.01f, 0.0f, 10.0f, "%.2f");
	anyPropertiesChanged |= ImGui::DragFloat3("Position", (float*)getPositionPointer(), 0.01f);
	anyPropertiesChanged |= ImGui::DragFloat("Shadow softness", getShadowSoftnessPointer(), 0.01f, 0.0f, 10.0f, "%.2f");

	// If anything changed, no shader will have the updated data
	if (anyPropertiesChanged)
	{
		clearShaderWrittenTo();
	}

	return anyPropertiesChanged;
}

void PointLight::writeDataToStream(std::ofstream& filestream)
{
	// Writing basic data using the base class
	Light::writeDataToStream(filestream);

	// Then point light specific data
	filestream << position.x << " " << position.y << " " << position.z << "\n";
	filestream << shadowSoftness << "\n";
}

bool PointLight::writeToShader(AbstractShader* shader, bool useGlslCoordinates)
{
	if (hasWrittenToShader(shader))
	{
		// No data was updated
		return false;
	}

	//Logger::log("wrote point light to shader");

	writePositionToShader(shader, useGlslCoordinates);
	shader->setVector3(("pointLights[" + std::to_string(this->index) + "].color").c_str(), color);
	shader->setFloat(("pointLights[" + std::to_string(this->index) + "].intensity").c_str(), intensity);
	shader->setFloat(("pointLights[" + std::to_string(this->index) + "].shadowSoftness").c_str(), shadowSoftness);

	// The given shader now has updated data
	markShaderAsWrittenTo(shader);

	// New data was written
	return true;
}

bool PointLight::writePositionToShader(AbstractShader* shader, bool useGlslCoordinates)
{
	if (hasWrittenToShader(shader))
	{
		// No data was updated
		return false;
	}

	shader->setVector3(("pointLights[" + std::to_string(this->index) + "].pos").c_str(),
		useGlslCoordinates ? CoordinateUtility::vec3ToGLSLVec3(position) : position);

	// The given shader now has updated data
	markShaderAsWrittenTo(shader);

	// New data was written
	return true;
}