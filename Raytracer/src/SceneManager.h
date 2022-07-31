#pragma once

#include "Scene.h"
#include "SceneFileSaver.h"

#include <iostream>
#include <vector>
#include <string>
#include <filesystem>

class SceneManager
{
public:
	SceneManager();
	~SceneManager();

	// Update this scene manager (and the active scene)
	void update();

	// Save the changes to the current scene
	void saveChanges();

	// Set the new scene by name
	void changeScene(std::string& sceneName);

	// Revert all changes to the current scene by reading it again
	void revertChanges();

	// Load all the names of the available scenes
	void loadAvailableScenesNames();

	// Get all the names of the available scenes
	std::vector<std::string>& getAvailableScenesNames();

	// Get the current scenes
	Scene& getCurrentScene();

private:
	// Keeps the current scene loaded in memory
	Scene currentScene;

	// The scene names in the scene folder
	std::vector<std::string> availableScenesNames;
};
