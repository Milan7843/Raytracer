#pragma once

#include "Logger.h"

#include <vector>
#include <string>
#include <filesystem>
#include <glad/glad.h>

namespace FileUtility
{
	std::vector<std::string> getFilesOfTypeInFolder(const char* folder, const char* filetype);

	void saveRender(const std::string& imageName, unsigned int width, unsigned int height, unsigned int pixelBuffer);
};