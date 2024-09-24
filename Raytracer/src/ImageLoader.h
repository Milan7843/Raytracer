#pragma once

#include <string>
#include <vector>
#include <memory>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>

#include "../Logger.h"
#include "shaders/ComputeShader.h"


struct AtlasTexture
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

	~AtlasTexture()
	{
		// Deleting the preview data on delete
		glDeleteTextures(1, &previewTextureID);
		Logger::logDebug("Destroying atlas texture");
	}
};

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
	unsigned int textureID;

	~Texture()
	{
		// Deleting the preview data on delete
		glDeleteTextures(1, &previewTextureID);
		glDeleteTextures(1, &textureID);
		Logger::logDebug("Destroying texture");
	}
};

struct HDRITexture
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
	std::vector<float> data;
	unsigned int previewTextureID;
	unsigned int textureID;
	unsigned int blurredTextureID;

	~HDRITexture()
	{
		// Deleting the preview data on delete
		glDeleteTextures(1, &previewTextureID);
		glDeleteTextures(1, &textureID);
		glDeleteTextures(1, &blurredTextureID);
		Logger::logDebug("Destroying HDRI texture.");
	}
};

namespace ImageLoader
{
	void initialise();

	//unsigned int loadImage(std::string imagePath, bool pixelPerfect = false);

	std::shared_ptr<AtlasTexture> loadAtlasTexture(std::string imagePath, bool pixelPerfect = false, float previewAspectRatio = 1.0f);
	std::shared_ptr<Texture> loadTexture(std::string imagePath, bool pixelPerfect = false, float previewAspectRatio = 1.0f);
	std::shared_ptr<HDRITexture> loadHDRITexture(std::string imagePath, bool pixelPerfect = false, float previewAspectRatio = 1.0f);
	//std::shared_ptr<Texture> loadTexture(std::string imagePath, bool pixelPerfect = false);

	unsigned int loadImage(std::string imagePath, bool pixelPerfect = false);

	void copyImageToFixedSize(int width, int height, int newWidth, int newHeight, int nrComponents, const std::vector<float>& data, std::vector<float>& newData);
	void copyImageToFixedSize(int width, int height, int newWidth, int newHeight, int nrComponents, const std::vector<unsigned char>& data, std::vector<unsigned char>& newData);
}