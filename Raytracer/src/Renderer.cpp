#include "Renderer.h"

Renderer::Renderer(const char* raytraceComputeShaderPath, unsigned int width, unsigned int height)
	: computeShader(raytraceComputeShaderPath), width(width), height(height)
{
	// Immediately creating the pixel buffer with the given width and height
	setResolution(width, height);
}

Renderer::~Renderer()
{
}

void Renderer::render(Scene* scene, Camera* camera)
{
	computeShader.use();
	/*
	if (scene->hasPendingChanges())
	{
		scene->writeLightsToShader(&computeShader);
		scene->writeMaterialsToShader(&computeShader);
	}*/
	computeShader.setVector3("cameraPosition", camera->getPosition());
	computeShader.setVector3("cameraRotation", camera->getRotation());

	computeShader.setInt("screenWidth", width);

	bindPixelBuffer();

	// Running the compute shader once for each pixel
	glDispatchCompute(width, height, 1);
	//glDispatchCompute(std::ceil(float(size / 8.0f)), std::ceil(float(size / 8.0f)), 1);
	glMemoryBarrier(GL_ALL_BARRIER_BITS);

	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
}

void Renderer::updateMeshData(Scene* scene)
{
	scene->checkObjectUpdates(&computeShader, pixelBuffer);
}

void Renderer::setResolution(unsigned int width, unsigned int height)
{
	// Deleting the previous pixel buffer
	glDeleteBuffers(1, &pixelBuffer);

	this->width = width;
	this->height = height;

	// Creating the pixel array buffer
	pixelBuffer = 0;
	glGenBuffers(1, &pixelBuffer);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, pixelBuffer);

	// Setting buffer size to be width x height x 16, for 16 bytes per pixel
	glBufferData(GL_SHADER_STORAGE_BUFFER, width * height * 16, 0, GL_DYNAMIC_COPY);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 3, pixelBuffer);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
}

void Renderer::bindPixelBuffer()
{
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, pixelBuffer);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 3, pixelBuffer);
}
