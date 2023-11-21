#pragma once

#define GLFW_EXPOSE_NATIVE_WIN32

#include <GLFW/glfw3.h>
#include <GLFW/glfw3native.h>
#include <Windows.h>
#include <filesystem>

#include <string>

namespace WindowUtility
{
	void setWindow(GLFWwindow* window);

	std::string openImageFileChooseDialog();
};

