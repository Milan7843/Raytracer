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

Model* Scene::addModel(const std::string& path, unsigned int materialIndex)
{
	Model model(path, &meshCount, &triangleCount, materialIndex);
	models.push_back(model);
	return &(models[models.size()-1]);
}

void Scene::addMaterial(Material& material)
{
	// Adding a new material and incrementing the counter for this
	materials.push_back(material);
	this->materialCount++;
}

std::string& Scene::setShaderVariables(std::string& input)
{
	std::cout << std::endl << "Variable injection:"	<< std::endl;
	std::cout << "$numTriangles     "	<< replace(input, "$numTriangles", std::to_string(triangleCount)) << std::endl;
	std::cout << "$numMeshes        "	<< replace(input, "$numMeshes", std::to_string(meshCount)) << std::endl;
	std::cout << "$numPointLights   "	<< replace(input, "$numPointLights", std::to_string(pointLightCount)) << std::endl;
	std::cout << "$numMaterials   "		<< replace(input, "$numMaterials", std::to_string(materialCount)) << std::endl;
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

void Scene::writeMaterialsToShader(Shader* shader)
{
	shader->use();
	// Writing materials to shader
	unsigned int index = 0;
	for (Material material : materials)
	{
		material.writeToShader(shader, index++);
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
