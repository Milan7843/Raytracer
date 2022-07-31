#pragma once

#include "Scene.h"
#include "Camera.h"
#include "ComputeShader.h"

class Renderer
{
public:
	Renderer(const char* raytraceComputeShaderPath, unsigned int width, unsigned int height);
	~Renderer();

	// Bind the camera to be rendered with
	void bindCamera(Camera* camera);

	// Bind the scene to be rendered
	void bindScene(Scene* scene);

	// Render the given scene with the given camera to the given buffer
	void render();

	// Render the scene in blocks (not all at once)
	void startBlockRender();

	void blockRenderStep();

	// Update this renderer; must be called every frame
	void update(float deltaTime);

	// Check for updates in the mesh data of the scene and update the data in the buffer if necessary
	void updateMeshData(Scene* scene);

	// Set the resolution used to render
	void setResolution(unsigned int width, unsigned int height);

	// Bind the pixel buffer to the GL_SHADER_STORAGE_BUFFER
	void bindPixelBuffer();

	// Set the sample count
	void setSampleCount(unsigned int sampleCount);

	int* getBlockSizePointer();
	int* getMultisamplePointer();
	int* getSampleCountPointer();
	int* getRenderPassCountPointer();
	float getRenderProgress();
	float getTimeLeft();


private:

	// Things to bind before rendering
	Camera* cameraBound;
	Scene* sceneBound;

	// Set everything up for rendering with the given scene and camera
	void setUpForRender(Scene* scene, Camera* camera);

	float getRenderProgressPrecise();

	// Get the top-left coordinate of the currently rendering block
	glm::vec2 getBlockOrigin();

	// The compute shader used to render to the buffer
	ComputeShader computeShader;

	// The buffer to store rendered pixel data
	unsigned int pixelBuffer;

	// The resolution used to render
	unsigned int width, height;

	// The sample count used to render (= number of rays per pixel)
	int sampleCount = 20;

	// The numbers of render passes per block
	int renderPassCount = 1;

	// The number of sample frames already rendered
	unsigned int currentFrameSampleCount = 0;

	// The number of sample points per pixel
	int multisamples = 1;

	/* Block rendering */
	// The size in pixels of each block
	int blockSize = 100;
	int blockSizeRendering = 100;
	bool currentlyBlockRendering = false;
	unsigned int blockIndexX = 0, blockIndexY = 0;

	float currentRenderTime = 0.0f;
	int currentBlockRenderPassIndex = 0;
};

