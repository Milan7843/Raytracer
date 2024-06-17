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
	std::shared_ptr<AtlasTexture> loadAtlasTexture(const std::string& spritePath, bool pixelPerfect, float previewAspectRatio = 1.0f);
	std::shared_ptr<AtlasTexture> loadAtlasTexture(const char* spritePath, bool pixelPerfect, float previewAspectRatio = 1.0f);

	std::shared_ptr<Texture> loadTexture(const std::string& spritePath, bool pixelPerfect, float previewAspectRatio = 1.0f);
	std::shared_ptr<Texture> loadTexture(const char* spritePath, bool pixelPerfect, float previewAspectRatio = 1.0f);

	std::shared_ptr<HDRITexture> loadHDRITexture(const std::string& spritePath, bool pixelPerfect, float previewAspectRatio = 1.0f);
	std::shared_ptr<HDRITexture> loadHDRITexture(const char* spritePath, bool pixelPerfect, float previewAspectRatio = 1.0f);

	unsigned int packTextures(std::vector<Material>& materials);

	void textureRemoved();
};
