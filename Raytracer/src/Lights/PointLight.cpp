#pragma once

#include "PointLight.h"

PointLight::PointLight(std::string& name, glm::vec3 position, glm::vec3 color, float intensity)
	: Light(name, position, color, intensity)
{
}

PointLight::PointLight(glm::vec3 position, glm::vec3 color, float intensity)
	: Light("Point light", position, color, intensity)
{
}

PointLight::~PointLight()
{
}

void PointLight::drawInterface(Scene& scene)
{
	ImGui::InputText("##", &getName());
	ImGui::ColorEdit3("Color", (float*)getColorPointer());
	ImGui::DragFloat("Intensity", getIntensityPointer(), 0.01f, 0.0f, 10.0f, "%.2f");
	ImGui::DragFloat3("Position", (float*)getPositionPointer(), 0.01f);
}

void PointLight::writeDataToStream(std::ofstream& filestream)
{
	// Writing basic data using the base class
	Light::writeDataToStream(filestream);

	// Then point light specific data
	filestream << position.x << " " << position.y << " " << position.z << "\n";
}

void PointLight::writeToShader(AbstractShader* shader, bool useGlslCoordinates)
{
	writePositionToShader(shader, useGlslCoordinates);
	shader->setVector3(("pointLights[" + std::to_string(this->index) + "].color").c_str(), color);
	shader->setFloat(("pointLights[" + std::to_string(this->index) + "].intensity").c_str(), intensity);
}

void PointLight::writePositionToShader(AbstractShader* shader, bool useGlslCoordinates)
{
	shader->setVector3(("pointLights[" + std::to_string(this->index) + "].pos").c_str(),
		useGlslCoordinates ? CoordinateUtility::vec3ToGLSLVec3(position) : position);
}