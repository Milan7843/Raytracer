#pragma once
#include "Scene.h"

// For HDRI loading
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

Scene::Scene()
{
}

Scene::~Scene()
{
}

void Scene::writeDataToStream(std::ofstream& filestream)
{
	// Writing the scene's data to the filestream
	filestream << loadedHDRIName << "\n";

	// Writing all materials
	filestream << "# Materials\n";
	for (Material& material : materials)
	{
		material.writeDataToStream(filestream);
		// Write newline if it is not the last item
		if (&material != &materials.back())
			filestream << "\n";
	}
	filestream << "# Materials end\n\n";

	// Writing all spheres
	filestream << "# Spheres\n";
	for (Sphere& sphere : getSpheres())
	{
		sphere.writeDataToStream(filestream);
		// Write newline if it is not the last item
		if (&sphere != &getSpheres().back())
			filestream << "\n";
	}
	filestream << "# Spheres end\n\n";

	// Writing all models
	filestream << "# Models\n";
	for (Model& model : getModels())
	{
		model.writeDataToStream(filestream);
		// Write newline if it is not the last item
		if (&model != &getModels().back())
			filestream << "\n";
	}
	filestream << "# Models end\n\n";

	// Writing all lights
	filestream << "# Point lights\n";
	for (PointLight& light : getPointLights())
	{
		light.writeDataToStream(filestream);
		// Write newline if it is not the last item
		if (&light != &getPointLights().back())
			filestream << "\n";
	}
	filestream << "# Point lights end\n\n";

	filestream << "# Directional lights\n";
	for (DirectionalLight& light : getDirectionalLights())
	{
		light.writeDataToStream(filestream);
		// Write newline if it is not the last item
		if (&light != &getDirectionalLights().back())
			filestream << "\n";
	}
	filestream << "# Directional lights end\n\n";

	filestream << "# Ambient lights\n";
	for (AmbientLight& light : getAmbientLights())
	{
		light.writeDataToStream(filestream);
		// Write newline if it is not the last item
		if (&light != &getAmbientLights().back())
			filestream << "\n";
	}
	filestream << "# Ambient lights end\n\n";

	// Writing all cameras
	filestream << "# Cameras\n";
	for (Camera& camera : cameras)
	{
		camera.writeDataToStream(filestream);
		// Write newline if it is not the last item
		if (&camera != &cameras.back())
			filestream << "\n";
	}
	filestream << "# Cameras end\n\n";
}

void Scene::setName(std::string name)
{
	this->name = name;
}

void Scene::loadHDRI(const std::string& imageName)
{
	std::string fileName = "HDRIs/" + imageName;

	unsigned int textureID;
	glGenTextures(1, &textureID);

	int width, height, nrComponents;
	unsigned char* data = stbi_load(fileName.c_str(), &width, &height, &nrComponents, 0);

	if (data)
	{
		GLenum format;
		if (nrComponents == 1)
			format = GL_RED;
		else if (nrComponents == 3)
			format = GL_RGB;
		else if (nrComponents == 4)
			format = GL_RGBA;

		glBindTexture(GL_TEXTURE_2D, textureID);
		glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);

		//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	}
	else
	{
		Logger::logError("Failed to load HDRI: " + fileName + ".\nReason: " + stbi_failure_reason());
	}

	stbi_image_free(data);

	// Saving the data to this scene
	this->hdri = textureID;
	this->loadedHDRIName = imageName;
}

unsigned int Scene::getHDRI()
{
	return hdri;
}


void Scene::addLight(PointLight& pointLight)
{
	// Full of point lights
	if (this->pointLightCount >= MAX_POINT_LIGHT_COUNT)
		return;

	// Adding a new point light and incrementing the counter for this
	pointLight.setIndex(this->pointLightCount);
	pointLights.push_back(pointLight);

	this->pointLightCount++;
}

void Scene::addLight(DirectionalLight& directionalLight)
{
	// Full of directional lights
	if (this->directionalLightCount >= MAX_DIR_LIGHT_COUNT)
		return;

	// Adding a new directional light and incrementing the counter for this
	directionalLight.setIndex(this->directionalLightCount);
	directionalLights.push_back(directionalLight);

	this->directionalLightCount++;
}

void Scene::addLight(AmbientLight& ambientLight)
{
	// Full of ambient lights
	if (this->ambientLightCount >= MAX_AMBIENT_LIGHT_COUNT)
		return;

	// Adding a new ambient light and incrementing the counter for this
	ambientLight.setIndex(this->ambientLightCount);
	ambientLights.push_back(ambientLight);

	this->ambientLightCount++;
}

Model* Scene::addModel(std::string& name, std::vector<unsigned int>& meshMaterialIndices, const std::string& path)
{
	Model model(name, meshMaterialIndices, path, &meshCount, &triangleCount, MAX_MESH_COUNT);
	models.push_back(model);
	return &(models[models.size() - 1]);
}

