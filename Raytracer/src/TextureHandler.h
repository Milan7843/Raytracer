#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <vector>
#include <string>

#include "Logger.h"
#include "ImageLoader.h"

class Material;

namespace TextureHandler
{
	Texture loadTexture(const std::string& spritePath, bool pixelPerfect);
	Texture loadTexture(const char* spritePath, bool pixelPerfect);

	unsigned int packTextures(std::vector<Material>& materials);
};
