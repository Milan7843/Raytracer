#include "Camera.h"
#include "Scene.h"

Camera::Camera()
{
	up = glm::vec3(0.0f, 1.0f, 0.0f);
	glm::vec3 direction;
	direction.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
	direction.y = sin(glm::radians(pitch));
	direction.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
	forward = glm::normalize(direction);
}
Camera::Camera(glm::vec3 pos)
	: position(pos)
{
	up = glm::vec3(0.0f, 1.0f, 0.0f);
	glm::vec3 direction;
	direction.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
	direction.y = sin(glm::radians(pitch));
	direction.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
	forward = glm::normalize(direction);
}
Camera::Camera(glm::vec3 pos, float yaw, float pitch)
	: position(pos)
	, yaw(yaw)
	, pitch(pitch)
{
	up = glm::vec3(0.0f, 1.0f, 0.0f);
	glm::vec3 direction;
	direction.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
	direction.y = sin(glm::radians(pitch));
	direction.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
	forward = glm::normalize(direction);
}
Camera::Camera(glm::vec3 pos, float yaw, float pitch, float sensitivity, float fov, float cameraSpeed)
	: position(pos)
	, yaw(yaw)
	, pitch(pitch)
	, sensitivity(sensitivity)
	, fov(fov)
	, cameraSpeed(cameraSpeed)
{
	up = glm::vec3(0.0f, 1.0f, 0.0f);
	glm::vec3 direction;
	direction.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
	direction.y = sin(glm::radians(pitch));
	direction.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
	forward = glm::normalize(direction);
}
Camera::~Camera()
{
	Logger::log("Camera destroyed");
}

glm::mat4 Camera::getViewMatrix()
{   //                           cam pos,  target,            up vector
	glm::mat4 view = glm::lookAt(position, position + forward, up);
	return view;
}
glm::mat4 Camera::getProjectionMatrix()
{   //                           cam pos,  target,            up vector
	glm::mat4 projection = glm::perspective(glm::radians(fov), (float)width / (float)height, 0.1f, 1000.0f);
	return projection;
}
glm::mat4 Camera::getProjectionMatrix(unsigned int width, unsigned int height)
{   //                           cam pos,  target,            up vector
	glm::mat4 projection = glm::perspective(glm::radians(fov), (float)width / (float)height, 0.1f, 1000.0f);
	return projection;
}

glm::vec3 Camera::getPosition()
{
	return glm::vec3(position.x, position.y, position.z);
}
glm::vec3 Camera::getRotation()
{
	return glm::vec3(0.0f, glm::radians(yaw), -glm::radians(pitch));
}

float Camera::getFov()
{
	return fov;
}

void Camera::setAspectRatio(int width, int height)
{
	this->width = width;
	this->height = height;
}

void Camera::writeDataToStream(std::ofstream& filestream)
{
	filestream << position.x << " " << position.y << " " << position.z << "\n";
	filestream << pitch << "\n";
	filestream << yaw << "\n";
	filestream << sensitivity << "\n";
	filestream << fov << "\n";
	filestream << cameraSpeed << "\n";
}

std::string Camera::getInformation()
{
	return "Camera position: ("
		+ std::to_string(getPosition().x) + ", "
		+ std::to_string(getPosition().y) + ", "
		+ std::to_string(getPosition().z) + 
		"), pitch: " + std::to_string(pitch)
		+ ", yaw: " + std::to_string(yaw);
}

float* Camera::getCameraSpeedPointer()
{
	return &cameraSpeed;
}

float* Camera::getFovPointer()
{
	return &fov;
}

float* Camera::getSensitivityPointer()
{
	return &sensitivity;
}

bool Camera::processInputFirstPerson(GLFWwindow* window, Scene& scene, double xpos, double ypos, double xoffset, double yoffset, double xscroll, double yscroll, float deltaTime)
{
	// Saving the previous camera position to compare to the new to determine if the camera moved
	glm::vec3 prevPosition = position;

	float realSpeed = cameraSpeed * deltaTime;
	if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
		realSpeed *= 3;

	// FORWARD/BACKWARD
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		position += realSpeed * forward;
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		position -= realSpeed * forward;

	// SIDE/SIDE
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		position -= glm::normalize(glm::cross(forward, up)) * realSpeed;
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		position += glm::normalize(glm::cross(forward, up)) * realSpeed;

	// UP/DOWN
	if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
		position += realSpeed * up;
	if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
		position -= realSpeed * up;

	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);

	// Handling mouse movement

	if (xoffset == 0.0f && yoffset == 0.0f)
	{
		// No change in camera direction
		return false;
	}

	xoffset *= sensitivity * 0.1f;
	yoffset *= sensitivity * 0.1f;

	// Applying rotation
	yaw += xoffset;
	pitch += yoffset;
	if (pitch > 89.0f)
		pitch = 89.0f;
	if (pitch < -89.0f)
		pitch = -89.0f;

	// Setting the new vectors
	forward = calculateDirectionVector(yaw, pitch);

	// The camera moved
	return true;

	// Return that the camera moved if its position is not the same anymore
	//return prevPosition != position;
}