Model* Scene::addModel(const std::string& path, unsigned int materialIndex)
{
	Model model(materialIndex, path, &meshCount, &triangleCount, MAX_MESH_COUNT);
	models.push_back(model);
	return &(models[models.size() - 1]);
}

bool Scene::addSphere(Sphere& sphere)
{
	// Full of spheres
	if (this->sphereCount >= MAX_SPHERE_COUNT)
		return false;

	sphere.setShaderSphereIndex(sphereCount);

	sphereCount++;
	spheres.push_back(sphere);
	return true;
}

Sphere* Scene::addSphere(glm::vec3 position, float radius, unsigned int materialIndex)
{
	// Full of spheres
	if (this->sphereCount >= MAX_SPHERE_COUNT)
		return nullptr;

	Sphere sphere(position, radius, materialIndex, sphereCount);
	sphereCount++;
	spheres.push_back(sphere);
	return &(spheres[spheres.size() - 1]);
}

void Scene::addMaterial(Material& material)
{
	// Full of materials
	if (this->materialCount >= MAX_MATERIAL_COUNT)
		return;

	// Adding a new material and incrementing the counter for this
	materials.push_back(material);
	this->materialCount++;
}

void Scene::addCamera(Camera& camera)
{
	this->cameras.push_back(camera);
}

void Scene::activateCamera(unsigned int index)
{
	this->activeCamera = index;
}

Camera& Scene::getActiveCamera()
{
	return cameras[activeCamera];
}

void Scene::draw(AbstractShader* shader)
{
	// Drawing each model with the given shader
	for (Model& model : models)
	{
		model.draw(shader, (Scene*)this);
	}

	// Drawing each sphere with the given shader
	for (Sphere& sphere : spheres)
	{
		sphere.draw(shader, (Scene*)this);
	}
}

void Scene::writeLightsToShader(AbstractShader* shader)
{
	shader->use();

	// Writing current lights amounts to shader
	shader->setInt("pointLightCount", pointLightCount);
	shader->setInt("dirLightCount", directionalLightCount);
	shader->setInt("ambientLightCount", ambientLightCount);

	// Writing lights to shader
	for (PointLight& light : getPointLights())
	{
		light.writeToShader(shader);
	}
	for (DirectionalLight& light : getDirectionalLights())
	{
		light.writeToShader(shader);
	}
	for (AmbientLight& light : getAmbientLights())
	{
		light.writeToShader(shader);
	}
}

void Scene::writeMaterialsToShader(AbstractShader* shader)
{
	shader->use();

	// Writing current material count to shader
	shader->setInt("materialCount", materialCount);

	// Writing materials to shader
	unsigned int index = 0;
	for (Material& material : materials)
	{
		material.writeToShader(shader, index++);
	}
}

void Scene::checkObjectUpdates(AbstractShader* shader)
{
	// Activate the shader program so that uniforms can be set
	shader->use();

	// Writing current sphere count to shader
	shader->setInt("sphereCount", sphereCount);

	// Updating each model as needed
	for (Model& model : models)
	{
		if (model.updated)
		{
			model.writeToShader(shader, triangleBufferSSBO);
			model.updated = false;
		}
	}

	// Updating each sphere as needed
	for (Sphere& sphere : spheres)
	{
		if (sphere.updated)
		{
			sphere.writeToShader(shader, triangleBufferSSBO);
			//sphere.updated = false;
		}
	}
}

void Scene::generateTriangleBuffer()
{
	// Generating a buffer for the triangles to go into
	triangleBufferSSBO = 0;
	glGenBuffers(1, &triangleBufferSSBO);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, triangleBufferSSBO);

	Logger::log("Making room for " + std::to_string(triangleCount) + " triangles in SSBO " + std::to_string(triangleBufferSSBO));

	// Loading zero-data into the new buffer
	glBufferData(GL_SHADER_STORAGE_BUFFER, triangleCount * Mesh::getTriangleSize(), 0, GL_DYNAMIC_COPY);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 2, triangleBufferSSBO);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
}

void Scene::bindTriangleBuffer()
{
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, triangleBufferSSBO);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 2, triangleBufferSSBO);
}

std::string* Scene::getNamePointer()
{
	return &name;
}

bool* Scene::getUseHDRIAsBackgroundPointer()
{
	return &useHDRIAsBackground;
}

std::vector<Material>& Scene::getMaterials()
{
	return materials;
}

std::vector<PointLight>& Scene::getPointLights()
{
	return pointLights;
}

std::vector<DirectionalLight>& Scene::getDirectionalLights()
{
	return directionalLights;
}

std::vector<AmbientLight>& Scene::getAmbientLights()
{
	return ambientLights;
}

std::vector<Model>& Scene::getModels()
{
	return models;
}

std::vector<Sphere>& Scene::getSpheres()
{
	return spheres;
}

bool Scene::replace(std::string& str, const std::string& from, const std::string& to)
{
	size_t start_pos = str.find(from);
	if (start_pos == std::string::npos)
		return false;
	str.replace(start_pos, from.length(), to);
	return true;
}
