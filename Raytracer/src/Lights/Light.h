#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <vector>
#include <iostream>

#include "../AbstractShader.h"

#include "../CoordinateUtility.h"

class Light
{
public:
	void writeToShader(AbstractShader* shader);
	void writePositionToShader(AbstractShader* shader);

	// Write this light to the given filestream
	virtual void writeDataToStream(std::ofstream& filestream);

	void setIndex(unsigned int index);

	glm::vec3* getPositionPointer();
	glm::vec3* getColorPointer();
	float* getIntensityPointer();

	std::string& getName();

	~Light();
protected:
	Light(const std::string& name, glm::vec3 position, glm::vec3 color, float intensity);
	unsigned int index = 0;
	glm::vec3 position = glm::vec3(0.0f);
	glm::vec3 color = glm::vec3(0.0f);
	float intensity = 1.0f;

	std::string name;
};