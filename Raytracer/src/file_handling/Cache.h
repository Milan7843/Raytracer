#pragma once

#include <glad/glad.h>
#include <string>
#include <fstream>
#include <iostream>
#include "../Logger.h"

namespace Cache
{
	void initialise(bool enabled = true);

	bool cachedFileExists(std::string& path);

	void cacheShader(std::string& binaryPath, GLenum binaryFormat, GLvoid* binaryData, GLint binaryLength);
};

