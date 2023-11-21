#pragma once

#include "Light.h"

#include "../shaders/AbstractShader.h"

class AmbientLight : public Light
{
public:
	AmbientLight(std::string& name, glm::vec3 color, float intensity);
	AmbientLight(glm::vec3 color, float intensity);
	~AmbientLight();

	virtual bool drawInterface(Scene& scene);

	bool writeToShader(AbstractShader* shader) override;
};

