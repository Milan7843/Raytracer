#pragma once

#include "Light.h"

#include "../shaders/AbstractShader.h"

class DirectionalLight : public Light
{
public:
	DirectionalLight();
	DirectionalLight(std::string& name, glm::vec3 direction, glm::vec3 color, float intensity);
	DirectionalLight(glm::vec3 direction, glm::vec3 color, float intensity);
	~DirectionalLight();

	virtual void drawInterface(Scene& scene);

	// Write this light to the given filestream
	virtual void writeDataToStream(std::ofstream& filestream);

	// Write all the data of this directional light into the given shader
	virtual bool writeToShader(AbstractShader* shader, bool useGlslCoordinates);

	// Get a pointer the the direction vector of this light
	glm::vec3* getDirectionPointer();

private:
	// The direction this light points in
	glm::vec3 direction;
};

