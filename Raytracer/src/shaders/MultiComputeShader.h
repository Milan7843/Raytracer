#pragma once

#include "ComputeShader.h"

#include <vector>
#include <cstdarg>

class MultiComputeShader : public ComputeShader
{
public:
	MultiComputeShader(unsigned int count, ...);
	~MultiComputeShader();

	// Activates the shader
	void use() override;

	void run(unsigned int numGroupsX, unsigned int numGroupsY, unsigned int numGroupsZ) const override;

	// Uniform setting functions
	void setBool(const std::string& name, bool value) const override;
	void setFloat(const std::string& name, float value) const override;
	void setDouble(const std::string& name, double value) const override;
	void setInt(const std::string& name, int value) const override;
	void setVector2(const std::string& name, float v1, float v2) const override;
	void setVector2(const std::string& name, glm::vec2 v) const override;
	void setVector3(const std::string& name, float v1, float v2, float v3) const override;
	void setVector3(const std::string& name, glm::vec3 v) const override;
	void setMat4(const std::string& name, glm::mat4 matrix) const override;

	// Delete the shader in this object
	void deleteProgram() override;

private:
	std::vector<ComputeShader*> shaders = {};
};

