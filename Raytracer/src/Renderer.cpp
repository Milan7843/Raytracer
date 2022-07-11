#include "Renderer.h"

Renderer::Renderer(const char* raytraceComputeShaderPath)
	: computeShader(raytraceComputeShaderPath)
{

}

Renderer::~Renderer()
{
}

void Renderer::Render(Scene* scene, Camera* camera, unsigned int buffer)
{
	/*
	if (scene->hasPendingChanges())
	{
		scene->writeLightsToShader(&computeShader);
		scene->writeMaterialsToShader(&computeShader);
	}*/
}
