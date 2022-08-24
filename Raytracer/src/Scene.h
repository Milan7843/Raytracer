#pragma once
#ifndef SCENE_H
#define SCENE_H

#include <vector>

#include "Lights/PointLight.h"
#include "Lights/DirectionalLight.h"
#include "Lights/AmbientLight.h"
#include "ImGuiEditorInterface.h"
#include "Model.h"
#include "Sphere.h"
#include "Material.h"
#include "Camera.h"

#include <iostream>

enum ObjectType
{
	NONE,
	MODEL,
	SPHERE,
	POINT_LIGHT,
	DIRECTIONAL_LIGHT,
	AMBIENT_LIGHT,
	MATERIAL
};

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

	// Remove a point light from the scene
	void deletePointLight(unsigned int index);
	// Remove a directional light from the scene
	void deleteDirectionalLight(unsigned int index);
	// Remove an ambient light from the scene
	void deleteAmbientLight(unsigned int index);

	// Recalculate the indices in the respective light type vector
	void recalculatePointLightIndices();
	void recalculateDirectionalLightIndices();
	void recalculateAmbientLightIndices();


	Model* addModel(std::string& name, std::vector<unsigned int>& meshMaterialIndices, const std::string& path);
	Model* addModel(const std::string& path, unsigned int materialIndex);
	void deleteModel(unsigned int modelIndex);
	// Add a sphere to the scene, returns whether the addition was succesfull
	bool addSphere(Sphere& sphere);
	Sphere* addSphere(glm::vec3 position, float radius, unsigned int materialIndex);
	void deleteSphere(unsigned int sphereIndex);
	void addMaterial(Material& material);

	void recalculateModelIndices();
	void recalculateSphereIndices();


	// Add a camera to the scene
	void addCamera(Camera& camera);
	// Activate a camera by index
	void activateCamera(unsigned int index);
	// Get the active camera
	Camera& getActiveCamera();

	// Draw this scene with the given shader
	void draw(AbstractShader* shader);

	// Draw only the selected objects in this scene with the given shader
	void drawSelected(AbstractShader* shader);

	// Write the data in the lights vector into the shader
	void writeLightsToShader(AbstractShader* shader, bool useGlslCoordinates);
	// Write the data in the materials vector into the shader
	void writeMaterialsToShader(AbstractShader* shader);

	// Update any changed data on the given shader
	void checkObjectUpdates(AbstractShader* shader);

	// Generate and fill up the buffer holding all triangles
	void generateTriangleBuffer();

	// Bind the buffer holding all triangles
	void bindTriangleBuffer();

	// Mark an object of the given type and the index as selected
	void markSelected(ObjectType objectType, unsigned int objectIndex);

	// Draw the currently selected object using ImGui
	void drawCurrentlySelectedObjectInterface();

	// Get the data of the selected object into the integers
	void getSelectedObjectData(unsigned int* objectType, unsigned int* objectIndex);

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

	// Keeping track of the currently selected object
	ImGuiEditorInterface* currentlySelectedObject;

	// Get a pointer to the currently selected objects
	ImGuiEditorInterface* getSelectedObject();

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

	std::vector<Camera> cameras;
	unsigned int activeCamera;

	bool changedTriangleBuffer{ true };

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