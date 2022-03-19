#pragma once
#ifndef SCENE_H
#define SCENE_H

#include <vector>

#include "Lights/PointLight.h"
#include "Model.h"

class Scene
{
public:
	Scene();
	~Scene();

	void addPointLight(PointLight& pointLight);
	void addModel(const std::string& path);


	// Set the variables for number of things in the shader.
	// e.g. NUM_POINT_LIGHTS, NUM_TRIANGLES
	std::string& setShaderVariables(std::string& input);

	// Draw this scene with the given shader
	void draw(Shader* shader);

	void writeLightsToShader(Shader* shader);

	void checkMeshUpdates(Shader* shader, unsigned int ssbo);

	unsigned int triangleCount = 0;

private:
	std::vector<PointLight> pointLights;
	unsigned int pointLightCount = 0;

	std::vector<Model> models;

	unsigned int meshCount = 0;

	// Replace part of a string with another string.
	bool replace(std::string& str, const std::string& from, const std::string& to);
};

#endif