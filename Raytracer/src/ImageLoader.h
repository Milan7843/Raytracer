#pragma once

#include <string>
#include <vector>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>

#include "../Logger.h"


struct Texture
{
	std::string path;
	bool pixelPerfect;
	int width;
	int height;
	float xMin;
	float xMax;
	float yMin;
	float yMax;
	std::vector<unsigned char> data;
};

namespace ImageLoader
{
	//unsigned int loadImage(std::string imagePath, bool pixelPerfect = false);

	Texture loadTexture(std::string imagePath, bool pixelPerfect = false);

	unsigned int loadImage(std::string imagePath, bool pixelPerfect = false);
}