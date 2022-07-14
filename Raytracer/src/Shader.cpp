
#include "Shader.h"

#include "Scene.h"

Shader::Shader(const char* vertexPath, const char* fragmentPath)
{
	std::string vertexCode = readFile(vertexPath);
	std::string fragmentCode = readFile(fragmentPath);
	const char* vShaderCode = vertexCode.c_str();
	const char* fShaderCode = fragmentCode.c_str();


	/* Compiling the shaders */

	unsigned int vertex, fragment;
	vertex = compileShader(GL_VERTEX_SHADER, vShaderCode);
	fragment = compileShader(GL_FRAGMENT_SHADER, fShaderCode);


	/* Creating the shader program */
	ID = glCreateProgram();
	glAttachShader(ID, vertex);
	glAttachShader(ID, fragment);
	linkProgram();

	// Deleting the shaders as they're linked into our program now and no longer necessary
	glDeleteShader(vertex);
	glDeleteShader(fragment);
}

Shader::Shader(const char* vertexPath, const char* fragmentPath, Scene* scene)
{
	std::string vertexCode = readFile(vertexPath);
	std::string fragmentCode = readFile(fragmentPath);
	fragmentCode = scene->setShaderVariables(fragmentCode);
	const char* vShaderCode = vertexCode.c_str();
	const char* fShaderCode = fragmentCode.c_str();


	/* Compiling the shaders */

	unsigned int vertex, fragment;
	vertex = compileShader(GL_VERTEX_SHADER, vShaderCode);
	fragment = compileShader(GL_FRAGMENT_SHADER, fShaderCode);


	/* Creating the shader program */

	ID = glCreateProgram();
	glAttachShader(ID, vertex);
	glAttachShader(ID, fragment);
	linkProgram();

	// Deleting the shaders as they're linked into our program now and no longer necessary
	glDeleteShader(vertex);
	glDeleteShader(fragment);
}

Shader::~Shader()
{
	std::cout << "Shader destroyed." << std::endl;
}