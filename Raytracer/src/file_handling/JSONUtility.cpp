#include "JSONUtility.h"

namespace JSONUtility
{
	namespace
	{
		json vec3ToJSON(glm::vec3 vec)
		{
			json j{ json::array({ vec.x, vec.y, vec.z }) };
			return j;
		}

		glm::vec3 jsonToVec3(json j)
		{
			return glm::vec3(j[0], j[1], j[2]);
		}
	}

	void loadModels(json data, Scene& scene)
	{
		if (!data.contains("models"))
		{
			return;
		}

		// Assuming JSON structure is an array of objects where each object represents a model
		for (const json& modelData : data["models"])
		{
			// Extract relevant information from JSON and create a Model object
			std::string path = modelData["path"];
			std::string name = modelData["name"];

			Model* model = scene.addModel(name, path);

			// If position data is available
			if (modelData.contains("position"))
			{
				json positionData = modelData["position"];
				glm::vec3 position(
					positionData[0],
					positionData[1],
					positionData[2]
				);

				model->setPosition(position);
			}

			// If rotation data is available
			if (modelData.contains("rotation"))
			{
				json rotationData = modelData["rotation"];
				glm::vec3 rotation(
					rotationData[0],
					rotationData[1],
					rotationData[2]
				);

				model->setRotation(rotation);
			}

			// If scale data is available
			if (modelData.contains("scale"))
			{
				json scaleData = modelData["scale"];
				glm::vec3 scale(
					scaleData[0],
					scaleData[1],
					scaleData[2]
				);

				model->scale(scale);
			}

			// Reading submesh data
			if (modelData.contains("submeshdata"))
			{
				unsigned int index{ 0 };
				for (json meshData : modelData["submeshdata"])
				{
					model->getMeshes()[index].setName(meshData["name"]);
					model->getMeshes()[index].move(jsonToVec3(meshData["offsetPosition"]));
					model->getMeshes()[index].rotate(jsonToVec3(meshData["rotation"]));
					model->getMeshes()[index].scale(jsonToVec3(meshData["scale"]));
					model->getMeshes()[index].setMaterialIndex(meshData["materialIndex"]);
					index++;
				}
			}
		}
	}

	void loadSpheres(json data, Scene& scene)
	{
		if (!data.contains("spheres"))
		{
			return;
		}

		// Assuming JSON structure is an array of objects where each object represents a model
		for (const json& sphereData : data["spheres"])
		{
			// Extract relevant information from JSON and create a Model object
			std::string path = sphereData["path"];

			unsigned int materialIndex = sphereData["materialIndex"];

			Sphere* sphere = scene.addSphere(materialIndex);

			// If position data is available
			if (sphereData.contains("position"))
			{
				json positionData = sphereData["position"];
				glm::vec3 position(
					positionData[0],
					positionData[1],
					positionData[2]
				);

				sphere->setPosition(position);
			}

			// If rotation data is available
			if (sphereData.contains("radius"))
			{
				float radius = sphereData["radius"];

				sphere->setRadius(radius);
			}
		}
	}

	void loadPointLights(json data, Scene& scene)
	{
		if (!data.contains("pointlights"))
		{
			return;
		}

		for (const json& lightData : data["pointlights"])
		{
			PointLight* pointLight = scene.addPointLight();

			if (pointLight == nullptr)
			{
				// No more new point lights can be added
				return;
			}

			if (lightData.contains("name"))
			{
				std::string name = lightData["name"];
				pointLight->setName(name);
			}

			if (lightData.contains("color"))
			{
				json colorData = lightData["color"];
				glm::vec3 color(
					colorData[0],
					colorData[1],
					colorData[2]
				);
				pointLight->setColor(color);
			}

			if (lightData.contains("position"))
			{
				json positionData = lightData["position"];
				glm::vec3 position(
					positionData[0],
					positionData[1],
					positionData[2]
				);
				pointLight->setPosition(position);
			}

			if (lightData.contains("intensity"))
			{
				float intensity = lightData["intensity"];
				pointLight->setIntensity(intensity);
			}
		}
	}

	void loadDirectionalLights(json data, Scene& scene)
	{
		if (!data.contains("directionallights"))
		{
			return;
		}

		for (const json& lightData : data["directionallights"])
		{
			DirectionalLight* directionalLight = scene.addDirectionalLight();

			if (directionalLight == nullptr)
			{
				// No more new directional lights can be added
				return;
			}

			if (lightData.contains("name"))
			{
				std::string name = lightData["name"];
				directionalLight->setName(name);
			}

			if (lightData.contains("direction"))
			{
				json directionData = lightData["direction"];
				glm::vec3 direction(
					directionData[0],
					directionData[1],
					directionData[2]
				);
				directionalLight->setDirection(direction);
			}

			if (lightData.contains("intensity"))
			{
				float intensity = lightData["intensity"];
				directionalLight->setIntensity(intensity);
			}
		}
	}

