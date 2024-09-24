#pragma once

#include <string>
#include <iostream>
#include <iomanip>
#include <glm/glm.hpp>

// For colored text
#define NOMINMAX
#include <Windows.h>
#include "WinCon.h"

namespace Logger
{
	// Enable or disable the debug output
	void setDebug(bool enabled);

	// Regularly log the given message
	void log(std::string message);

	// Regularly log the given message
	void log(const char* message);

	// Log the given message as a warning
	void logWarning(std::string message);

	// Log the given message as a warning
	void logWarning(const char* message);

	// Log the given message as a debug message
	void logDebug(std::string message);

	// Log the given message as a debug message
	void logDebug(const char* message);

	// Log the given message as an error
	void logError(std::string message);

	// Log the given message as an error
	void logError(const char* message);

	void write(const char* message);

	// Stop this logger
	void stop();

	// Log a matrix with the given message
	void logMatrix(glm::mat4& matrix, const char* message);
}