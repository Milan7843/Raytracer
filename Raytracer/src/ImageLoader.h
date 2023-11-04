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
	int previewWidth;
	int previewHeight;
	float xMin;
	float xMax;
	float yMin;
	float yMax;
	int components;
	std::vector<unsigned char> data;
	unsigned int previewTextureID;

	~Texture()
	{
		// Deleting the preview data on delete
		glDeleteTextures(1, &previewTextureID);
		std::cout << "Destroying texture" << std::endl;
	}
};

namespace ImageLoader
{
	//unsigned int loadImage(std::string imagePath, bool pixelPerfect = false);

	std::shared_ptr<Texture> loadTexture(std::string imagePath, bool pixelPerfect = false);

	unsigned int loadImage(std::string imagePath, bool pixelPerfect = false);

	void copyImageToFixedSize(int width, int height, int newWidth, int newHeight, int nrComponents, const std::vector<unsigned char>& data, std::vector<unsigned char>& newData);
}