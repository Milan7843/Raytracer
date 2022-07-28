#include "SceneFileSaver.h"



void SceneFileSaver::writeSceneToFile(Scene& scene, const std::string& fileName)
{
	// This function should use a to string function from every object that should be saved
	// Then read in the same format later

	// The data stream into the file
	std::ofstream filestream { "scenes/" + fileName};

	// Writing all scene data
	scene.writeDataToStream(filestream);

	filestream.close();
}
