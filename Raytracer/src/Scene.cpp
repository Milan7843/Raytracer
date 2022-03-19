#pragma once
#include "Scene.h"

Scene::Scene()
{
}

Scene::~Scene()
{
}


void Scene::addPointLight(PointLight& pointLight)
{
	// Adding a new pointlight and incrementing the counter for this
	pointLight.setIndex(this->pointLightCount);
	pointLights.push_back(pointLight);

	this->pointLightCount++;
}

void Scene::addModel(const std::string& path)
{
	Model model(path, &meshCount, &triangleCount);
	models.push_back(model);
}

std::string& Scene::setShaderVariables(std::string& input)
{
	std::cout << std::endl << "Variable injection:"	<< std::endl;
	std::cout << "$numTriangles     "	<< replace(input, "$numTriangles", std::to_string(triangleCount)) << std::endl;
	std::cout << "$numMeshes        "	<< replace(input, "$numMeshes", std::to_string(meshCount)) << std::endl;
	std::cout << "$numPointLights   "	<< replace(input, "$numPointLights", std::to_string(pointLightCount)) << std::endl;
	return input;
}

void Scene::draw(Shader* shader)
{
	// Updating each model as needed
	for (Model model : models)
	{
		model.draw(shader);
	}
}

void Scene::writeLightsToShader(Shader* shader)
{
	shader->use();
	// Writing point lights to shader
	for (PointLight pointLight : pointLights)
	{
		pointLight.writeToShader(shader);
	}
}

void Scene::checkMeshUpdates(Shader* shader, unsigned int ssbo)
{
	// Updating each model as needed
	for (Model model : models)
	{
		if (model.updated)
		{
			model.writeToShader(shader, ssbo);
			model.updated = false;
		}
	}
}

bool Scene::replace(std::string& str, const std::string& from, const std::string& to)
{
	size_t start_pos = str.find(from);
	if (start_pos == std::string::npos)
		return false;
	str.replace(start_pos, from.length(), to);
	return true;
}
