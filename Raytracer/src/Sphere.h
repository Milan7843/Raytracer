#pragma once

#include "Model.h"

class Sphere : public Model
{
public:
	Sphere(glm::vec3 position, float radius, unsigned int materialIndex, unsigned int shaderSphereIndex);
	~Sphere();

	// Write this object's data to the given shader
	virtual void writeToShader(AbstractShader* shader, unsigned int ssbo);

	// Scale this sphere
	virtual void scale(float scale);
	virtual void scale(glm::vec3 scale);

	// Get a single matrix which includes all transformations
	glm::mat4 getTransformationMatrix();

	// Get a pointer to the radius of this sphere
	float* getRadiusPointer();

private:
	float radius = 1.0f;
	unsigned int shaderSphereIndex;
	unsigned int meshCount = 0;
	unsigned int triangleCount = 0;
};