#pragma once

#include "Scene.h"
#include "SceneManager.h"
#include "Camera.h"
#include "ComputeShader.h"
#include "Render processes/RenderProcess.h"
#include "Render processes/BlockRenderProcess.h"
#include "Render processes/RealtimeRenderProcess.h"
#include "ImGuiEditorInterface.h"

class Renderer : public ImGuiEditorInterface
{
public:
	Renderer(const char* raytraceComputeShaderPath, unsigned int width, unsigned int height);
	~Renderer();

	// Bind the scene to be rendered
	void bindSceneManager(SceneManager* sceneManager);

	// Render the given scene with the given camera to the given buffer
	void render();

	// Render the scene in blocks (not all at once)
	void startBlockRender();

	// Render the scene using the realtime render process
	void startRealtimeFrameRender();

	// Update this renderer; must be called every frame
	void update(float deltaTime, bool realtimeRaytracing, bool cameraMoved, Scene& currentScene);

	// Check for updates in the mesh data of the scene and update the data in the buffer if necessary
	void updateMeshData(Scene* scene);

	// Set the resolution used to render
	void setResolution(unsigned int width, unsigned int height);

	// Get the resolution used to render
	unsigned int getWidth();
	unsigned int getHeight();

	// Bind the pixel buffer to the GL_SHADER_STORAGE_BUFFER
	void bindPixelBuffer();

	unsigned int getPixelBuffer();

	// Set the sample count
	void setSampleCount(unsigned int sampleCount);

	// Verify the block size: it must be a multiple of 16
	void verifyBlockSize();

	// Draw the interface for this renderer using ImGui
	void drawInterface();

	float getRenderProgress();
	float getTimeLeft();


private:

	// Format a number of seconds
	std::string formatTime(float time);

	// Read all the render settings from the save file
	void readRenderSettings();
	// Save the render settings to the save file
	void writeRenderSettingsToFile();

	// Things to bind before rendering
	SceneManager* sceneManagerBound;

	// Set everything up for rendering with the given scene and camera
	void setUpForRender(Scene& scene, Camera* camera);

	float getRenderProgressPrecise();

	// The compute shader used to render to the buffer
	ComputeShader computeShader;

	// The buffer to store rendered pixel data
	unsigned int pixelBuffer;

	// The resolution used to render
	unsigned int width, height;

	// The sample count used to render (= number of rays per pixel)
	int sampleCount{ 20 };

	// The numbers of render passes per block
	int renderPassCount{ 1 };

	// The number of sample frames already rendered
	unsigned int currentFrameSampleCount{ 0 };

	// The number of sample points per pixel
	int multisamples{ 1 };

	// The size in pixels of each block
	int blockSize{ 64 };

	// The quality of the indirect lighting calculation
	int indirectLightingQuality{ 1 };

	RenderProcess* currentRenderProcess = nullptr;
};

