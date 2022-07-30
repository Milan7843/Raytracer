#include "ComputeShader.h"

ComputeShader::ComputeShader(const char* shaderPath)
{
	std::string shaderCode = readFile(shaderPath);
	const char* shaderCodeChars = shaderCode.c_str();


	/* Compiling the shaders */

	unsigned int shader;
	shader = compileShader(GL_COMPUTE_SHADER, shaderCodeChars);


	/* Creating the shader program */

	ID = glCreateProgram();

	glAttachShader(ID, shader);
	linkProgram();

	// delete the shaders as they're linked into our program now and no longer necessary
	glDeleteShader(shader);
}

ComputeShader::ComputeShader(const char* shaderPath, Scene* scene)
{
	std::string shaderCode = readFile(shaderPath);
	scene->setShaderVariables(shaderCode);
	const char* shaderCodeChars = shaderCode.c_str();


	/* Compiling the shaders */

	unsigned int shader;
	shader = compileShader(GL_COMPUTE_SHADER, shaderCodeChars);


	/* Creating the shader program */

	ID = glCreateProgram();
	glAttachShader(ID, shader);
	linkProgram();

	// delete the shaders as they're linked into our program now and no longer necessary
	glDeleteShader(shader);
}

ComputeShader::~ComputeShader()
{
	Logger::log("Compute shader destroyed.");
}