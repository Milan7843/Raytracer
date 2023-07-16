#pragma once

#include <GLFW/glfw3.h>

// Matrix math
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/rotate_vector.hpp>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <iostream>
#include <string>
#include <fstream>
#include <sstream>

#include "Logger.h"

enum class MovementMode
{
	FIRST_PERSON,
	GLOBAL
};

class Scene;

class Camera
{
	/* Public members */
public:
	Camera();
	Camera(glm::vec3 pos);
	Camera(glm::vec3 pos, float yaw, float pitch);
	Camera(glm::vec3 pos, float yaw, float pitch, float sensitivity, float fov, float cameraSpeed);
	~Camera();

	glm::mat4 getViewMatrix();
	glm::mat4 getProjectionMatrix();
	glm::mat4 getProjectionMatrix(unsigned int width, unsigned int height);

	glm::vec3 getPosition();
	glm::vec3 getRotation();
	float getFov();

	// Don't use the current mouse offset, but start from where the cursor is
	void resetMouseOffset();

	// Process the input to the camera.
	// Returns whether the camera moved.
	bool processInput(GLFWwindow* window, Scene& scene, double xpos, double ypos, float deltaTime);

	void setAspectRatio(int width, int height);

	// Write this light to the given filestream
	virtual void writeDataToStream(std::ofstream& filestream);

	// Get important information of this camera (position, rotation)
	std::string getInformation();

	float* getCameraSpeedPointer();
	float* getFovPointer();
	float* getSensitivityPointer();

	void scrollCallback(double xoffset, double yoffset);

	/* Private members */
private:

	// Different movement modes
	bool processInputFirstPerson(GLFWwindow* window, Scene& scene, double xpos, double ypos, double xoffset, double yoffset, double xscroll, double yscroll, float deltaTime);
	bool processInputGlobal(GLFWwindow* window, Scene& scene, double xpos, double ypos, double xoffset, double yoffset, double xscroll, double yscroll, float deltaTime);

	glm::vec3 calculateDirectionVector(float yaw, float pitch);

	// Camera postion data
	glm::vec3 position = glm::vec3(4.0f, 3.5f, 1.5f);
	glm::vec3 up;
	glm::vec3 forward;
	float lastx = 400, lasty = 300;
	float yaw{ 200.0f }, pitch{ -40.0f };
	bool firstMouse = true;
	unsigned int ssbo;

	// Rendering size: only used for aspect ratio
	unsigned int width{ 0 };
	unsigned int height{ 0 };

	float sensitivity = 1.0f;
	float fov = 40.0f;
	float cameraSpeed = 1.0f;

	double scrollDeltaX{ 0.0 };
	double scrollDeltaY{ 0.0 };

	MovementMode currentMode{ MovementMode::GLOBAL };
};