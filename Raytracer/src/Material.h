#pragma once

#include "ShaderWritable.h"
#include "ImGuiUtility.h"

#include <glm/glm.hpp>
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>

// Forward declaration of the Shader class
class AbstractShader;

class Material : public ShaderWritable
{
public:
	// Default constructor for new material
	Material();

	// Actual constructors
	Material(std::string name, glm::vec3 color, float reflectiveness, float transparency, float refractiveness, float reflectionDiffusion, glm::vec3 emission, float emissionStrength, float fresnelReflectionStrength);
	Material(std::string name, glm::vec3 color, float reflectiveness, float transparency, glm::vec3 emission, float emissionStrength);
	Material(std::string name, glm::vec3 color, float reflectiveness, float transparency, float refractiveness);
	~Material();

	// Write this light to the given filestream
	void writeDataToStream(std::ofstream& filestream);

	// Draw an interface for this model using ImGui
	virtual bool drawInterface(Scene& scene);

	// Generate a material with easily distinguishable error properties
	static Material generateErrorMaterial();

	glm::vec3 color;
	float reflectiveness;
	float transparency;
	float refractiveness;
	float reflectionDiffusion;
	glm::vec3 emission;
	float emissionStrength;
	float fresnelReflectionStrength;

	// Write this material into the shader at the provided index
	// Returns whether any data was written
	bool writeToShader(AbstractShader* shader, unsigned int index);

	// Get a pointer to the name of this material
	std::string* getNamePointer();
	glm::vec3* getColorPointer();
	float* getReflectivenessPointer();
	float* getTransparencyPointer();
	float* getRefractivenessPointer();
	float* getReflectionDiffusionPointer();
	glm::vec3* getEmissionPointer();

	// Write this material to the stream (human readable format)
	friend std::ostream& operator<< (std::ostream& stream, const Material& material);
protected:


private:
	std::string name;
};