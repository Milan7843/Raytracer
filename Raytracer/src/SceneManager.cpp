#include "SceneManager.h"

SceneManager::SceneManager()
{
}

SceneManager::~SceneManager()
{
}

void SceneManager::update()
{

}

void SceneManager::saveChanges()
{
	SceneFileSaver::writeSceneToFile(currentScene, *currentScene.getNamePointer());
}

void SceneManager::changeScene(const std::string& sceneName)
{
	try
	{
		// Trying to read the file
		Scene newScene{ SceneFileSaver::readSceneFromFile(sceneName) };

		// If it could be read, set it as the new scene
		// and set the scene name to the new one
		currentScene = newScene;
	}
	catch (const char* e)
	{
		Logger::logError(e);
		// If it cannot be read: reload available scene names; should be done automatically on opening scene choose menu ?
		// maybe it was removed?
		//loadAvailableScenesNames();
	}
}

void SceneManager::revertChanges()
{
	// Just change the scene to the one we are already in
	changeScene(*currentScene.getNamePointer());
}

void SceneManager::loadAvailableScenesNames()
{
	try
	{
		// Empty the array of available scene names
		availableScenesNames.clear();

		// Looping through all files in the scene folder, adding each name
		for (const auto& file : std::filesystem::directory_iterator("scenes"))
		{
			// The file must end with .scene; if it does, add it to the list
			if (file.path().string().ends_with(".scene"))
				availableScenesNames.push_back(file.path().string());

			else // Throw an error if an unknown filetype was found in the scenes folder
				Logger::logError("Error: unknown file found in /scenes folder: " + file.path().string());
		}
	}
	catch (std::filesystem::filesystem_error e)
	{
		Logger::logError(std::string("Error reading scene names: ") + e.what());
	}
	/*
	catch (const char* e)
	{
		// Printing the error
		std::cout << e << std::endl;
	}*/
}

std::vector<std::string>& SceneManager::getAvailableScenesNames()
{
	loadAvailableScenesNames();
	return availableScenesNames;
}

Scene& SceneManager::getCurrentScene()
{
	return currentScene;
}
