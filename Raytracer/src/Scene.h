#pragma once
#ifndef SCENE_H
#define SCENE_H

#include <vector>

#include "Lights/PointLight.h"
#include "Model.h"
#include "Sphere.h"
#include "Material.h"

class Scene
{
public:
	Scene();
	~Scene();

	void addPointLight(PointLight& pointLight);
	Model* addModel(const std::string& path, unsigned int materialIndex);
	Sphere* addSphere(glm::vec3 position, float radius, unsigned int materialIndex);
	void addMaterial(Material& material);


	// Set the variables for number of things in the shader.
	// e.g. NUM_POINT_LIGHTS, NUM_TRIANGLES
	std::string& setShaderVariables(std::string& input);

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

	unsigned int triangleCount = 0;

private:
	// Keeping track of the lights in this scene
	std::vector<PointLight> pointLights;
	unsigned int pointLightCount = 0;

	std::vector<Model> models;
	std::vector<Sphere> spheres;

	unsigned int sphereCount = 0;
	unsigned int meshCount = 0;

	// The buffer for storing mesh triangles
	unsigned int triangleBufferSSBO = 0;

	// Keeping track of the materials
	std::vector<Material> materials;
	unsigned int materialCount = 0;

	// Replace part of a string with another string.
	bool replace(std::string& str, const std::string& from, const std::string& to);
};

#endif