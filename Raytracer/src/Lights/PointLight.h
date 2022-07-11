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

	void writeToShader(AbstractShader* shader);
	void writePositionToShader(AbstractShader* shader);

private:
};

#endif