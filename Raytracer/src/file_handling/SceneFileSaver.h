#pragma once

#include "Scene.h"
#include <iostream>
#include <fstream>
#include <sstream>

#include "JSONUtility.h"

struct SavedSettings
{
	std::string previouslyLoadedScene;
};

namespace SceneFileSaver
{
	// Write the given scene into a file with the given name
	void writeSceneToFile(Scene& scene, const std::string& fileName);

	// Read the scene with the given name from the file
	Scene readSceneFromFile(const std::string& fileName, bool* success);

	void writeSettingsToFile(std::string& loadedScene);

	SavedSettings readSavedSettings();
}
