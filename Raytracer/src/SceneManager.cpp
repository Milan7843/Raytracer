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

void SceneManager::saveChangesAs(std::string& sceneName)
{
	SceneFileSaver::writeSceneToFile(currentScene, sceneName);
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

void SceneManager::newScene()
{
	Scene newScene;
	currentScene = newScene;

	// The new scene must have a camera
	Camera camera;
	currentScene.addCamera(camera);

	// And at least one material
	Material material;
	currentScene.addMaterial(material);
}

void SceneManager::loadAvailableScenesNames()
{
	availableScenesNames = FileUtility::getFilesOfTypeInFolder(scenePath, ".scene");
}

std::vector<std::string>& SceneManager::getAvailableScenesNames(bool update)
{
	if (update)
		loadAvailableScenesNames();
	return availableScenesNames;
}

void SceneManager::loadAvailableHDRINames()
{
	availableHDRINames = FileUtility::getFilesOfTypeInFolder(HDRIsPath, "");
}

std::vector<std::string>& SceneManager::getAvailableHDRINames(bool update)
{
	if (update)
		loadAvailableHDRINames();
	return availableHDRINames;
}

void SceneManager::loadAvailableModelsNames()
{
	availableModelsNames = FileUtility::getFilesOfTypeInFolder(modelsPath, ".obj");
}

std::vector<std::string>& SceneManager::getAvailableModelsNames(bool update)
{
	if (update)
		loadAvailableModelsNames();
	return availableModelsNames;
}

bool SceneManager::willSaveOverwrite(std::string& sceneName)
{
	try
	{
		// Looping through all files in the scene folder, checking each name
		for (const auto& file : std::filesystem::directory_iterator("scenes"))
		{
			// Comparing every file name in the scenes folder
			if (scenePathToSceneName(file.path().string()) == sceneName)
			{
				return true;
			}
		}
	}
	catch (std::filesystem::filesystem_error e)
	{
		Logger::logError(std::string("Error reading scene names: ") + e.what());
	}

	return false;
}

bool SceneManager::containsInvalidSceneNameCharacters(std::string& sceneName)
{
	if (sceneName.find('\\') != std::string::npos)
		return true;
	if (sceneName.find('.') != std::string::npos)
		return true;
	if (sceneName.find('/') != std::string::npos)
		return true;
	return false;
}

Scene& SceneManager::getCurrentScene()
{
	return currentScene;
}

std::string SceneManager::scenePathToSceneName(std::string scenePath)
{
	int preLength = 7; // scenes/
	int aftLength = 6; // .scene

	std::string s{ scenePath.substr(preLength, scenePath.length() - preLength - aftLength)};
	return s;
}

void SceneManager::loadHDRI(const std::string& imageName)
{
	getCurrentScene().loadHDRI(imageName);
}

std::vector<std::string> SceneManager::split(const std::string& input, char delim)
{
	std::vector<std::string> splitted;
	std::string temp;

	// Iterating over every character in the string
	for (int i = 0; i < input.size(); i++)
	{
		char currentChar = input[i];

		// Check for delimiter
		if (currentChar == delim)
		{
			// Must not add empty string on double delim
			if (temp.length() != 0)
			{
				// Adding whatever temp contains to the splitted
				splitted.push_back(temp);
				temp = "";
			}
		}
		else
		{
			// Moving forward
			temp += currentChar;
		}
	}

	return splitted;
}