	void loadAmbientLights(json data, Scene& scene)
	{
		if (!data.contains("ambientlights"))
		{
			return;
		}

		for (const json& lightData : data["ambientlights"])
		{
			AmbientLight* ambientLight = scene.addAmbientLight();

			if (ambientLight == nullptr)
			{
				// No more new ambient lights can be added
				return;
			}

			if (lightData.contains("name"))
			{
				std::string name = lightData["name"];
				ambientLight->setName(name);
			}

			if (lightData.contains("color"))
			{
				json colorData = lightData["color"];
				glm::vec3 color(
					colorData[0],
					colorData[1],
					colorData[2]
				);
				ambientLight->setColor(color);
			}

			if (lightData.contains("intensity"))
			{
				float intensity = lightData["intensity"];
				ambientLight->setIntensity(intensity);
			}
		}
	}

	void loadMaterials(json data, Scene& scene)
	{
		if (!data.contains("materials"))
		{
			return;
		}

		for (const json& materialData : data["materials"])
		{
			Material material;

			if (materialData.contains("name"))
			{
				std::string name = materialData["name"];
				material.setName(name);
			}

			if (materialData.contains("color"))
			{
				json data = materialData["color"];
				material.setColor(jsonToVec3(data));
			}

			if (materialData.contains("reflectiveness"))
			{
				json data = materialData["reflectiveness"];
				material.setReflectiveness(data);
			}

			if (materialData.contains("transparency"))
			{
				json data = materialData["transparency"];
				material.setTransparency(data);
			}

			if (materialData.contains("refractiveness"))
			{
				json data = materialData["refractiveness"];
				material.setRefractiveness(data);
			}

			if (materialData.contains("reflectionDiffusion"))
			{
				json data = materialData["reflectionDiffusion"];
				material.setReflectionDiffusion(data);
			}

			if (materialData.contains("emission"))
			{
				json data = materialData["emission"];
				material.setEmission(jsonToVec3(data));
			}

			if (materialData.contains("emissionStrength"))
			{
				json data = materialData["emissionStrength"];
				material.setEmissionStrength(data);
			}

			if (materialData.contains("fresnelReflectionStrength"))
			{
				json data = materialData["fresnelReflectionStrength"];
				material.setFresnelReflectionStrength(data);
			}

			scene.addMaterial(material);
		}
	}

	void loadCamera(json data, Scene& scene)
	{
		Camera camera;

		if (data.contains("camera"))
		{
			json cameraData = data["camera"];

			if (cameraData.contains("position"))
			{
				json positionData = cameraData["position"];
				glm::vec3 position(
					positionData[0],
					positionData[1],
					positionData[2]
				);
				camera.setPosition(position);
			}

			if (cameraData.contains("yaw"))
			{
				camera.setYaw(cameraData["yaw"]);
			}

			if (cameraData.contains("pitch"))
			{
				camera.setPitch(cameraData["pitch"]);
			}
		}

		scene.addCamera(camera);
	}

	void loadHDRI(json data, Scene& scene)
	{
		if (!data.contains("hdri"))
		{
			return;
		}

		scene.loadHDRI(data["hdri"]);
	}


	json toJSON(Model& model)
	{
		json j;
		j["path"] = model.getPath();
		j["name"] = model.getName();
		j["position"] = vec3ToJSON(model.getPosition());
		j["rotation"] = vec3ToJSON(model.getRotation());
		j["scale"] = vec3ToJSON(model.getScale());

		j["submeshdata"] = toJSON(model.getMeshes());
		
		return j;
	}

	json toJSON(const Mesh& mesh)
	{
		json j;
		j["name"] = mesh.getName();
		glm::vec3 offsetPosition{ mesh.getPosition() - mesh.getAverageVertexPosition() };
		j["offsetPosition"] = vec3ToJSON(offsetPosition);
		j["rotation"] = vec3ToJSON(mesh.getRotation());
		j["scale"] = vec3ToJSON(mesh.getScale());
		j["materialIndex"] = mesh.getMaterialIndex();

		return j;
	}

