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
	float gizmoScreenSize;
	float gizmoWorldSize;
};

struct GizmoDrawData
{
	glm::vec3 position;
	glm::vec3 color;
};

struct GizmoClickSelectDrawData
{
	glm::vec3 position;
	unsigned int objectID;
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

	void render(std::vector<GizmoClickSelectDrawData>& data, GizmoType type, Camera& camera);

private:
	std::vector<Gizmo> gizmos {
		// GizmoType::POINTLIGHT
		Gizmo {
			"point_light.png",
			0,
			0.08f,
			0.2f
		}
	};
	Shader shader;

	unsigned int regularVAO{ 0 };
	unsigned int regularVBO{ 0 };

	unsigned int clickSelectVAO{ 0 };
	unsigned int clickSelectVBO{ 0 };

	unsigned int loadImage(std::string imageName);

	void writeDataToVAO(std::vector<GizmoDrawData>& data);
	void writeDataToVAO(std::vector<GizmoClickSelectDrawData>& data);
};