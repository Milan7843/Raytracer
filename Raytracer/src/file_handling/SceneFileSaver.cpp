#include "SceneFileSaver.h"

void SceneFileSaver::writeSceneToFile(Scene& scene, const std::string& fileName)
{
	// The data stream into the file
	std::ofstream filestream{ "scenes/" + fileName + ".json" };

	// Writing all scene data
	nlohmann::json data{ JSONUtility::toJSON(scene) };

	filestream << data;
	filestream.close();
}

Scene SceneFileSaver::readSceneFromFile(const std::string& fileName, bool* success)
{
	// Opening the scene file into a stream
	std::ifstream filestream{ "scenes/" + fileName + ".json" };

	// Creating an empty scene
	Scene scene{};

	// Throw an error and return an empty scene if the file could not be opened
	if (!filestream)
	{
		Logger::logError("Scene " + fileName + " could not be opened. "
			"Make sure it is in the scene directory and ends in '.json'.");

		*success = false;
		return scene;
	}

	json data = json::parse(filestream);

	scene.setName(fileName);

	// Setting wireframe mode


	JSONUtility::loadModels(data, scene);
	JSONUtility::loadMaterials(data, scene);
	JSONUtility::loadSpheres(data, scene);
	JSONUtility::loadPointLights(data, scene);
	JSONUtility::loadDirectionalLights(data, scene);
	JSONUtility::loadAmbientLights(data, scene);
	JSONUtility::loadCamera(data, scene);
	JSONUtility::loadHDRI(data, scene);
	JSONUtility::loadSettings(data, scene);

	// Finally closing the file
	filestream.close();

	// Generating a buffer big enough for all triangles to go into
	scene.generateTriangleBuffer();

	*success = true;

	return scene;
}

void SceneFileSaver::writeSettingsToFile(std::string& loadedScene)
{
	// The data stream into the file
	std::ofstream filestream{ "settings.json" };

	// Writing all settings data
	json data;

	data["previousscene"] = loadedScene;

	filestream << data;
	filestream.close();
}

SavedSettings SceneFileSaver::readSavedSettings()
{
	// Opening the scene file into a stream
	std::ifstream filestream{ "settings.json" };

	SavedSettings settings;

	// Return empty settings if the file could not be read
	if (!filestream)
	{
		return settings;
	}

	json data = json::parse(filestream);

	if (data.contains("previousscene"))
	{
		settings.previouslyLoadedScene = data["previousscene"];
	}

	// Finally closing the file
	filestream.close();

	return settings;
}
