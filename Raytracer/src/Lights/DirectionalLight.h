#pragma once

#include "Light.h"

#include "../shaders/AbstractShader.h"

class DirectionalLight : public Light
{
public:
	DirectionalLight();
	DirectionalLight(std::string& name, glm::vec3 direction, glm::vec3 color, float intensity, float shadowSoftness);
	DirectionalLight(glm::vec3 direction, glm::vec3 color, float intensity, float shadowSoftness);
	~DirectionalLight();

	virtual bool drawInterface(Scene& scene);

	// Write this light to the given filestream
	void writeDataToStream(std::ofstream& filestream) override;

	// Write all the data of this directional light into the given shader
	virtual bool writeToShader(AbstractShader* shader, bool useGlslCoordinates);

	// Get a pointer the the direction vector of this light
	glm::vec3* getDirectionPointer();

private:
	// The direction this light points in
	glm::vec3 direction;
};

