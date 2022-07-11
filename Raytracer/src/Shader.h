#pragma once
#ifndef SHADER_H
#define SHADER_H

#include "AbstractShader.h"
//#include "Scene.h"

class Scene;

class Shader : public AbstractShader
{
public:
	Shader(const char* vertexPath, const char* fragmentPath);
	Shader(const char* vertexPath, const char* fragmentPath, Scene* scene);
	~Shader();
};

#endif