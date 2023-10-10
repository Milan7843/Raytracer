#include "GizmoRenderer.h"

GizmoRenderer::GizmoRenderer(const char* vertexShaderPath,
	const char* geometryShaderPath,
	const char* fragmentShaderPath)
	: shader(vertexShaderPath, fragmentShaderPath, geometryShaderPath)
{
	for (unsigned int i{ 0 }; i < gizmos.size(); i++)
	{
		// Loading every gizmo's texture
		gizmos[i].textureId = loadImage(gizmos[i].texturePath);
	}

	// Generating the required buffers
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);

	// Making sure everything gets put on this specific VAO
	glBindVertexArray(VAO);

	// Binding the buffer
	glBindBuffer(GL_ARRAY_BUFFER, VBO);

	// Letting OpenGL know how to interpret the data:
	// vec3 for position
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	// vec3 for color
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	// Unbinding
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

GizmoRenderer::~GizmoRenderer()
{
	glDeleteBuffers(1, &VBO);
	glDeleteVertexArrays(1, &VAO);
}

void GizmoRenderer::render(std::vector<GizmoDrawData>& points, GizmoType type, Camera& camera)
{
	Gizmo gizmo = gizmos[(unsigned int)type];

	shader.use();


	// View matrix
	glm::mat4 view{ camera.getViewMatrix() };
	shader.setMat4("view", view);

	// Projection matrix
	glm::mat4 projection{ camera.getProjectionMatrix() };
	shader.setMat4("projection", projection);

	shader.setInt("gizmoTexture", 0);
	shader.setFloat("gizmoSize", gizmo.gizmoSize);
	shader.setFloat("aspectRatio", camera.getAspectRatio());

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, gizmo.textureId);

	writeDataToVAO(points);

	glBindVertexArray(VAO);

	glDrawArrays(GL_POINTS, 0, points.size());

	glBindVertexArray(0);
}

unsigned int GizmoRenderer::loadImage(std::string imageName)
{
	std::string fileName = "src/Textures/" + imageName;

	unsigned int textureID{ 0 };

	try
	{
		textureID = ImageLoader::loadImage(fileName);
	}
	catch (const std::exception& e)
	{
		Logger::logError("Failed to load Gizmo Texture: " + fileName + ".\nReason: " + e.what());
		return 0;
	}

	return textureID;
}

void GizmoRenderer::writeDataToVAO(std::vector<GizmoDrawData>& data)
{
	// Making sure everything gets put on this specific VAO
	glBindVertexArray(VAO);

	// Binding the buffer
	glBindBuffer(GL_ARRAY_BUFFER, VBO);

	// Putting the draw data into the buffer
	glBufferData(GL_ARRAY_BUFFER,
		data.size() * sizeof(data),
		data.data(),
		GL_STATIC_DRAW);

	// Unbinding
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}
