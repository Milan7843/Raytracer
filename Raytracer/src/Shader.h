#pragma once
#ifndef SHADER_H
#define SHADER_H

#include "AbstractShader.h"

class Scene;

class Shader : public AbstractShader
{
public:
	Shader(const char* vertexPath, const char* fragmentPath);
	~Shader();
};

#endif