#pragma once
#ifndef SCENE_H
#define SCENE_H

#include <vector>

#include "Lights/PointLight.h"
#include "Model.h"
#include "Material.h"

class Scene
{
public:
	Scene();
	~Scene();

	void addPointLight(PointLight& pointLight);
	Model* addModel(const std::string& path, unsigned int materialIndex);
	void addMaterial(Material& material);


	// Set the variables for number of things in the shader.
	// e.g. NUM_POINT_LIGHTS, NUM_TRIANGLES
	std::string& setShaderVariables(std::string& input);

	// Draw this scene with the given shader
	void draw(Shader* shader);

	// Write the data in the lights vector into the shader
	void writeLightsToShader(Shader* shader);

	// Write the data in the materials vector into the shader
	void writeMaterialsToShader(Shader* shader);

	// Get whether any mesh has their updated variable set to true
	void checkMeshUpdates(Shader* shader, unsigned int ssbo);

	unsigned int triangleCount = 0;

private:
	// Keeping track of the lights in this scene
	std::vector<PointLight> pointLights;
	unsigned int pointLightCount = 0;

	std::vector<Model> models;

	unsigned int meshCount = 0;

	// Keeping track of the materials
	std::vector<Material> materials;
	unsigned int materialCount = 0;

	// Replace part of a string with another string.
	bool replace(std::string& str, const std::string& from, const std::string& to);
};

#endif