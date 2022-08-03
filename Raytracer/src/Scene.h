#pragma once
#ifndef SCENE_H
#define SCENE_H

#include <vector>

#include "Lights/PointLight.h"
#include "Lights/DirectionalLight.h"
#include "Lights/AmbientLight.h"
#include "Model.h"
#include "Sphere.h"
#include "Material.h"

#include <iostream>

class Scene
{
public:
	Scene();
	~Scene();

	// Write this scene to the given filestream
	void writeDataToStream(std::ofstream& filestream);

	// Set the name of this scene
	void setName(std::string name);

	// Set the HDRI used in this scene
	void loadHDRI(const std::string& imageName);
	unsigned int getHDRI();

	// Add a point light to the scene
	void addLight(PointLight& pointLight);

	// Add a directional light to the scene
	void addLight(DirectionalLight& directionalLight);

	// Add an ambient light to the scene
	void addLight(AmbientLight& ambientLight);

	Model* addModel(const std::string& path, unsigned int materialIndex);
	Sphere* addSphere(glm::vec3 position, float radius, unsigned int materialIndex);
	void addMaterial(Material& material);

	// Draw this scene with the given shader
	void draw(AbstractShader* shader);

	// Write the data in the lights vector into the shader
	void writeLightsToShader(AbstractShader* shader);

	// Write the data in the materials vector into the shader
	void writeMaterialsToShader(AbstractShader* shader);

	// Update any changed data on the given shader
	void checkObjectUpdates(AbstractShader* shader);

	// Generate and fill up the buffer holding all triangles
	void generateTriangleBuffer();

	// Bind the buffer holding all triangles
	void bindTriangleBuffer();

	// Get a pointer to the name of this scene
	std::string* getNamePointer();

	bool* getUseHDRIAsBackgroundPointer();

	std::vector<Material>& getMaterials();
	std::vector<PointLight>& getPointLights();
	std::vector<DirectionalLight>& getDirectionalLights();
	std::vector<AmbientLight>& getAmbientLights();
	std::vector<Model>& getModels();
	std::vector<Sphere>& getSpheres();


	unsigned int triangleCount{ 0 };

private:

	std::string name{};

	// The hdri currently loaded
	unsigned int hdri = 0;
	std::string loadedHDRIName{"default_hdri.png"};

	// Keeping track of the lights in this scene
	std::vector<PointLight> pointLights;
	std::vector<DirectionalLight> directionalLights;
	std::vector<AmbientLight> ambientLights;
	unsigned int pointLightCount = 0;
	unsigned int directionalLightCount = 0;
	unsigned int ambientLightCount = 0;
	unsigned int MAX_POINT_LIGHT_COUNT = 10;
	unsigned int MAX_DIR_LIGHT_COUNT = 10;
	unsigned int MAX_AMBIENT_LIGHT_COUNT = 10;
	unsigned int MAX_MATERIAL_COUNT = 10;
	unsigned int MAX_SPHERE_COUNT = 10;
	unsigned int MAX_MESH_COUNT = 10;

	std::vector<Model> models;
	std::vector<Sphere> spheres;

	unsigned int sphereCount = 0;
	unsigned int meshCount = 0;

	// Whether to render the HDRI as a background or just plain colours
	bool useHDRIAsBackground{ true };

	// The buffer for storing mesh triangles
	unsigned int triangleBufferSSBO = 0;

	// Keeping track of the materials
	std::vector<Material> materials;
	unsigned int materialCount = 0;

	// Replace part of a string with another string.
	bool replace(std::string& str, const std::string& from, const std::string& to);
};

#endif