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
	return &(models[models.size() - 1]);
}

Sphere* Scene::addSphere(glm::vec3 position, float radius, unsigned int materialIndex)
{
	Sphere sphere(position, radius, materialIndex, sphereCount);
	sphereCount++;
	spheres.push_back(sphere);
	return &(spheres[spheres.size() - 1]);
}

void Scene::addMaterial(Material& material)
{
	// Adding a new material and incrementing the counter for this
	materials.push_back(material);
	this->materialCount++;
}

std::string& Scene::setShaderVariables(std::string& input)
{
	std::cout << std::endl << "Variable injection:" << std::endl;
	std::cout << "$numTriangles     " << replace(input, "$numTriangles", std::to_string(triangleCount)) << std::endl;
	std::cout << "$numMeshes        " << replace(input, "$numMeshes", std::to_string(meshCount)) << std::endl;
	std::cout << "$numPointLights   " << replace(input, "$numPointLights", std::to_string(pointLightCount)) << std::endl;
	std::cout << "$numMaterials     " << replace(input, "$numMaterials", std::to_string(materialCount)) << std::endl;
	std::cout << "$numSpheres       " << replace(input, "$numSpheres", std::to_string(sphereCount)) << std::endl;
	// TODO: get window size from somewhere
	std::cout << "$numPixels        " << replace(input, "$numPixels", std::to_string(1200*700)) << std::endl;
	return input;
}

void Scene::draw(AbstractShader* shader)
{
	// Drawing each model with the given shader
	for (Model model : models)
	{
		model.draw(shader, &materials[model.materialIndex]);
	}

	// Drawing each sphere with the given shader
	for (Sphere sphere : spheres)
	{
		sphere.draw(shader, &materials[sphere.materialIndex]);
	}
}

void Scene::writeLightsToShader(AbstractShader* shader)
{
	shader->use();

	// Writing point lights to shader
	for (PointLight pointLight : pointLights)
	{
		pointLight.writeToShader(shader);
	}
}

void Scene::writeMaterialsToShader(AbstractShader* shader)
{
	shader->use();

	// Writing materials to shader
	unsigned int index = 0;
	for (Material material : materials)
	{
		material.writeToShader(shader, index++);
	}
}

void Scene::checkObjectUpdates(AbstractShader* shader)
{
	// Updating each model as needed
	for (Model model : models)
	{
		if (model.updated)
		{
			model.writeToShader(shader, triangleBufferSSBO);
			model.updated = false;
		}
	}

	// Updating each sphere as needed
	for (Sphere sphere : spheres)
	{
		if (sphere.updated)
		{
			sphere.writeToShader(shader, triangleBufferSSBO);
			sphere.updated = false;
		}
	}
}

void Scene::generateTriangleBuffer()
{
	// Generating a buffer for the triangles to go into
	triangleBufferSSBO = 0;
	glGenBuffers(1, &triangleBufferSSBO);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, triangleBufferSSBO);

	std::cout << "Making room for " << triangleCount << " triangles" << std::endl;

	glBufferData(GL_SHADER_STORAGE_BUFFER, triangleCount * Mesh::getTriangleSize(), 0, GL_DYNAMIC_COPY);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 2, triangleBufferSSBO);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
}

void Scene::bindTriangleBuffer()
{
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, triangleBufferSSBO);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 2, triangleBufferSSBO);
}

bool Scene::replace(std::string& str, const std::string& from, const std::string& to)
{
	size_t start_pos = str.find(from);
	if (start_pos == std::string::npos)
		return false;
	str.replace(start_pos, from.length(), to);
	return true;
}
