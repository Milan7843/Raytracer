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
#include "BVH/BVH.h"
#include "gui/ContextMenuSource.h"
#include "InputManager.h"
#include "ImageLoader.h"
#include "gui/GizmoRenderer.h"
#include "TextureHandler.h"
#include "RasterizedDebugMode.h"
#include "TextureHandler.h"

#include <iostream>

struct ShaderMesh
{
	glm::mat4 transformation;
	int material;
	int padding[3];
};

struct ShaderMaterial
{
	glm::vec4 color; // 1
	glm::vec4 emission; // 2
	float reflectiveness;
	float transparency;
	float refractiveness;
	float reflectionDiffusion; // 3
	float emissionStrength;
	float fresnelReflectionStrength;

	int hasAlbedoTexture;
	float albedoTexture_xMin; // 4
	float albedoTexture_xMax;
	float albedoTexture_yMin;
	float albedoTexture_yMax;

	int hasNormalTexture; // 5
	float normalTexture_xMin;
	float normalTexture_xMax;
	float normalTexture_yMin;
	float normalTexture_yMax; // 6
	float normalMapStrength;
	float pad1;
	float pad2;
	float pad3; // 7
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
	void loadHDRI(const std::string& imagePath);
	Texture* getHDRI();
	bool hasHDRI();
	void removeHDRI();


	// Add a point light to the scene
	void addLight(PointLight& pointLight);
	// Add a directional light to the scene
	void addLight(DirectionalLight& directionalLight);
	// Add an ambient light to the scene
	void addLight(AmbientLight& ambientLight);

	// Add an empty point light to the scene
	PointLight* addPointLight();
	// Add an empty directional light to the scene
	DirectionalLight* addDirectionalLight();
	// Add an empty ambient light to the scene
	AmbientLight* addAmbientLight();

	void deleteObject(unsigned int id);

	// Remove a point light from the scene
	bool deletePointLight(unsigned int id);
	// Remove a directional light from the scene
	bool deleteDirectionalLight(unsigned int id);
	// Remove an ambient light from the scene
	bool deleteAmbientLight(unsigned int id);

	// Recalculate the indices in the respective light type vector
	void recalculatePointLightIndices();
	void recalculateDirectionalLightIndices();
	void recalculateAmbientLightIndices();


	Model* addModel(std::string& name, const std::string& path);
	Model* addModel(const std::string& path);
	bool deleteModel(unsigned int id);
	Model* getModelByID(unsigned int id);
	// Add a sphere to the scene, returns whether the addition was succesfull
	bool addSphere(Sphere& sphere);

	// Add an empty sphere to the scene
	Sphere* addSphere(unsigned int materialIndex);
	Sphere* addSphere(glm::vec3 position, float radius, unsigned int materialIndex);
	bool deleteSphere(unsigned int id);
	void addMaterial(Material& material);

	// Remove a material from the scene
	bool deleteMaterial(unsigned int id);

	void recalculateModelIndices();
	void recalculateSphereIndices();


	// Add a camera to the scene
	void addCamera(Camera& camera);
	// Activate a camera by index
	void activateCamera(unsigned int index);
	// Get the active camera
	Camera& getActiveCamera();

	void updateCamera(float deltaTime);
	void onScroll(float delta);

	// Draw this scene with the given shader
	void draw(AbstractShader* shader, RasterizedDebugMode debugMode = RasterizedDebugMode::REGULAR);

	// Draw only the selected objects in this scene with the given shader
	void drawSelected(AbstractShader* shader);

	// Get whether an object is selected or not
	bool hasObjectSelected();

	ContextMenuSource* getContextMenuSourceFromSelected();

	ContextMenuSource* getContextMenuSourceByID(unsigned int objectID);

	ImGuiEditorInterface* getImGuiEditorInterfaceByID(unsigned int objectID);

	Object* getObjectFromSelected();
	Object* getObjectByID(unsigned int objectID);
	bool isObjectSelected(unsigned int objectID);

	ObjectType getSelectedObjectType();

	void renderContextMenus();

	// Write the data in the lights vector into the shader
	// Returns whether any new data was written to the shader
	bool writeLightsToShader(AbstractShader* shader, bool useGlslCoordinates);

	// Write the data in the materials vector into the shader
	// Returns whether any new data was written to the shader
	bool writeMaterialsToShader(AbstractShader* shader);

	void bindMaterialsBuffer();

	// Return whether there are any updates to the scene data required on this shader
	bool checkObjectUpdates(AbstractShader* shader);
	// Update any changed data on the given shader
	void writeObjectsToShader(AbstractShader* shader);

	// Generate and fill up the buffer holding all triangles
	void generateTriangleBuffer();

	// Bind the buffer holding all triangles
	void bindTriangleBuffer();

	// Mark an object with the given ID as selected
	void markSelected(unsigned int objectID);

	// Draw the currently selected object using ImGui
	void drawCurrentlySelectedObjectInterface();

	// Get a pointer to the name of this scene
	std::string* getNamePointer();

	// Get the name of this scene
	std::string getName() const;

	bool* getUseHDRIAsBackgroundPointer();

	std::vector<Material>& getMaterials();
	std::vector<PointLight>& getPointLights();
	std::vector<DirectionalLight>& getDirectionalLights();
	std::vector<AmbientLight>& getAmbientLights();
	std::vector<Model>& getModels();
	std::vector<Sphere>& getSpheres();

	void drawGizmos(GizmoRenderer& renderer);
	void drawClickSelectGizmos(GizmoRenderer& renderer);
	
	// Update the BVH according to the scene
	void updateBVH();

	BVH& getBVH();

	unsigned int triangleCount{ 0 };

	glm::vec3 getRotationPoint();

	// Verify the pointer of each mesh to its parent model
	void verifyMeshModelPointers();

	void markAllChangesSaved();
	bool hasUnsavedChanges();

private:

	void generateMeshBuffer(std::vector<ShaderMesh>& shaderMeshes);

	void onObjectDeleted(unsigned int id);

	BVH bvh;

	std::string name{};

	// Keeping track of the currently selected object
	unsigned int currentlySelectedObject;

	// Get a pointer to the currently selected object
	ImGuiEditorInterface* getSelectedObject();

	// The hdri currently loaded
	std::shared_ptr<Texture> hdri;

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
	unsigned int MAX_MATERIAL_COUNT = 1000; // no max
	unsigned int MAX_SPHERE_COUNT = 10;
	unsigned int MAX_MESH_COUNT = 100;

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

	unsigned int meshBufferSSBO{ 0 };

	unsigned int materialsBufferSSBO{ 0 };

	// Keeping track of the materials
	std::vector<Material> materials;
	unsigned int materialCount = 0;

	// Rendering size: only used for aspect ratio
	unsigned int width{ 0 };
	unsigned int height{ 0 };

	// Replace part of a string with another string.
	bool replace(std::string& str, const std::string& from, const std::string& to);

	glm::vec3 rotationPoint{ glm::vec3(0.0f) };
};

#endif