#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <vector>
#include <string>

#include "Logger.h"
#include "ImageLoader.h"

struct Texture
{
	std::string path;
	bool pixelPerfect;
	unsigned int textureID;
};

namespace TextureHandler
{
	Texture loadTexture(const std::string& spritePath, bool pixelPerfect);
};
