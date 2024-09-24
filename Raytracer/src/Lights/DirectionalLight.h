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

	// Write all the data of this directional light into the given shader
	virtual bool writeToShader(AbstractShader* shader, bool useGlslCoordinates);

	// Get a pointer to the direction vector of this light
	glm::vec3* getDirectionPointer() const;

	// Get the direction vector of this light
	glm::vec3 getDirection() const;

	// Set the direction vector of this light
	void setDirection(glm::vec3 newDirection);

private:
	// The direction this light points in
	glm::vec3 direction;
};

