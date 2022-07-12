#pragma once

#include <GLFW/glfw3.h>

// Matrix math
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <iostream>
#include <string>

class Camera
{
	/* Public members */
public:
	Camera();
	Camera(glm::vec3 pos);
	~Camera();

	glm::mat4 getViewMatrix();
	glm::mat4 getProjectionMatrix(int width, int height);

	glm::vec3 getPosition();
	glm::vec3 getRotation();

	// Instantiate this camera's pixel buffer
	void instantiatePixelBuffer();
	// Empty this camera's pixel buffer; allows for a new image to be rendered
	void emptyPixelBuffer();

	void processInput(GLFWwindow* window, float deltaTime);

	// Callback for when the mouse is moved
	void mouseCallback(GLFWwindow* window, double xpos, double ypos);

	// Get important information of this camera (position, rotation)
	std::string getInformation();

	/* Private members */
private:
	// Camera postion data
	glm::vec3 position = glm::vec3(-2.0f, 2.0f, 0.0f);
	glm::vec3 up;
	glm::vec3 forward;
	float lastx = 400, lasty = 300;
	float yaw = 150.0f, pitch = -18.0f;
	bool firstMouse = true;
	const float cameraSpeed = 1.0f;
	unsigned int ssbo;
};