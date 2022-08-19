#pragma once

#include "Logger.h"

#include <vector>
#include <string>
#include <filesystem>
#include <iostream>
#include <fstream>
#include <sstream>
#include <glad/glad.h>

namespace FileUtility
{
	std::vector<std::string> getFilesOfTypeInFolder(const char* folder, const char* filetype);

	void saveRender(const std::string& imageName, unsigned int width, unsigned int height, unsigned int pixelBuffer);

	bool isValidInput(std::string& input);

	// Save the current application settings to a save file
	void saveSettings(std::string& loadedSceneName);
	// Read the saved application settings from the save file
	void readSavedSettings(std::string& savedSceneName);
};