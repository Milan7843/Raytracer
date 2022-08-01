#include "SceneManager.h"

// For HDRI loading
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

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

void SceneManager::loadAvailableScenesNames()
{
	availableScenesNames = FileUtility::getFilesOfTypeInFolder("scenes", ".scene");
}

std::vector<std::string>& SceneManager::getAvailableScenesNames(bool update)
{
	if (update)
		loadAvailableScenesNames();
	return availableScenesNames;
}

void SceneManager::loadAvailableHDRINames()
{
	availableHDRINames = FileUtility::getFilesOfTypeInFolder("HDRIs", "");
}

std::vector<std::string>& SceneManager::getAvailableHDRINames(bool update)
{
	if (update)
		loadAvailableHDRINames();
	return availableHDRINames;
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
	std::string fileName = "HDRIs/" + imageName;

	unsigned int textureID;
	glGenTextures(1, &textureID);

	int width, height, nrComponents;
	unsigned char* data = stbi_load(fileName.c_str(), &width, &height, &nrComponents, 0);

	if (data)
	{
		GLenum format;
		if (nrComponents == 1)
			format = GL_RED;
		else if (nrComponents == 3)
			format = GL_RGB;
		else if (nrComponents == 4)
			format = GL_RGBA;

		glBindTexture(GL_TEXTURE_2D, textureID);
		glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);

		//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	}
	else
	{
		std::cout << stbi_failure_reason() << std::endl;
		Logger::logError("Failed to load image: " + fileName);
	}

	stbi_image_free(data);

	getCurrentScene().setHDRI(textureID);
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
