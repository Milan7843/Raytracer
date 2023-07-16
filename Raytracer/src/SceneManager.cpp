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

	// Moving over to the new scene immediately
	currentScene.setName(sceneName);
	hasSceneLoaded = true;
}

void SceneManager::changeScene(const std::string& sceneName)
{
	// Trying to read the file
	bool success{ false };
	Scene loadedScene{ SceneFileSaver::readSceneFromFile(sceneName, &success) };

	if (!success)
	{
		if (!hasSceneLoaded)
		{
			// Loading an empty scene into the current scene slot
			newScene();
		}
		else
		{
			// Do nothing; keep the old scene active
		}

		return;
	}

	// If it could be read, set it as the new scene
	// and set the scene name to the new one
	currentScene = loadedScene;
	hasSceneLoaded = true;
	getCurrentScene().setAspectRatio(width, height);
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

	DirectionalLight directionalLight;
	currentScene.addLight(directionalLight);

	hasSceneLoaded = true;

	getCurrentScene().setAspectRatio(width, height);
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

void SceneManager::setAspectRatio(unsigned int width, unsigned int height)
{
	this->width = width;
	this->height = height;
	getCurrentScene().setAspectRatio(width, height);
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
