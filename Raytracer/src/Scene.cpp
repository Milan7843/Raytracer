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

void Scene::deleteObject(unsigned int id)
{
	deletePointLight(id);
	deleteDirectionalLight(id);
	deleteAmbientLight(id);
	deleteModel(id);
	deleteSphere(id);
	deleteMaterial(id);
}

bool Scene::deletePointLight(unsigned int id)
{
	for (unsigned int i = 0; i < pointLights.size(); i++)
	{
		if (pointLights[i].getID() == id)
		{
			// A light with the given ID was found:
			// Now erase it from the list and recalculate the shader indices
			pointLights.erase(pointLights.begin() + i);
			this->pointLightCount--;
			recalculatePointLightIndices();

			onObjectDeleted(id);
			return true;
		}
	}

	// No light with the given ID was found
	return false;
}

bool Scene::deleteDirectionalLight(unsigned int id)
{
	for (unsigned int i = 0; i < directionalLights.size(); i++)
	{
		if (directionalLights[i].getID() == id)
		{
			// A light with the given ID was found:
			// Now erase it from the list and recalculate the shader indices
			directionalLights.erase(directionalLights.begin() + i);
			this->directionalLightCount--;
			recalculateDirectionalLightIndices();

			onObjectDeleted(id);
			return true;
		}
	}

	// No light with the given ID was found
	return false;
}

bool Scene::deleteAmbientLight(unsigned int id)
{
	for (unsigned int i = 0; i < ambientLights.size(); i++)
	{
		if (ambientLights[i].getID() == id)
		{
			// A light with the given ID was found:
			// Now erase it from the list and recalculate the shader indices
			ambientLights.erase(ambientLights.begin() + i);
			this->ambientLightCount--;
			recalculateAmbientLightIndices();

			onObjectDeleted(id);
			return true;
		}
	}

	// No light with the given ID was found
	return false;
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

	// Making sure the meshes' parent model pointers are still valid after resizing
	for (Model& model : models)
	{
		model.updateChildPointers();
	}

	return &(models[models.size() - 1]);
}

Model* Scene::addModel(const std::string& path, unsigned int materialIndex)
{
	Model model(materialIndex, path, &meshCount, &triangleCount, MAX_MESH_COUNT);
	models.push_back(model);

	// Making sure the meshes' parent model pointers are still valid after resizing
	for (Model& model : models)
	{
		model.updateChildPointers();
	}

	return &(models[models.size() - 1]);
}

bool Scene::deleteModel(unsigned int id)
{
	for (unsigned int i = 0; i < models.size(); i++)
	{
		if (models[i].getID() == id)
		{
			// A model with the given ID was found:
			// Now erase it from the list and recalculate the shader indices
			models.erase(models.begin() + i);

			recalculateModelIndices();

			onObjectDeleted(id);

			// Making sure the meshes' parent model pointers are still valid after resizing
			for (Model& model : models)
			{
				model.updateChildPointers();
			}

			return true;
		}
	}

	// No model with the given ID was found
	return false;
}

