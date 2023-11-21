#pragma once
#ifndef POINT_LIGHT_H
#define POINT_LIGHT_H

#include "Light.h"

#include "../shaders/AbstractShader.h"

class PointLight : public Light
{
public:
	PointLight(std::string& name, glm::vec3 position, glm::vec3 color, float intensity, float shadowSoftness);
	PointLight(glm::vec3 position, glm::vec3 color, float intensity, float shadowSoftness);
	~PointLight();

	virtual bool drawInterface(Scene& scene);

	// Write this light to the given filestream
	void writeDataToStream(std::ofstream& filestream) override;

	// Write all the data of this point light into the given shader
	bool writeToShader(AbstractShader* shader, bool useGlslCoordinates);

	// Write only the position of this point light into the given shader
	bool writePositionToShader(AbstractShader* shader, bool useGlslCoordinates);

private:
};

#endif