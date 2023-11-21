#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <vector>
#include <iostream>

#include "../shaders/AbstractShader.h"
#include "../ShaderWritable.h"
#include "../Object.h"

#include "../CoordinateUtility.h"

class Light : public Object, public ShaderWritable
{
public:
	virtual bool writeToShader(AbstractShader* shader);
	bool writePositionToShader(AbstractShader* shader);

	// Write this light to the given filestream
	void writeDataToStream(std::ofstream& filestream) override;

	void setIndex(unsigned int index);

	glm::vec3 getColor();

	glm::vec3* getPositionPointer();
	glm::vec3* getColorPointer();
	float* getIntensityPointer();
	float* getShadowSoftnessPointer();

	~Light();
protected:
	Light(const std::string& name, glm::vec3 position, glm::vec3 color, float intensity, float shadowSoftness);
	unsigned int index{ 0 };
	glm::vec3 color{ glm::vec3(0.0f) };
	float intensity{ 1.0f };
	float shadowSoftness{ 0.0f };
};