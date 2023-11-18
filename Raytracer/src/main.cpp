#pragma once

#include "Application.h"
#include <iostream>
#include <unordered_map>
#include <string>

// Window size
const int WIDTH = 1600, HEIGHT = 900;

int main(int argc, char* argv[])
{
    std::unordered_map<std::string, std::string> args;

    // Loop through each argument and parse them
    for (int i = 1; i < argc; i += 2)
    {
        if (i + 1 < argc)
        {
            // argv[i] is the argument name, and argv[i + 1] is its value
            args[argv[i]] = argv[i + 1];
        }
    }

    bool useShaderCache = true;

    // Check if arguments are present and update values accordingly
    if (args.count("-useShaderCache") > 0)
    {
        useShaderCache = args["-useShaderCache"] == "true";
    }

	Application application(WIDTH, HEIGHT, useShaderCache);
	return application.Start();
}
