#pragma once

#include "Scene.h"
#include "Camera.h"
#include "ComputeShader.h"

class Renderer
{
public:
	Renderer(const char* raytraceComputeShaderPath);
	~Renderer();

	// Render the given scene with the given camera to the given buffer
	void Render(Scene* scene, Camera* camera, unsigned int buffer);

private:
	// The compute shader used to render to the buffer
	ComputeShader computeShader;
};

