#include "Camera.h"
#include "Scene.h"

Camera::Camera()
{
	updateVectors();
}
Camera::Camera(glm::vec3 pos)
	: position(pos)
{
	updateVectors();
}
Camera::Camera(glm::vec3 pos, float yaw, float pitch)
	: position(pos)
	, yaw(yaw)
	, pitch(pitch)
{
	updateVectors();
}
Camera::Camera(glm::vec3 pos, float yaw, float pitch, float sensitivity, float fov, float cameraSpeed)
	: position(pos)
	, yaw(yaw)
	, pitch(pitch)
	, sensitivity(sensitivity)
	, fov(fov)
	, cameraSpeed(cameraSpeed)
{
	updateVectors();
}
Camera::~Camera()
{
}

void Camera::updateVectors()
{
	up = glm::vec3(0.0f, 1.0f, 0.0f);
	glm::vec3 direction;
	direction.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
	direction.y = sin(glm::radians(pitch));
	direction.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
	forward = glm::normalize(direction);
}

glm::mat4 Camera::getViewMatrix() const
{   //                           cam pos,  target,            up vector
	glm::mat4 view = glm::lookAt(position, position + forward, up);
	return view;
}
glm::mat4 Camera::getProjectionMatrix() const
{
	if (WindowUtility::getRenderWindowSize().x == 0 || WindowUtility::getRenderWindowSize().y == 0)
		return glm::mat4(1.0f);
	glm::mat4 projection = glm::perspective(glm::radians(fov), (float)WindowUtility::getRenderWindowSize().x / (float)WindowUtility::getRenderWindowSize().y, nearPlane, farPlane);
	return projection;
}
glm::mat4 Camera::getProjectionMatrix(unsigned int width, unsigned int height)
{   //                           cam pos,  target,            up vector
	glm::mat4 projection = glm::perspective(glm::radians(fov), (float)width / (float)height, 0.1f, 1000.0f);
	return projection;
}

glm::vec3 Camera::getPosition() const
{
	return glm::vec3(position.x, position.y, position.z);
}
glm::vec3 Camera::getRotation() const
{
	return glm::vec3(0.0f, glm::radians(yaw), -glm::radians(pitch));
}

float Camera::getFov() const
{
	return fov;
}

float Camera::getYaw() const
{
	return yaw;
}

float Camera::getPitch() const
{
	return pitch;
}

void Camera::setPosition(glm::vec3 position)
{
	this->position = position;
	updateVectors();
}

void Camera::setPitch(float newPitch)
{
	this->pitch = newPitch;
	updateVectors();
}

void Camera::setYaw(float newYaw)
{
	this->yaw = newYaw;
	updateVectors();
}

void Camera::setFov(float newFov)
{
	this->fov = newFov;
	updateVectors();
}

float Camera::getAspectRatio() const
{
	return (float)WindowUtility::getRenderWindowSize().x / (float)WindowUtility::getRenderWindowSize().y;
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

void Camera::onScroll(float delta)
{
	scroll += delta;
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

bool Camera::processInputFirstPerson(Scene& scene, double xoffset, double yoffset, float scroll, float deltaTime)
{
	// Saving the previous camera position to compare to the new to determine if the camera moved
	glm::vec3 prevPosition = position;

	float realSpeed = cameraSpeed * deltaTime;
	if (InputManager::getModifierState() == InputManager::Modifier::SHIFT)
		realSpeed *= 3;

	// FORWARD/BACKWARD
	if (InputManager::keyHeld(InputManager::InputAction::MOVE_FORWARD))
		position += realSpeed * forward;
	if (InputManager::keyHeld(InputManager::InputAction::MOVE_BACKWARD))
		position -= realSpeed * forward;

	// SIDE/SIDE
	if (InputManager::keyHeld(InputManager::InputAction::MOVE_LEFT))
		position -= glm::normalize(glm::cross(forward, up)) * realSpeed;
	if (InputManager::keyHeld(InputManager::InputAction::MOVE_RIGHT))
		position += glm::normalize(glm::cross(forward, up)) * realSpeed;

	// UP/DOWN
	if (InputManager::keyHeld(InputManager::InputAction::MOVE_UP))
		position += realSpeed * up;
	if (InputManager::keyHeld(InputManager::InputAction::MOVE_DOWN))
		position -= realSpeed * up;

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

bool Camera::processInputGlobal(Scene& scene, double xoffset, double yoffset, float scroll, float deltaTime)
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
		if (InputManager::keyPressed(InputManager::InputAction::MOVE_VIEW_TO_SELECTED))
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

	bool shiftHeld{ InputManager::getModifierState() == InputManager::Modifier::SHIFT };

	bool middleMouseHeld{ InputManager::keyHeld(InputManager::InputAction::MOVE_VIEW) };

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
		fov -= scroll * sensitivity;

		fov = glm::clamp(fov, 10.0f, 120.0f);
	}
	else
	{
		position += forward * scroll * sensitivity * 0.2f * glm::max(distanceToRotationPoint, 0.1f);
	}

	// Return that the camera moved if its position is not the same anymore
	return prevPosition != position;
}

void Camera::resetMouseOffset()
{
	firstMouse = true;
}


// Processes the input
bool Camera::processInput(Scene& scene, float deltaTime)
{
	glm::vec2 mousePos = InputManager::getMousePosition();

	// Sets the mouse offset by frame appropriately for the first frame
	if (firstMouse)
	{
		lastx = mousePos.x;
		lasty = mousePos.y;
		firstMouse = false;
	}

	float xoffset = mousePos.x - lastx;
	float yoffset = lasty - mousePos.y; // reversed since y-coordinates range from bottom to top
	lastx = mousePos.x;
	lasty = mousePos.y;

	// Capturing the scroll deltas
	double scrollDelta{ scroll };

	// Resetting the scroll deltas
	scroll = 0.0;

	glm::vec3 positionBefore = position;
	float yawBefore = yaw;
	float pitchBefore = pitch;

	bool moved = false;

	if (currentMode == MovementMode::GLOBAL)
	{
		moved = processInputGlobal(scene, xoffset, yoffset, scrollDelta, deltaTime);
	}
	else if (currentMode == MovementMode::FIRST_PERSON)
	{
		moved = processInputFirstPerson(scene, xoffset, yoffset, scrollDelta, deltaTime);
	}

	glm::vec3 positionDelta = position - positionBefore;

	float deltaYaw = yaw - yawBefore;
	float deltaPitch = pitch - pitchBefore;

	return moved;
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