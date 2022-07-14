#pragma once

#include "Scene.h"
#include "Camera.h"
#include "ComputeShader.h"

class Renderer
{
public:
	Renderer(const char* raytraceComputeShaderPath, unsigned int width, unsigned int height, Scene* scene);
	~Renderer();

	// Render the given scene with the given camera to the given buffer
	void render(Scene* scene, Camera* camera);

	// Check for updates in the mesh data of the scene and update the data in the buffer if necessary
	void updateMeshData(Scene* scene);

	// Set the resolution used to render
	void setResolution(unsigned int width, unsigned int height);

	// Bind the pixel buffer to the GL_SHADER_STORAGE_BUFFER
	void bindPixelBuffer();

	// Set the sample count
	void setSampleCount(unsigned int sampleCount);

private:
	// The compute shader used to render to the buffer
	ComputeShader computeShader;

	// The buffer to store rendered pixel data
	unsigned int pixelBuffer;

	// The resolution used to render
	unsigned int width, height;

	// The sample count used to render (= number of rays per pixel)
	unsigned int sampleCount = 1;

	// The number of sample frames already rendered
	unsigned int currentFrameSampleCount = 0;
};

