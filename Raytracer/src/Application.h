#pragma once

#include "stb_image.h"

#include <iostream>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Shader.h"
#include "ComputeShader.h"
#include "Scene.h"
#include "Camera.h"
#include "Callbacks.h"
#include "Model.h"
#include "Material.h"

class Application
{
public:
	Application(unsigned int WIDTH, unsigned int HEIGHT);
	~Application();

	// Start up the application
	int Start();

private:

	// Making a camera
	Camera camera;

	float cameraSpeed = 0.01f;

	float deltaTime = 0.0f;	// Time between current frame and last frame
	float lastFrame = 0.0f; // Time of last frame
	float timeSinceSwitchingModes = 100.0f;

	bool inRaytraceMode = false;

	unsigned int WINDOW_SIZE_X = 1200, WINDOW_SIZE_Y = 700;

	// Initiating GLFW
	void init_glfw();

	// Draws the axes
	void drawAxes(unsigned int VAO, Shader* shader, Camera* camera);

	// Generates a VAO for the axes
	unsigned int generateAxesVAO();

	void processInput(GLFWwindow* window);
};