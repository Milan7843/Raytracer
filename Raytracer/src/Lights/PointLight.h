#pragma once
#ifndef POINT_LIGHT_H
#define POINT_LIGHT_H

#include "Light.h"

#include "../Shader.h"

class PointLight : public Light
{
public:
	PointLight(glm::vec3 position, glm::vec3 color, float intensity);
	~PointLight();

	void writeToShader(Shader* shader);
	void writePositionToShader(Shader* shader);

private:
};

#endif