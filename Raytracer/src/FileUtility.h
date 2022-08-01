#pragma once

#include "Logger.h"

#include <vector>
#include <string>
#include <filesystem>

namespace FileUtility
{
	std::vector<std::string> getFilesOfTypeInFolder(const char* folder, const char* filetype);
};