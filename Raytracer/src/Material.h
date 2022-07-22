#pragma once

#include <glm/glm.hpp>
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>

// Forward declaration of the Shader class
class AbstractShader;

class Material
{
public:
	Material(std::string name, glm::vec3 color, float reflectiveness, float transparency, glm::vec3 emission);
	Material(std::string name, glm::vec3 color, float reflectiveness, float transparency, float refractiveness);
	~Material();
	glm::vec3 color;
	float reflectiveness;
	float transparency;
	float refractiveness;
	glm::vec3 emission;

	// Write this material into the shader at the provided index
	void writeToShader(AbstractShader* shader, unsigned int index);

	// Get a pointer to the name of this material
	std::string* getNamePointer();
	glm::vec3* getColorPointer();
	float* getReflectivenessPointer();
	float* getTransparencyPointer();
	float* getRefractivenessPointer();
	glm::vec3* getEmissionPointer();
protected:


private:
	std::string name;
};