#pragma once

#include "Model.h"

class Sphere : public Model
{
public:
	Sphere(std::string& name, glm::vec3 position, float radius, unsigned int materialIndex);
	Sphere(glm::vec3 position, float radius, unsigned int materialIndex, unsigned int shaderSphereIndex);
	~Sphere();

	// Write this sphere to the given filestream
	virtual void writeDataToStream(std::ofstream& filestream);

	// Write this material to the stream (human readable format)
	friend std::ostream& operator<< (std::ostream& stream, const Sphere& sphere);

	// Write this object's data to the given shader
	virtual void writeToShader(AbstractShader* shader, unsigned int ssbo);

	// Scale this sphere
	virtual void scale(float scale);
	virtual void scale(glm::vec3 scale);

	void setShaderSphereIndex(unsigned int shaderSphereIndex);

	// Get a single matrix which includes all transformations
	glm::mat4 getTransformationMatrix();

	// Get a pointer to the radius of this sphere
	float* getRadiusPointer();

	unsigned int* getMaterialIndexPointer();

private:
	float radius{ 1.0f };
	unsigned int shaderSphereIndex;
	unsigned int meshCount{ 0 };
	unsigned int triangleCount{ 0 };

	// The index of the material this sphere uses
	unsigned int materialIndex{ 0 };
};