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
	glGenVertexArrays(1, &regularVAO);
	glGenBuffers(1, &regularVBO);

	// Making sure everything gets put on this specific VAO
	glBindVertexArray(regularVAO);

	// Binding the buffer
	glBindBuffer(GL_ARRAY_BUFFER, regularVBO);

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



	// Generating the required buffers
	glGenVertexArrays(1, &clickSelectVAO);
	glGenBuffers(1, &clickSelectVBO);

	// Making sure everything gets put on this specific VAO
	glBindVertexArray(clickSelectVAO);

	// Binding the buffer
	glBindBuffer(GL_ARRAY_BUFFER, clickSelectVBO);

	// Letting OpenGL know how to interpret the data:
	// vec3 for position
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	// int for objectID
	glVertexAttribPointer(1, 1, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	// Unbinding
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

GizmoRenderer::~GizmoRenderer()
{
	glDeleteBuffers(1, &regularVBO);
	glDeleteVertexArrays(1, &regularVAO);
	glDeleteBuffers(1, &clickSelectVBO);
	glDeleteVertexArrays(1, &clickSelectVAO);
}

void GizmoRenderer::render(std::vector<GizmoDrawData>& data, GizmoType type, Camera& camera)
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
	shader.setFloat("gizmoScreenSize", gizmo.gizmoScreenSize);
	shader.setFloat("gizmoWorldSize", gizmo.gizmoWorldSize);
	shader.setFloat("aspectRatio", camera.getAspectRatio());

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, gizmo.textureId);

	writeDataToVAO(data);

	glBindVertexArray(regularVAO);

	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_BLEND);

	glDrawArrays(GL_POINTS, 0, data.size());

	glBindVertexArray(0);
}

void GizmoRenderer::render(std::vector<GizmoClickSelectDrawData>& data, GizmoType type, Camera& camera)
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
	shader.setFloat("gizmoScreenSize", gizmo.gizmoScreenSize);
	shader.setFloat("gizmoWorldSize", gizmo.gizmoWorldSize);
	shader.setFloat("aspectRatio", camera.getAspectRatio());

	writeDataToVAO(data);

	glBindVertexArray(clickSelectVAO);

	glDrawArrays(GL_POINTS, 0, data.size());

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
	glBindVertexArray(regularVAO);

	// Binding the buffer
	glBindBuffer(GL_ARRAY_BUFFER, regularVBO);

	// Putting the draw data into the buffer
	glBufferData(GL_ARRAY_BUFFER,
		data.size() * sizeof(data),
		data.data(),
		GL_STATIC_DRAW);

	// Unbinding
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

void GizmoRenderer::writeDataToVAO(std::vector<GizmoClickSelectDrawData>& data)
{
	// Making sure everything gets put on this specific VAO
	glBindVertexArray(clickSelectVAO);

	// Binding the buffer
	glBindBuffer(GL_ARRAY_BUFFER, clickSelectVBO);

	// Putting the draw data into the buffer
	glBufferData(GL_ARRAY_BUFFER,
		data.size() * sizeof(data),
		data.data(),
		GL_STATIC_DRAW);

	// Unbinding
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}
