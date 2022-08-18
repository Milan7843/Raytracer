#pragma once

#include "Light.h"

#include "../AbstractShader.h"

class AmbientLight : public Light
{
public:
	AmbientLight(std::string& name, glm::vec3 color, float intensity);
	AmbientLight(glm::vec3 color, float intensity);
	~AmbientLight();

	// Write all the data of this directional light into the given shader
	virtual void writeToShader(AbstractShader* shader);
};

