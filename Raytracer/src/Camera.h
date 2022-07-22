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
	float getFov();

	// Don't use the current mouse offset, but start from where the cursor is
	void resetMouseOffset();

	// Process the input to the camera
	void processInput(GLFWwindow* window, float deltaTime);

	// Callback for when the mouse is moved
	void mouseCallback(GLFWwindow* window, double xpos, double ypos);

	// Get important information of this camera (position, rotation)
	std::string getInformation();

	float* getCameraSpeedPointer();
	float* getFovPointer();
	float* getSensitivityPointer();

	/* Private members */
private:
	// Camera postion data
	glm::vec3 position = glm::vec3(-2.0f, 2.0f, 0.0f);
	glm::vec3 up;
	glm::vec3 forward;
	float lastx = 400, lasty = 300;
	//float yaw = 150.0f, pitch = -18.0f;
	float yaw = 0.0f, pitch = -90.0f;
	bool firstMouse = true;
	unsigned int ssbo;

	float sensitivity = 1.0f;
	float fov = 40.0f;
	float cameraSpeed = 1.0f;
};