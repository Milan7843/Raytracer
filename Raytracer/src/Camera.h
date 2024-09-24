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
#include "Maths.h"

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

	void updateVectors();

	glm::mat4 getViewMatrix() const;
	glm::mat4 getProjectionMatrix() const;
	glm::mat4 getProjectionMatrix(unsigned int width, unsigned int height);

	glm::vec3 getPosition() const;
	glm::vec3 getRotation() const;
	float getFov() const;

	// Don't use the current mouse offset, but start from where the cursor is
	void resetMouseOffset();

	// Process the input to the camera.
	// Returns whether the camera moved.
	bool processInput(Scene& scene, float deltaTime);

	float getYaw() const;
	float getPitch() const;

	void setPosition(glm::vec3 position);
	void setPitch(float newPitch);
	void setYaw(float newYaw);
	void setFov(float newFov);

	// Get the ratio width/height
	float getAspectRatio() const;

	// Get important information of this camera (position, rotation)
	std::string getInformation();

	void onScroll(float delta);

	float* getCameraSpeedPointer();
	float* getFovPointer();
	float* getSensitivityPointer();

	/* Private members */
private:

	// Different movement modes
	bool processInputFirstPerson(Scene& scene, double xoffset, double yoffset, float scroll, float deltaTime);
	bool processInputGlobal(Scene& scene, double xoffset, double yoffset, float scroll, float deltaTime);

	glm::vec3 calculateDirectionVector(float yaw, float pitch);

	// Camera postion data
	glm::vec3 position = glm::vec3(4.0f, 3.5f, 1.5f);
	glm::vec3 up;
	glm::vec3 forward;
	float lastx = 400, lasty = 300;
	float yaw{ 200.0f }, pitch{ -40.0f };
	bool firstMouse = true;
	unsigned int ssbo;

	float nearPlane{ 0.1f };
	float farPlane{ 1000.0f };

	float sensitivity = 1.0f;
	float fov = 40.0f;
	float cameraSpeed = 1.0f;

	float scroll{ 0.0f };

	
	bool movingToPosition{ false };
	float movingToPositionProgress{ 0.0f };
	glm::vec3 positionMovingFrom{ glm::vec3(0.0f) };
	glm::vec3 positionMovingTo{ glm::vec3(0.0f) };
	
	float movingToPositionDuration{ 0.2f };
	
	MovementMode currentMode{ MovementMode::GLOBAL };
};