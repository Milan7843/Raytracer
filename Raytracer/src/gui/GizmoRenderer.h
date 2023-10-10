#pragma once
#include <string>
#include <vector>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>

#include "../Logger.h"
#include "../Camera.h"
#include "../shaders/Shader.h"
#include "../ImageLoader.h"

struct Gizmo
{
	std::string texturePath;
	unsigned int textureId;
	float gizmoSize;
};

struct GizmoDrawData
{
	glm::vec3 position;
	glm::vec3 color;
};

enum class GizmoType
{
	POINTLIGHT
};

class GizmoRenderer
{
public:
	GizmoRenderer(const char* vertexShaderPath,
		const char* geometryShaderPath,
		const char* fragmentShaderPath);
	~GizmoRenderer();

	void render(std::vector<GizmoDrawData>& data, GizmoType type, Camera& camera);

private:
	std::vector<Gizmo> gizmos {
		// GizmoType::POINTLIGHT
		Gizmo {
			"point_light.png",
			0,
			0.05f
		}
	};
	Shader shader;

	unsigned int VAO{ 0 };
	unsigned int VBO{ 0 };

	unsigned int loadImage(std::string imageName);

	void writeDataToVAO(std::vector<GizmoDrawData>& data);
};