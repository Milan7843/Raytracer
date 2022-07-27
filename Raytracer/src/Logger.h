#pragma once

#include <string>
#include <iostream>

// For colored text
#define NOMINMAX
#include <Windows.h>
#include "WinCon.h"

namespace Logger
{
	// Regularly log the given message
	void log(std::string message);

	// Regularly log the given message
	void log(const char* message);

	// Log the given message as a warning
	void logWarning(std::string message);

	// Log the given message as a warning
	void logWarning(const char* message);

	// Log the given message as an error
	void logError(std::string message);

	// Log the given message as an error
	void logError(const char* message);

	void write(const char* message);
}