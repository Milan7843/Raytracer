#pragma once

#include "Scene.h"
#include "SceneFileSaver.h"

#include "FileUtility.h"

#include <iostream>
#include <vector>
#include <string>

class SceneManager
{
public:
	SceneManager();
	~SceneManager();

	// Update this scene manager (and the active scene)
	void update();

	// Save the changes to the current scene
	void saveChanges();

	// Save the changes in a new file
	void saveChangesAs(std::string& sceneName);

	// Save the currently loaded scene so it will be loaded on next startup automatically
	void saveSceneLoaded();

	// Change to another scene by name, will not overwrite current with empty scene on fail to load
	void changeScene(const std::string& sceneName);

	// Revert all changes to the current scene by reading it again
	void revertChanges();

	// Create a new and empty scene
	void newScene();

	// Load all the names of the available scenes
	void loadAvailableScenesNames();
	// Get all the names of the available scenes
	std::vector<std::string>& getAvailableScenesNames(bool update);

	// Load all the names of the available HDRI's
	void loadAvailableHDRINames();
	// Get all the names of the available HDRI's
	std::vector<std::string>& getAvailableHDRINames(bool update);

	// Load all the names of the available Models
	void loadAvailableModelsNames();
	// Get all the names of the available Models
	std::vector<std::string>& getAvailableModelsNames(bool update);

	// Get whether saving a scene under this name will overwrite a scene
	bool willSaveOverwrite(std::string& sceneName);

	// Get whether this scene name contains any invalid characters ('\', '.', '/')
	bool containsInvalidSceneNameCharacters(std::string& sceneName);

	// Get the current scenes
	Scene& getCurrentScene();

	// Extract the scene name from the path
	std::string scenePathToSceneName(std::string scenePath);

	// Set the HDRI in the currently active scene
	void loadHDRI(const std::string& imageName);

	bool hasUnsavedChanges();

private:
	// Keeps the current scene loaded in memory
	Scene currentScene;

	// Indicates whether a working scene has been loaded
	bool hasSceneLoaded{ false };
	// Indicates whether the user is working in an unnamed/unsaved scene
	bool inUnnamedScene{ true };

	// The scene names in the scene folder
	std::vector<std::string> availableScenesNames;

	// The scene names in the HDRIs folder
	std::vector<std::string> availableHDRINames;

	// The scene names in the models folder
	std::vector<std::string> availableModelsNames;

	// Split a string by the delimiter
	std::vector<std::string> split(const std::string& input, char delim);

	// Rendering size: only used for aspect ratio
	unsigned int width{ 0 };
	unsigned int height{ 0 };

	// PATHS
	const char* scenePath{ "scenes" };
	const char* HDRIsPath{ "HDRIs" };
	const char* modelsPath{ "src/models" };
};
