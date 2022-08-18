#pragma once

#include "AbstractShader.h"
#include "Scene.h"

class ComputeShader : public AbstractShader
{
public:
	ComputeShader(const char* shaderPath);
	~ComputeShader();
};

