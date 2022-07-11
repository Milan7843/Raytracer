#include "Renderer.h"

Renderer::Renderer(const char* raytraceComputeShaderPath)
	: computeShader(raytraceComputeShaderPath)
{

}

Renderer::~Renderer()
{
}

void Renderer::render(Scene* scene, Camera* camera, unsigned int buffer)
{
	/*
	if (scene->hasPendingChanges())
	{
		scene->writeLightsToShader(&computeShader);
		scene->writeMaterialsToShader(&computeShader);
	}*/
	computeShader.setVector3("cameraPosition", camera->getPosition());
	computeShader.setVector3("cameraRotation", camera->getRotation());

	glDispatchCompute(1200, 700, 1);
	//glDispatchCompute(std::ceil(float(size / 8.0f)), std::ceil(float(size / 8.0f)), 1);
	glMemoryBarrier(GL_ALL_BARRIER_BITS);
}

void Renderer::updateMeshData(Scene* scene, unsigned int buffer)
{
	scene->checkObjectUpdates(&computeShader, buffer);
}
