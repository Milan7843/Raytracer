#pragma once
#ifndef SHADER_H
#define SHADER_H

#include <glad/glad.h>

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>

// Matrix math
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

// Forward declaration of scene
class Scene;


class Shader
{
public:
	// Program ID
	unsigned int ID;
	Shader(const char* vertexPath, const char* fragmentPath);
	Shader(const char* vertexPath, const char* fragmentPath, Scene* scene);
	~Shader();

	// Activate the shader
	void use();

	// Compile the shader
	void compile();
	void compile(Scene* scene);


	// Uniform setting functions
	void setBool(const std::string &name, bool value) const;
	void setFloat(const std::string &name, float value) const;
	void setInt(const std::string &name, int value) const;
	void setVector2(const std::string& name, float v1, float v2) const;
	void setVector2(const std::string& name, glm::vec2 v) const;
	void setVector3(const std::string& name, float v1, float v2, float v3) const;
	void setVector3(const std::string& name, glm::vec3 v) const;
	void setMat4(const std::string& name, glm::mat4 matrix) const;

private:
	unsigned int compileShader(GLenum type, const char* code);

	const char* vertexPath;
	const char* fragmentPath;
};

#endif