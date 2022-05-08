#include "Camera.h"

Camera::Camera()
{
	position = glm::vec3(0.0f, 0.0f, 0.0f);
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
Camera::~Camera()
{
	std::cout << "Camera destroyed" << std::endl;
}

glm::mat4 Camera::getViewMatrix()
{   //                           cam pos,  target,            up vector
	glm::mat4 view = glm::lookAt(position, position + forward, up);
	return view;
}
glm::mat4 Camera::getProjectionMatrix(int width, int height)
{   //                           cam pos,  target,            up vector
	glm::mat4 projection = glm::perspective(glm::radians(40.0f), (float)width / (float)height, 0.1f, 100.0f);
	return projection;
}

glm::vec3 Camera::getPosition()
{
	return glm::vec3(position.z, position.y, position.x);
}
glm::vec3 Camera::getRotation()
{
	return glm::vec3(0.0f, glm::radians(yaw), -glm::radians(pitch));
}


void Camera::mouseCallback(GLFWwindow* window, double xpos, double ypos)
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

	const float sensitivity = 0.1f;
	xoffset *= sensitivity;
	yoffset *= sensitivity;

	// Applying rotation
	yaw += xoffset;
	pitch += yoffset;
	if (pitch > 89.0f)
		pitch = 89.0f;
	if (pitch < -89.0f)
		pitch = -89.0f;

	// Setting the new vectors
	glm::vec3 direction;
	direction.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
	direction.y = sin(glm::radians(pitch));
	direction.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
	forward = glm::normalize(direction);
}

void Camera::instantiatePixelBuffer()
{
	// Creating the pixel array buffer
	ssbo = 0;
	glGenBuffers(1, &ssbo);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssbo);
	glBufferData(GL_SHADER_STORAGE_BUFFER, 1200 * 700 * 16, 0, GL_DYNAMIC_COPY);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 3, ssbo);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
}

void Camera::emptyPixelBuffer()
{
	// Clearing the pixel array buffer
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssbo);
	glClearBufferData(GL_SHADER_STORAGE_BUFFER, GL_RGBA32F, GL_RGBA, GL_FLOAT, (void*)0);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
}

// Processes the input
void Camera::processInput(GLFWwindow* window, float deltaTime)
{
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
}