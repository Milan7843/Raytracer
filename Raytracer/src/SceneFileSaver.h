#pragma once

#include "Scene.h"

namespace SceneFileSaver
{
	// Write the given scene into a file with the given name
	void writeSceneToFile(Scene& scene, const std::string& fileName);

	// Read the scene with the given name from the file
	Scene& readSceneFromFile(const std::string& fileName);
}
