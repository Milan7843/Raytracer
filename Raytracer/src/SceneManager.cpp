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
	// Empty the array of available scene names
	availableScenesNames.clear();

	// Looping through all files in the scene folder, adding each name
	for (const auto& file : std::filesystem::directory_iterator("/scenes/"))
	{
		availableScenesNames.push_back(file.path().string());
	}
}

std::vector<std::string>& SceneManager::getAvailableScenesNames()
{
	return availableScenesNames;
}

Scene& SceneManager::getCurrentScene()
{
	return currentScene;
}
