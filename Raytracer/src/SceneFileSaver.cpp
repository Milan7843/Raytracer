#include "SceneFileSaver.h"

glm::vec3 readVec3(std::ifstream& input);
void readMaterials(std::ifstream& filestream, Scene& scene);
void readModels	(std::ifstream& filestream, Scene& scene);
void readSpheres(std::ifstream& filestream, Scene& scene);
void readPointLights	(std::ifstream& filestream, Scene& scene);
void readDirectionalLights	(std::ifstream& filestream, Scene& scene);
void readAmbientLights	(std::ifstream& filestream, Scene& scene);

void SceneFileSaver::writeSceneToFile(Scene& scene, const std::string& fileName)
{
	// The data stream into the file
	std::ofstream filestream { "scenes/" + fileName + ".scene"};

	// Writing all scene data
	scene.writeDataToStream(filestream);

	// Done writing so flush data and close filestream
	filestream.close();
}

Scene SceneFileSaver::readSceneFromFile(const std::string& fileName)
{
	// Opening the scene file into a stream
	std::ifstream filestream{ "scenes/" + fileName + ".scene" };

	// Throw an error if the file could not be opened
	if (!filestream)
	{
		throw "Scene " + fileName + " could not be opened. "
			"Make sure it is in the scene directory and ends in '.scene'.";
	}

	// Otherwise create a scene
	Scene scene{};

	// String buffer for file data
	std::string buffer;

	// Then load all the data into it
	std::getline(filestream, buffer);
	scene.setName(buffer);

	// Loading all important scene data
	readMaterials(filestream, scene);
	readSpheres(filestream, scene);
	readModels(filestream, scene);
	readPointLights(filestream, scene);
	readDirectionalLights(filestream, scene);
	readAmbientLights(filestream, scene);

	// Finally closing the file
	filestream.close();

	return scene;
}

glm::vec3 readVec3(std::ifstream& input)
{
	float x, y, z;

	input >> x;
	input >> y;
	input >> z;

	return glm::vec3(x, y, z);
}

void readMaterials(std::ifstream& filestream, Scene& scene)
{
	// String buffer for any full-line reading
	std::string buffer;

	while (filestream)
	{
		// Reached end of materials
		if (buffer == std::string{ "# Materials end"})
		{
			// Skip a line and stop parsing materials
			std::getline(filestream, buffer);
			return;
		}

		// Getting the material's name
		std::getline(filestream, buffer);

		// Defining all other data
		glm::vec3 color;
		float reflectiveness;
		float refractiveness;
		float transparency;
		glm::vec3 emission;

		// Then getting said data
		color = readVec3(filestream);
		filestream >> reflectiveness;
		filestream >> refractiveness;
		filestream >> transparency;
		emission = readVec3(filestream);

		// Creating the material with the read properties
		Material material(buffer, color, reflectiveness, transparency, refractiveness, emission);

		// And adding the material to the scene
		scene.addMaterial(material);

		// Skipping two lines
		std::getline(filestream, buffer);
		std::getline(filestream, buffer);
	}
}


void readSpheres(std::ifstream& filestream, Scene& scene)
{
	// String buffer for any full-line reading
	std::string buffer;

	while (filestream)
	{
		// Reading the next line to find out whether we reached the end of spheres
		std::getline(filestream, buffer);

		// Reached end of spheres
		if (buffer == std::string{ "# Spheres end" })
		{
			// Skip a line and stop parsing spheres
			std::getline(filestream, buffer);
			return;
		}

		// Defining all other data
		glm::vec3 position;
		glm::vec3 rotation;
		glm::vec3 scale;
		int materialIndex;

		// Then getting said data
		position = readVec3(filestream);
		rotation = readVec3(filestream);
		scale = readVec3(filestream);
		filestream >> materialIndex;

		// Creating the sphere with the read properties
		scene.addSphere(position, scale.x, materialIndex);

		// Skipping two lines
		std::getline(filestream, buffer);
	}
}


void readModels(std::ifstream& filestream, Scene& scene)
{
	// String buffer for any full-line reading
	std::string buffer;

	while (filestream)
	{
		// Reading the next line to find out whether we reached the end of spheres
		std::getline(filestream, buffer);

		// Reached end of spheres
		if (buffer == std::string{ "# Models end" })
		{
			// Skip a line and stop parsing models
			std::getline(filestream, buffer);
			return;
		}

		// Defining all other data
		glm::vec3 position;
		glm::vec3 rotation;
		glm::vec3 scale;
		int materialIndex;

		// Then getting said data
		position = readVec3(filestream);
		rotation = readVec3(filestream);
		scale = readVec3(filestream);
		std::getline(filestream, buffer); // Skip a line
		std::getline(filestream, buffer); // The path
		filestream >> materialIndex;

		// Creating the model with the read properties
		Model* model = scene.addModel(buffer, materialIndex);

		// Then applying transformations
		model->setPosition(position);
		model->setRotation(rotation);
		model->scale(scale);

		// Skipping two lines
		std::getline(filestream, buffer);
	}
}


void readPointLights(std::ifstream& filestream, Scene& scene)
{
	// String buffer for any full-line reading
	std::string buffer;

	while (filestream)
	{
		// Reading the next line to find out whether we reached the end of spheres
		std::getline(filestream, buffer);

		// Reached end of spheres
		if (buffer == std::string{ "# Point lights end" })
		{
			// Skip a line and stop parsing models
			std::getline(filestream, buffer);
			return;
		}

		// Defining all other data
		glm::vec3 color;
		float intensity;
		glm::vec3 position;

		// Then getting said data
		color = readVec3(filestream);
		filestream >> intensity;
		position = readVec3(filestream);

		// Creating the light with the read properties
		PointLight light(position, color, intensity);
		scene.addLight(light);

		// Skipping a line
		std::getline(filestream, buffer);
	}
}


void readDirectionalLights(std::ifstream& filestream, Scene& scene)
{
	// String buffer for any full-line reading
	std::string buffer;

	while (filestream)
	{
		// Reading the next line to find out whether we reached the end of spheres
		std::getline(filestream, buffer);

		// Reached end of spheres
		if (buffer == std::string{ "# Directional lights end" })
		{
			// Skip a line and stop parsing models
			std::getline(filestream, buffer);
			return;
		}

		// Defining all other data
		glm::vec3 color;
		float intensity;
		glm::vec3 direction;

		// Then getting said data
		color = readVec3(filestream);
		filestream >> intensity;
		direction = readVec3(filestream);

		// Creating the light with the read properties
		DirectionalLight light(direction, color, intensity);
		scene.addLight(light);

		// Skipping a line
		std::getline(filestream, buffer);
	}
}


void readAmbientLights(std::ifstream& filestream, Scene& scene)
{
	// String buffer for any full-line reading
	std::string buffer;

	while (filestream)
	{
		// Reading the next line to find out whether we reached the end of spheres
		std::getline(filestream, buffer);

		// Reached end of spheres
		if (buffer == std::string{ "# Ambient lights end" })
		{
			// Skip a line and stop parsing models
			std::getline(filestream, buffer);
			return;
		}

		// Defining all other data
		glm::vec3 color;
		float intensity;

		// Then getting said data
		color = readVec3(filestream);
		filestream >> intensity;

		// Creating the light with the read properties
		AmbientLight light(color, intensity);
		scene.addLight(light);

		// Skipping a line
		std::getline(filestream, buffer);
	}
}
