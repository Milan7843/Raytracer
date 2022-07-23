#pragma once

#include "Light.h"

#include "../AbstractShader.h"

class AmbientLight : public Light
{
	AmbientLight(glm::vec3 color, float intensity);
	~AmbientLight();

	// Write all the data of this directional light into the given shader
	void writeToShader(AbstractShader* shader);
};