	json toJSON(std::vector<Mesh>& meshes)
	{
		std::vector<json> meshDatas;

		for (const Mesh& mesh : meshes)
		{
			json meshData = toJSON(mesh);

			meshDatas.push_back(meshData);
		}

		return meshDatas;
	}

	json toJSON(const Sphere& sphere)
	{
		json j;
		j["name"] = sphere.getName();
		j["position"] = vec3ToJSON(sphere.getPosition());
		j["radius"] = sphere.getRadius();
		j["materialIndex"] = sphere.getMaterialIndex();
		return j;
	}


	json toJSON(const AmbientLight& light)
	{
		json j;
		j["name"] = light.getName();
		j["color"] = vec3ToJSON(light.getColor());
		j["intensity"] = light.getIntensity();
		return j;
	}

	json toJSON(const PointLight& light)
	{
		json j;
		j["name"] = light.getName();
		j["color"] = vec3ToJSON(light.getColor());
		j["position"] = vec3ToJSON(light.getPosition());
		j["intensity"] = light.getIntensity();
		return j;
	}

	json toJSON(const DirectionalLight& light)
	{
		json j;
		j["name"] = light.getName();
		j["color"] = vec3ToJSON(light.getColor());
		j["direction"] = vec3ToJSON(light.getDirection());
		j["intensity"] = light.getIntensity();
		return j;
	}

	json toJSON(std::vector<Material>& materials)
	{
		json j;

		std::vector<json> materialsJSON;

		// Skipping error material, which is always in slot [0]
		bool pastErrorMaterial{ false };

		for (Material& material : materials)
		{
			if (!pastErrorMaterial)
			{
				pastErrorMaterial = true;
				continue;
			}

			materialsJSON.push_back(toJSON(material));
		}

		j = materialsJSON;

		return j;
	}

	json toJSON(const Material& material)
	{
		json j;
		j["name"] = material.getName();
		j["color"] = vec3ToJSON(material.getColor());
		j["reflectiveness"] = material.getReflectiveness();
		j["transparency"] = material.getTransparency();
		j["refractiveness"] = material.getRefractiveness();
		j["reflectionDiffusion"] = material.getReflectionDiffusion();
		j["emission"] = vec3ToJSON(material.getColor());
		j["emissionstrength"] = material.getEmissionStrength();
		j["fresnelReflectionStrength"] = material.getFresnelReflectionStrength();
		return j;
	}

	json toJSON(Camera& camera)
	{
		json j;
		j["position"] = vec3ToJSON(camera.getPosition());
		j["yaw"] = camera.getYaw();
		j["pitch"] = camera.getPitch();
		return j;
	}

	json toJSON(std::vector<Model>& models)
	{
		json j;

		std::vector<json> modelsJSON;

		for (Model& model : models)
		{
			modelsJSON.push_back(toJSON(model));
		}

		j = modelsJSON;

		return j;
	}

	json toJSON(std::vector<Sphere>& spheres)
	{
		json j;

		std::vector<json> spheresJSON;

		for (const Sphere& sphere : spheres)
		{
			spheresJSON.push_back(toJSON(sphere));
		}

		j = spheresJSON;

		return j;
	}

	json toJSON(std::vector<AmbientLight>& lights)
	{
		json j;

		std::vector<json> lightsJSON;

		for (const AmbientLight& light : lights)
		{
			lightsJSON.push_back(toJSON(light));
		}

		j = lightsJSON;

		return j;
	}

	json toJSON(std::vector<PointLight>& lights)
	{
		json j;

		std::vector<json> lightsJSON;

		for (const PointLight& light : lights)
		{
			lightsJSON.push_back(toJSON(light));
		}

		j = lightsJSON;

		return j;
	}

	json toJSON(std::vector<DirectionalLight>& lights)
	{
		json j;

		std::vector<json> lightsJSON;

		for (const DirectionalLight& light : lights)
		{
			lightsJSON.push_back(toJSON(light));
		}

		j = lightsJSON;

		return j;
	}

	json toJSON(Scene& scene)
	{
		json j;
		//j["name"] = scene.getName();

		j["models"] = toJSON(scene.getModels());
		j["spheres"] = toJSON(scene.getSpheres());

		j["materials"] = toJSON(scene.getMaterials());

		j["pointlights"] = toJSON(scene.getPointLights());
		j["directionallights"] = toJSON(scene.getDirectionalLights());
		j["ambientlights"] = toJSON(scene.getAmbientLights());

		j["camera"] = toJSON(scene.getActiveCamera());

		if (scene.hasHDRI())
		{
			j["hdri"] = scene.getHDRI()->path;
		}

		return j;
	}
};