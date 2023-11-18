 #pragma once

//#include "stb_image.h"

#include <iostream>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "shaders/Shader.h"
#include "shaders/ComputeShader.h"
#include "shaders/MultiComputeShader.h"
#include "Scene.h"
#include "SceneManager.h"
#include "Camera.h"
#include "Callbacks.h"
#include "Model.h"
#include "Material.h"
#include "Renderer.h"
#include "HDRIRenderer.h"
#include "ImGuiUserInterface.h"
#include "OutlineRenderer.h"
#include "ObjectScreenSelector.h"
#include "BVH/BVHHandler.h"
#include "InputManager.h"
#include "gui/GizmoRenderer.h"
#include "TextureHandler.h"

#include "WindowUtility.h"

#include "CoordinateUtility.h"
#include "Logger.h"
#include "SceneFileSaver.h"
#include "file_handling/Cache.h"

#include "ApplicationRenderMode.h"
#include "RasterizedDebugMode.h"

class Application
{
public:
	Application(unsigned int WIDTH, unsigned int HEIGHT, bool useShaderCache);
	~Application();

	// Start up the application
	int Start();

private:

	// Making a camera
	Camera camera;

	float cameraSpeed = 0.01f;

	float deltaTime = 0.0f;	// Time between current frame and last frame
	float lastFrame = 0.0f; // Time of last frame

	ApplicationRenderMode currentRenderMode = ApplicationRenderMode::RASTERIZED;
	RasterizedDebugMode currentRasterizedDebugMode = RasterizedDebugMode::REGULAR;

	unsigned int WINDOW_SIZE_X = 1200, WINDOW_SIZE_Y = 700;
	bool useShaderCache;

	// Initialise GLFW
	void initialiseGLFW();

	// Draw the axes given by the VAO using the given shader from the given camera's perspective
	void drawAxes(unsigned int VAO, Shader* shader, Camera* camera);

	// Generate a VAO for the axes
	void generateAxesVAO();

	// Generate a quad that spans the screen (WIP)
	void generateScreenQuad();

	// The objects that hold the screen quad
	unsigned int screenQuadVAO, screenQuadVBO, screenQuadEBO;

	// Holds the vertices required for rendering the axes
	unsigned int axesVAO;

	// Hold the vertices required to render the skybox
	void processInput(GLFWwindow* window);

	const char* saveFileName{ "saved_settings.save" };

	void setupFramebuffer(glm::ivec2 renderedScreenSize);
	void deleteFramebuffer();

	unsigned int framebuffer{ 0 };
	unsigned int screenTexture{ 0 };
	unsigned int depthBuffer{ 0 };
};
