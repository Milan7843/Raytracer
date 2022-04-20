#pragma once

#include "Model.h"

class Sphere : public Model
{
public:
	Sphere(glm::vec3 position, float radius, unsigned int materialIndex, unsigned int shaderSphereIndex);
	~Sphere();

	// Write this object's data to the given shader
	virtual void writeToShader(Shader* shader, unsigned int ssbo);

	// Scale this sphere
	virtual void scale(float scale);

private:
	float radius = 1.0f;
	unsigned int shaderSphereIndex;
	unsigned int meshCount = 0;
	unsigned int triangleCount = 0;
};