bool Camera::processInputGlobal(GLFWwindow* window, Scene& scene, double xpos, double ypos, double xoffset, double yoffset, double xscroll, double yscroll, float deltaTime)
{
	if (movingToPosition)
	{
		// Moving along the path
		this->position = Maths::cerp<glm::vec3>(
			this->positionMovingFrom,
			this->positionMovingTo,
			this->movingToPositionProgress / this->movingToPositionDuration
		);

		if (this->movingToPositionProgress >= this->movingToPositionDuration)
		{
			// Reached the desired position
			this->position = this->positionMovingTo;
			this->movingToPosition = false;
		}

		// Moving along
		this->movingToPositionProgress += deltaTime;

		return true;
	}
	else
	{
		// Check for the input to move to the selected object position
		if (InputManager::keyPressed(InputManager::InputKey::MOVE_VIEW_TO_SELECTED))
		{
			Object* selectedObject{ scene.getObjectFromSelected() };

			if (selectedObject != nullptr)
			{
				this->positionMovingFrom = this->position;
				
				// Finding an appropriate distance from the object
				float distanceFromObject{ selectedObject->getAppropriateCameraFocusDistance() };

				this->positionMovingTo = selectedObject->getPosition() - this->forward * distanceFromObject;
				this->movingToPositionProgress = 0.0f;
				this->movingToPosition = true;
			}
			return false;
		}
	}

	// Saving the previous camera position to compare to the new to determine if the camera moved
	glm::vec3 prevPosition = position;

	bool shiftHeld{ glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS };

	bool middleMouseHeld{ glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_MIDDLE) == GLFW_PRESS };

	glm::vec3 toRotationPoint{ scene.getRotationPoint() - getPosition() };
	float distanceToRotationPoint{ glm::dot(toRotationPoint, forward) };

	glm::vec3 right{ glm::cross(forward, up) };
	glm::vec3 localUp{ glm::cross(forward, right) };

	xoffset *= sensitivity * 0.1f;
	yoffset *= sensitivity * 0.1f;

	if (middleMouseHeld)
	{
		// Movement
		if (shiftHeld)
		{
			position -= right * (float)xoffset * 0.02f * glm::max(distanceToRotationPoint, 0.1f);
			position += localUp * (float)yoffset * 0.02f * glm::max(distanceToRotationPoint, 0.1f);
		}
		// Rotate around object
		else
		{
			/*
			glm::vec3 projection{ glm::dot(toRotationPoint, forward) * forward };
			glm::vec3 offset{ toRotationPoint - projection };

			std::cout << "offset: " << offset.x << ", " << offset.y << ", " << offset.z << std::endl;
			*/

			// Applying rotation
			float yawBefore{ yaw };
			float pitchBefore{ pitch };
			yaw += xoffset;
			pitch += yoffset;

			if (pitch > 89.0f)
				pitch = 89.0f;
			if (pitch < -89.0f)
				pitch = -89.0f;

			float deltaYaw{ yaw - yawBefore };
			float deltaPitch{ pitch - pitchBefore };

			// Recalculating the forward vector
			forward = calculateDirectionVector(yaw, pitch);

			// Rotate around with offset
			toRotationPoint = glm::rotate(toRotationPoint, glm::radians(deltaPitch), right);
			toRotationPoint = glm::rotate(toRotationPoint, -glm::radians(deltaYaw), glm::vec3(0.0f, 1.0f, 0.0f));

			position = scene.getRotationPoint() - toRotationPoint;
		}
	}

	// Movement by scrolling
	if (shiftHeld)
	{
		fov -= (float)yscroll * sensitivity;

		fov = glm::clamp(fov, 10.0f, 120.0f);
	}
	else
	{
		position += forward * (float)yscroll * sensitivity * 0.2f * glm::max(distanceToRotationPoint, 0.1f);
	}

	// Return that the camera moved if its position is not the same anymore
	return prevPosition != position;
}

void Camera::resetMouseOffset()
{
	firstMouse = true;
}

// Processes the input
bool Camera::processInput(GLFWwindow* window, Scene& scene, double xpos, double ypos, float deltaTime)
{
	// Sets the mouse offset by frame appropriately for the first frame
	if (firstMouse)
	{
		lastx = xpos;
		lasty = ypos;
		firstMouse = false;
	}

	float xoffset = xpos - lastx;
	float yoffset = lasty - ypos; // reversed since y-coordinates range from bottom to top
	lastx = xpos;
	lasty = ypos;

	// Capturing the scroll deltas
	double xscroll{ scrollDeltaX };
	double yscroll{ scrollDeltaY };

	// Resetting the scroll deltas
	scrollDeltaX = 0.0;
	scrollDeltaY = 0.0;

	if (currentMode == MovementMode::GLOBAL)
	{
		return processInputGlobal(window, scene, xpos, ypos, xoffset, yoffset, xscroll, yscroll, deltaTime);
	}
	return processInputFirstPerson(window, scene, xpos, ypos, xoffset, yoffset, xscroll, yscroll, deltaTime);
}

glm::vec3 Camera::calculateDirectionVector(float yaw, float pitch)
{
	glm::vec3 direction;

	// Calculate the direction vector using spherical coordinates
	direction.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
	direction.y = sin(glm::radians(pitch));
	direction.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));

	// Normalize the direction vector
	direction = glm::normalize(direction);

	return direction;
}

void Camera::scrollCallback(double xoffset, double yoffset)
{
	scrollDeltaX += xoffset;
	scrollDeltaY += yoffset;
}
