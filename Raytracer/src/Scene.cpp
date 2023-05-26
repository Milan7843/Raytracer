#pragma once
#include "Scene.h"

// For HDRI loading
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

Scene::Scene()
{
	Material material(Material::generateErrorMaterial());
	materials.push_back(material);
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
		// Skip the first material: it is the None material
		if (&material == &materials.front())
			continue;

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

void Scene::deletePointLight(unsigned int index)
{
	pointLights.erase(pointLights.begin() + index);
	this->pointLightCount--;
	recalculatePointLightIndices();
}

void Scene::deleteDirectionalLight(unsigned int index)
{
	directionalLights.erase(directionalLights.begin() + index);
	this->directionalLightCount--;
	recalculateDirectionalLightIndices();
}

void Scene::deleteAmbientLight(unsigned int index)
{
	ambientLights.erase(ambientLights.begin() + index);
	this->ambientLightCount--;
	recalculateAmbientLightIndices();
}

void Scene::recalculatePointLightIndices()
{
	unsigned int i = 0;

	// Updating each light's index
	for (Light& light : pointLights)
	{
		light.setIndex(i++);
	}
}

void Scene::recalculateDirectionalLightIndices()
{
	unsigned int i = 0;

	// Updating each light's index
	for (Light& light : directionalLights)
	{
		light.setIndex(i++);
	}
}

void Scene::recalculateAmbientLightIndices()
{
	unsigned int i = 0;

	// Updating each light's index
	for (Light& light : ambientLights)
	{
		light.setIndex(i++);
	}
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

void Scene::deleteModel(unsigned int modelIndex)
{
	models.erase(models.begin() + modelIndex);
	recalculateModelIndices();
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

void Scene::deleteSphere(unsigned int sphereIndex)
{
	// Removing it
	spheres.erase(spheres.begin() + sphereIndex);
	// Then making sure the sphere indices are still contiguous
	recalculateSphereIndices();
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

void Scene::deleteMaterial(unsigned int index)
{
	// Must have at least one material: the NONE material
	if (this->materials.size() <= 1)
		return;

	// Cannot delete the NONE material at index 0
	if (index <= 0)
		return;

	// If there were more materials: delete the one at the index
	this->materials.erase(materials.begin() + index);

	// Then verifying each model's material indices
	for (Sphere& sphere : getSpheres())
	{
		sphere.onDeleteMaterial(index);
	}

	for (Model& model : getModels())
	{
		model.onDeleteMaterial(index);
	}
}

void Scene::recalculateModelIndices()
{
	unsigned int triangleCount{ 0 };

	this->meshCount = 0;

	// Updating each model
	for (Model& model : models)
	{
		model.resetShaderIndices(&triangleCount, &meshCount);
	}

	// Must create a new triangle buffer for the new number of triangles
	generateTriangleBuffer();
}

void Scene::recalculateSphereIndices()
{
	unsigned int i = 0;

	// Updating each sphere's index
	for (Sphere& sphere : spheres)
	{
		sphere.setShaderSphereIndex(i);
		i++;
	}
}

void Scene::setAspectRatio(int width, int height)
{
	// Updating the aspect ratio of all cameras to the new one
	for (Camera& camera : cameras)
	{
		camera.setAspectRatio(width, height);
	}
}

void Scene::addCamera(Camera& camera)
{
	this->cameras.push_back(camera);

	// Setting the aspect ratio of the camera to the one currently being used
	camera.setAspectRatio(width, height);
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
	shader->use();

	// First writing all material data to the shader
	writeMaterialsToShader(shader);
	shader->setVector3("cameraPos", getActiveCamera().getPosition());

	// View matrix
	glm::mat4 view = glm::mat4(1.0f);
	view = getActiveCamera().getViewMatrix();
	shader->setMat4("view", view);

	// Projection matrix
	glm::mat4 projection;
	projection = getActiveCamera().getProjectionMatrix();
	shader->setMat4("projection", projection);

	// Binding the hdri
	shader->setInt("hdri", 0);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, getHDRI());

	// Drawing each model with the given shader
	unsigned int objectIndex{ 0 };
	// Setting the object type to model
	shader->setInt("objectType", MODEL);
	for (Model& model : models)
	{
		shader->setInt("objectIndex", objectIndex);
		model.draw(shader, (Scene*)this);
		objectIndex++;
	}

	// Drawing each sphere with the given shader
	objectIndex = 0;
	// Setting the object type to sphere
	shader->setInt("objectType", SPHERE);
	for (Sphere& sphere : spheres)
	{
		shader->setInt("objectIndex", objectIndex);
		sphere.draw(shader, (Scene*)this);
		objectIndex++;
	}
}

void Scene::drawSelected(AbstractShader* shader)
{
	// Drawing each model with the given shader if it is selected
	for (Model& model : models)
	{
		if (&model == getSelectedObject())
			model.draw(shader, (Scene*)this);
	}

	// Drawing each sphere with the given shader if it is selected
	for (Sphere& sphere : spheres)
	{
		if (&sphere == getSelectedObject())
			sphere.draw(shader, (Scene*)this);
	}
}

bool Scene::writeLightsToShader(AbstractShader* shader, bool useGlslCoordinates)
{
	shader->use();

	// Writing current lights amounts to shader
	shader->setInt("pointLightCount", pointLightCount);
	shader->setInt("dirLightCount", directionalLightCount);
	shader->setInt("ambientLightCount", ambientLightCount);

	bool anyDataWritten = false;
	// Writing lights to shader
	for (PointLight& light : getPointLights())
	{
		anyDataWritten |= light.writeToShader(shader, useGlslCoordinates);
	}
	for (DirectionalLight& light : getDirectionalLights())
	{
		anyDataWritten |= light.writeToShader(shader, useGlslCoordinates);
	}
	for (AmbientLight& light : getAmbientLights())
	{
		anyDataWritten |= light.writeToShader(shader);
	}
	return anyDataWritten;
}

bool Scene::writeMaterialsToShader(AbstractShader* shader)
{
	shader->use();

	// Writing current material count to shader
	shader->setInt("materialCount", materialCount);

	// Writing materials to shader
	unsigned int index = 0;
	bool anyDataWritten = false;
	for (Material& material : materials)
	{
		anyDataWritten |= material.writeToShader(shader, index++);
	}
	return anyDataWritten;
}

bool Scene::checkObjectUpdates(AbstractShader* shader)
{
	// Checking each model for updated data
	for (Model& model : models)
	{
		if (!model.hasWrittenToShader(shader) || changedTriangleBuffer)
		{
			Logger::log(std::to_string(changedTriangleBuffer) + " model updated");
			return true;
		}
	}

	// Checking each sphere for updated data
	for (Sphere& sphere : spheres)
	{
		if (!sphere.hasWrittenToShader(shader))
		{
			Logger::log("sphere updated");
			return true;
		}
	}

	return false;
}

void Scene::writeObjectsToShader(AbstractShader* shader)
{
	// Activate the shader program so that uniforms can be set
	shader->use();

	// Writing current sphere count to shader
	shader->setInt("sphereCount", sphereCount);

	// Updating each model as needed
	for (Model& model : models)
	{
		model.writeToShader(shader, triangleBufferSSBO);
	}

	// Updating each sphere as needed
	for (Sphere& sphere : spheres)
	{
		sphere.writeToShader(shader, triangleBufferSSBO);
	}

	// Must have written data to the new triangle buffer
	changedTriangleBuffer = false;
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

	// Give a signal that the triangle buffer has changed and data needs to be written to it
	changedTriangleBuffer = true;
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

void Scene::markSelected(ObjectType objectType, unsigned int objectIndex)
{
	switch (objectType)
	{
		case NONE:
			// No object was selected: unselect all and stop
			currentlySelectedObject = nullptr;
			return;

		case MODEL:
			// Model was selected
			currentlySelectedObject = &models[objectIndex];
			return;
		case SPHERE:
			// Sphere was selected
			currentlySelectedObject = &spheres[objectIndex];
			return;

		case POINT_LIGHT:
			// Point light was selected
			currentlySelectedObject = &pointLights[objectIndex];
			return;
		case DIRECTIONAL_LIGHT:
			// Directional light was selected
			currentlySelectedObject = &directionalLights[objectIndex];
			return;
		case AMBIENT_LIGHT:
			// Ambient light was selected
			currentlySelectedObject = &ambientLights[objectIndex];
			return;

		case MATERIAL:
			// Material was selected
			currentlySelectedObject = &materials[objectIndex];
			return;
	}
}

void Scene::drawCurrentlySelectedObjectInterface()
{
	// Can't draw selected object if there isn't one
	if (getSelectedObject() == nullptr)
	{
		return;
	}

	getSelectedObject()->drawInterface(*this);
}

ImGuiEditorInterface* Scene::getSelectedObject()
{
	return currentlySelectedObject;
}

void Scene::getSelectedObjectData(unsigned int* objectType, unsigned int* objectIndex)
{
	unsigned int currentObjectIndex{ 0 };
	for (Model& model : models)
	{
		if (&model == getSelectedObject())
		{
			*objectType = 1;
			*objectIndex = currentObjectIndex;
		}
		currentObjectIndex++;
	}

	currentObjectIndex = 0;
	for (Sphere& sphere : spheres)
	{
		if (&sphere == getSelectedObject())
		{
			*objectType = 2;
			*objectIndex = currentObjectIndex;
		}
		currentObjectIndex++;
	}
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
