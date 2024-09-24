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

	void setIndex(unsigned int index);

	glm::vec3 getColor() const;
	float getIntensity() const;

	void setIntensity(float intensity);
	void setColor(glm::vec3 color);

	~Light();
protected:
	Light(const std::string& name, glm::vec3 position, glm::vec3 color, float intensity, float shadowSoftness);
	unsigned int index{ 0 };
	glm::vec3 color{ glm::vec3(0.0f) };
	float intensity{ 1.0f };
	float shadowSoftness{ 0.0f };
};