#pragma once

#include "nlohmann/json.hpp"

#include "Scene.h"

typedef nlohmann::json json;

namespace JSONUtility
{
	void loadModels(json data, Scene& scene);
	void loadSpheres(json data, Scene& scene);
	void loadPointLights(json data, Scene& scene);
	void loadDirectionalLights(json data, Scene& scene);
	void loadAmbientLights(json data, Scene& scene);
	void loadMaterials(json data, Scene& scene);
	void loadCamera(json data, Scene& scene);
	void loadHDRI(json data, Scene& scene);
	void loadSettings(json data, Scene& scene);

	json toJSON(Model& model);
	json toJSON(Mesh& mesh);
	json toJSON(std::vector<Mesh>& meshes);
	json toJSON(Sphere& sphere);
	json toJSON(AmbientLight& light);
	json toJSON(PointLight& light);
	json toJSON(DirectionalLight& light);
	json toJSON(std::vector<Material>& materials);
	json toJSON(const Material& material);
	json toJSON(Camera& camera);
	json toJSON(std::vector<Model>& models);
	json toJSON(std::vector<Sphere>& spheres);
	json toJSON(std::vector<AmbientLight>& lights);
	json toJSON(std::vector<PointLight>& lights);
	json toJSON(std::vector<DirectionalLight>& lights);
	json toJSON(Scene& scene);
};

