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

	// Set the new scene by name
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

private:
	// Keeps the current scene loaded in memory
	Scene currentScene;

	// The scene names in the scene folder
	std::vector<std::string> availableScenesNames;

	// The scene names in the HDRIs folder
	std::vector<std::string> availableHDRINames;

	// Split a string by the delimiter
	std::vector<std::string> split(const std::string& input, char delim);
};
