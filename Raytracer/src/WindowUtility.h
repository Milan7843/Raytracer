#pragma once

#define GLFW_EXPOSE_NATIVE_WIN32

#include <GLFW/glfw3.h>
#include <GLFW/glfw3native.h>
#include <Windows.h>
#include <filesystem>
#include <glm/glm.hpp>

#include <string>

namespace WindowUtility
{
	// Set the window being worked on
	void setWindow(GLFWwindow* window);

	// Set the current title of the window
	void setWindowTitle(const std::string& title);

	// Set the current title of the window to reflect the fact there the user loaded a new scene
	void setWindowTitleNewScene();

	// Mark the window title with the unsaved changes symbol
    void markUnsavedChanges();

	// Unmark the window title with the unsaved changes symbol
	void markSavedChanges();

	void setNewWindowSize(unsigned int width, unsigned int height);

	glm::ivec2 getWindowSize();

	void setNewRenderWindowSize(unsigned int width, unsigned int height);

	glm::ivec2 getRenderWindowSize();

	// Open a Windows dialogue that lets the user choose an image file
	// Returns the path of the chosen image
	std::string openImageFileChooseDialog();
};

