#pragma once
#ifndef POINT_LIGHT_H
#define POINT_LIGHT_H

#include "Light.h"

#include "../AbstractShader.h"

class PointLight : public Light
{
public:
	PointLight(glm::vec3 position, glm::vec3 color, float intensity);
	~PointLight();

	// Write all the data of this point light into the given shader
	void writeToShader(AbstractShader* shader);

	// Write only the position of this point light into the given shader
	void writePositionToShader(AbstractShader* shader);

private:
};

#endif