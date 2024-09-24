#include "MultiComputeShader.h"

MultiComputeShader::MultiComputeShader(unsigned int count, ...)
{
	va_list args;
	va_start(args, count);

	for (int i = 0; i < count; i++)
	{
		// Getting the path from the argument
		const char* shaderPath = va_arg(args, const char*);

		ComputeShader* computeShader = new ComputeShader(shaderPath);

		shaders.push_back(computeShader);
	}

	va_end(args);
}

MultiComputeShader::~MultiComputeShader()
{
	// Deleting all shader members
	for (int i = 0; i < shaders.size(); i++)
	{
		delete shaders[i];
	}

	Logger::logDebug("Multi compute shader destroyed.");
}

void MultiComputeShader::use()
{
	// The multi compute shader cannot be used
}

void MultiComputeShader::run(unsigned int numGroupsX, unsigned int numGroupsY, unsigned int numGroupsZ) const
{
	for (ComputeShader* shader : shaders)
	{
		shader->use();
		shader->run(numGroupsX, numGroupsY, numGroupsZ);
	}
}

void MultiComputeShader::setBool(const std::string& name, bool value) const
{
	for (ComputeShader* shader : shaders)
	{
		shader->use();
		shader->setBool(name, value);
	}
}

void MultiComputeShader::setFloat(const std::string& name, float value) const
{
	for (ComputeShader* shader : shaders)
	{
		shader->use();
		shader->setFloat(name, value);
	}
}

void MultiComputeShader::setDouble(const std::string& name, double value) const
{
	for (ComputeShader* shader : shaders)
	{
		shader->use();
		shader->setDouble(name, value);
	}
}

void MultiComputeShader::setInt(const std::string& name, int value) const
{
	for (ComputeShader* shader : shaders)
	{
		shader->use();
		shader->setInt(name, value);
	}
}

void MultiComputeShader::setVector2(const std::string& name, float v1, float v2) const
{
	for (ComputeShader* shader : shaders)
	{
		shader->use();
		shader->setVector2(name, v1, v2);
	}
}

void MultiComputeShader::setVector2(const std::string& name, glm::vec2 v) const
{
	for (ComputeShader* shader : shaders)
	{
		shader->use();
		shader->setVector2(name, v);
	}
}

void MultiComputeShader::setVector3(const std::string& name, float v1, float v2, float v3) const
{
	for (ComputeShader* shader : shaders)
	{
		shader->use();
		shader->setVector3(name, v1, v2, v3);
	}
}

void MultiComputeShader::setVector3(const std::string& name, glm::vec3 v) const
{
	for (ComputeShader* shader : shaders)
	{
		shader->use();
		shader->setVector3(name, v);
	}
}

void MultiComputeShader::setMat4(const std::string& name, glm::mat4 matrix) const
{
	for (ComputeShader* shader : shaders)
	{
		shader->use();
		shader->setMat4(name, matrix);
	}
}

void MultiComputeShader::deleteProgram()
{
	for (ComputeShader* shader : shaders)
	{
		shader->deleteProgram();
	}
}
