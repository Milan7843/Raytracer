#pragma once

#include "Model.h"

class Sphere : public Model
{
public:
	Sphere(unsigned int materialIndex);
	Sphere(std::string& name, glm::vec3 position, float radius, unsigned int materialIndex);
	Sphere(glm::vec3 position, float radius, unsigned int materialIndex, unsigned int shaderSphereIndex);
	~Sphere();

	// Write this material to the stream (human readable format)
	friend std::ostream& operator<< (std::ostream& stream, const Sphere& sphere);

	// Write this object's data to the given shader
	virtual bool writeToShader(AbstractShader* shader, unsigned int ssbo);

	// Scale this sphere
	virtual void scale(float scale);
	virtual void scale(glm::vec3 scale);

	// Draw an interface for this sphere using ImGui
	virtual void drawInterface();

	// Set the index of this sphere in the shader (all sphere must have contiguous indices)
	void setShaderSphereIndex(unsigned int shaderSphereIndex);

	// Get a single matrix which includes all transformations
	glm::mat4 getTransformationMatrix();

	float getRadius() const;

	void setRadius(float radius);

	unsigned int getMaterialIndex() const;

private:
	float radius{ 1.0f };
	unsigned int shaderSphereIndex;
	unsigned int meshCount{ 0 };
	unsigned int triangleCount{ 0 };
};