Model* Scene::getModelByID(unsigned int id)
{
	for (Model& model : models)
	{
		if (model.getID() == id)
		{
			return &model;
		}
	}

	return nullptr;
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

bool Scene::deleteSphere(unsigned int id)
{
	for (unsigned int i = 0; i < spheres.size(); i++)
	{
		if (spheres[i].getID() == id)
		{
			// A model with the given ID was found:
			// Now erase it from the list and recalculate the shader indices
			spheres.erase(spheres.begin() + i);
			// Then making sure the sphere indices are still contiguous
			recalculateSphereIndices();

			onObjectDeleted(id);
			return true;
		}
	}

	// No sphere with the given ID was found
	return false;
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

bool Scene::deleteMaterial(unsigned int id)
{
	for (unsigned int i = 1; i < materials.size(); i++)
	{
		if (materials[i].getID() == id)
		{
			// A material with the given ID was found

			// If there were more materials: delete the one at the index
			this->materials.erase(materials.begin() + i);

			// Then verifying each model's material indices
			for (Sphere& sphere : getSpheres())
			{
				sphere.onDeleteMaterial(i);
			}

			for (Model& model : getModels())
			{
				model.onDeleteMaterial(i);
			}

			onObjectDeleted(id);
			return true;
		}
	}

	// No material with the given ID was found
	return false;
}

void Scene::recalculateModelIndices()
{
	this->triangleCount = 0;

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
	glEnable(GL_DEPTH_TEST);
	glClear(GL_DEPTH_BUFFER_BIT);
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

	for (Model& model : models)
	{
		shader->setInt("objectID", model.getID());

		model.draw(shader, (Scene*)this);
	}

	for (Sphere& sphere : spheres)
	{
		shader->setInt("objectID", sphere.getID());
		sphere.draw(shader, (Scene*)this);
	}
}

void Scene::drawSelected(AbstractShader* shader)
{
	// Drawing each model with the given shader if it is selected
	for (Model& model : models)
	{
		if (model.getID() == currentlySelectedObject)
			model.draw(shader, (Scene*)this);

		for (Mesh& mesh : model.getMeshes())
		{
			if (mesh.getID() == currentlySelectedObject)
			{
				model.prepareForDraw(shader);
				glm::mat4 transformation{ model.getTransformationMatrix() };
				mesh.draw(shader, (Scene*)this, transformation);
			}
		}
	}

	// Drawing each sphere with the given shader if it is selected
	for (Sphere& sphere : spheres)
	{
		if (sphere.getID() == currentlySelectedObject)
			sphere.draw(shader, (Scene*)this);
	}
}

bool Scene::hasObjectSelected()
{
	return currentlySelectedObject != 0;
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
			//Logger::log(std::to_string(changedTriangleBuffer) + " model updated");
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
	std::vector<ShaderMesh> shaderMeshes{};

	// Activate the shader program so that uniforms can be set
	shader->use();

	// Writing current sphere count to shader
	shader->setInt("sphereCount", sphereCount);

	// Updating each model as needed
	for (Model& model : models)
	{
		model.writeToShader(shader, triangleBufferSSBO);

		glm::mat4 transformation = model.getTransformationMatrix();

		// Writing all meshes to the vector
		for (Mesh& mesh : model.getMeshes())
		{
			ShaderMesh shaderMesh{
				transformation * mesh.getTransformationMatrix(),
				mesh.getMaterialIndex()
			};

			shaderMeshes.push_back(shaderMesh);
		}
	}

	// Updating each sphere as needed
	for (Sphere& sphere : spheres)
	{
		sphere.writeToShader(shader, triangleBufferSSBO);
	}

	updateBVH();

	// Must have written data to the new triangle buffer
	changedTriangleBuffer = false;

	generateMeshBuffer(shaderMeshes);
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

void Scene::onObjectDeleted(unsigned int id)
{
	// If the selected object was deleted, unselect the object.
	if (currentlySelectedObject == id)
	{
		currentlySelectedObject = 0;
	}
}

void Scene::generateMeshBuffer(std::vector<ShaderMesh>& shaderMeshes)
{
	// Generate the buffer if it didn't exist yet
	if (meshBufferSSBO == 0)
	{
		glGenBuffers(1, &meshBufferSSBO);
	}
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, meshBufferSSBO);

	// Loading zero-data into the new buffer
	glBufferData(GL_SHADER_STORAGE_BUFFER, shaderMeshes.size() * sizeof(ShaderMesh), &shaderMeshes[0], GL_STATIC_DRAW);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 8, meshBufferSSBO);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

	Logger::log("Making " + std::to_string(shaderMeshes.size() * sizeof(ShaderMesh)) + " bytes in SSBO " + std::to_string(meshBufferSSBO));
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

void Scene::markSelected(unsigned int objectID)
{
	if (objectID == 0)
	{
		// Deselect
		currentlySelectedObject = objectID;
		return;
	}

	// Getting a pointer to the selected object
	ImGuiEditorInterface* newlySelectedObject{ getImGuiEditorInterfaceByID(objectID) };
	if (newlySelectedObject->getType() == MESH)
	{
		Mesh* selectedMesh{ dynamic_cast<Mesh*>(newlySelectedObject) };
		unsigned int parentModelID = selectedMesh->getModelID();

		// If the parent model has only one mesh, always select the model
		Model* parentModel{ getModelByID(parentModelID) };
		if (parentModel->getMeshes().size() <= 1)
		{
			objectID = parentModelID;
		}
		// A model with >1 submeshes was selected
		else
		{
			// If we select the same mesh again, now select the parent model
			if (objectID == currentlySelectedObject)
			{
				objectID = parentModelID;
			}
			else
			{
				// Select the mesh
			}
		}
	}

	currentlySelectedObject = objectID;

	Object* object{ getObjectFromSelected() };
	if (object != nullptr)
	{
		rotationPoint = object->getRotationPoint();
	}
}

void Scene::drawCurrentlySelectedObjectInterface()
{
	ImGuiEditorInterface* selectedObject{ getSelectedObject() };
	if (selectedObject == nullptr)
	{
		// No object selected
		return;
	}

	selectedObject->drawInterface(*this);
}

ImGuiEditorInterface* Scene::getSelectedObject()
{
	return getImGuiEditorInterfaceByID(currentlySelectedObject);
}

ContextMenuSource* Scene::getContextMenuSourceFromSelected()
{
	if (!hasObjectSelected())
	{
		return nullptr;
	}

	ImGuiEditorInterface* selectedObject{ getSelectedObject() };
	ObjectType type{ selectedObject->getType() };

	// If the newly selected object was the same mesh again, now we select the object instead
	// TODO add supported context menus here
	if (type != MODEL && type != MESH)
	{
		return nullptr;
	}

	return dynamic_cast<ContextMenuSource*>(selectedObject);
}

Object* Scene::getObjectFromSelected()
{
	return getObjectByID(currentlySelectedObject);
}

Object* Scene::getObjectByID(unsigned int objectID)
{
	if (!hasObjectSelected())
	{
		return nullptr;
	}

	ImGuiEditorInterface* selectedObject{ getSelectedObject() };
	ObjectType type{ selectedObject->getType() };

	// If the newly selected object was the same mesh again, now we select the object instead
	// TODO add supported context menus here
	if (type != MODEL && type != MESH)
	{
		return nullptr;
	}

	return dynamic_cast<Object*>(selectedObject);
}

ObjectType Scene::getSelectedObjectType()
{
	ImGuiEditorInterface* selectedObject{ getSelectedObject() };
	if (selectedObject == nullptr)
	{
		return ObjectType::NONE;
	}
	ObjectType type{ selectedObject->getType() };
	return type;
}

ContextMenuSource* Scene::getContextMenuSourceByID(unsigned int objectID)
{
	for (Model& model : models)
	{
		if (model.getID() == objectID)
		{
			return &model;
		}
		for (Mesh& mesh : model.getMeshes())
		{
			if (mesh.getID() == objectID)
			{
				return &mesh;
			}
		}
	}

	for (Sphere& sphere : spheres)
	{
		if (sphere.getID() == objectID)
		{
			return &sphere;
		}
	}

	// Checking all lights
	/*
	for (PointLight& light : pointLights)
	{
		if (light.getID() == objectID)
		{
			return &light;
		}
	}
	for (DirectionalLight& light : directionalLights)
	{
		if (light.getID() == objectID)
		{
			return &light;
		}
	}
	for (AmbientLight& light : ambientLights)
	{
		if (light.getID() == objectID)
		{
			return &light;
		}
	}

	// Checking all materials
	for (Material& material : materials)
	{
		if (material.getID() == objectID)
		{
			return &material;
		}
	}*/

	// No matching object was found
	return nullptr;
}

ImGuiEditorInterface* Scene::getImGuiEditorInterfaceByID(unsigned int objectID)
{
	// TODO optimize not having to iterate over all objects every time this function is called by
	// storing the pointer and only updating on a `markSelected` call

	for (Model& model : models)
	{
		if (model.getID() == objectID)
		{
			return &model;
		}
		for (Mesh& mesh : model.getMeshes())
		{
			if (mesh.getID() == objectID)
			{
				return &mesh;
			}
		}
	}

	for (Sphere& sphere : spheres)
	{
		if (sphere.getID() == objectID)
		{
			return &sphere;
		}
	}

	// Checking all lights
	for (PointLight& light : pointLights)
	{
		if (light.getID() == objectID)
		{
			return &light;
		}
	}
	for (DirectionalLight& light : directionalLights)
	{
		if (light.getID() == objectID)
		{
			return &light;
		}
	}
	for (AmbientLight& light : ambientLights)
	{
		if (light.getID() == objectID)
		{
			return &light;
		}
	}

	// Checking all materials
	for (Material& material : materials)
	{
		if (material.getID() == objectID)
		{
			return &material;
		}
	}

	return nullptr;
}

void Scene::renderContextMenus()
{
	for (Model& model : models)
	{
		model.renderContextMenu(*this);
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

void Scene::updateBVH()
{
	bvh.updateByScene(*this);
}

BVH& Scene::getBVH()
{
	return bvh;
}

glm::vec3 Scene::getRotationPoint()
{
	return rotationPoint;
}

bool Scene::replace(std::string& str, const std::string& from, const std::string& to)
{
	size_t start_pos = str.find(from);
	if (start_pos == std::string::npos)
		return false;
	str.replace(start_pos, from.length(), to);
	return true;
}
