#pragma once

#include <glm/glm.hpp>
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>

// Forward declaration of the Shader class
class Shader;

class Material
{
public:
	Material(glm::vec3 color, float reflectiveness, float transparency);
	~Material();
	glm::vec3 color;
	float reflectiveness;
	float transparency;

	// Write this material into the shader at the provided index
	void writeToShader(Shader* shader, unsigned int index);

protected:


private:

};