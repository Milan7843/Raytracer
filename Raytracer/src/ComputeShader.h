#pragma once

#include "AbstractShader.h"

class ComputeShader : public AbstractShader
{
public:
	ComputeShader(const char* shaderPath);
	~ComputeShader();
};

