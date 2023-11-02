#pragma once

#include <string>
#include <vector>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>

#include "../Logger.h"


namespace ImageLoader
{
	unsigned int loadImage(std::string imagePath, bool pixelPerfect = false);
}