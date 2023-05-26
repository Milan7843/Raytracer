#include "BVH.h"

BVH::BVH(const char* vertexShaderPath, const char* geometryShaderPath, const char* fragmentShaderPath)
	: bvhRenderShader(vertexShaderPath, fragmentShaderPath, geometryShaderPath)
{
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
}

BVH::~BVH()
{
	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
}

void BVH::draw(Scene& scene)
{
	bvhRenderShader.use();

	// View matrix
	glm::mat4 view = glm::mat4(1.0f);
	view = scene.getActiveCamera().getViewMatrix();
	bvhRenderShader.setMat4("view", view);

	// Projection matrix
	glm::mat4 projection;
	projection = scene.getActiveCamera().getProjectionMatrix();
	bvhRenderShader.setMat4("projection", projection);

	bvhRenderShader.setVector3("inputColor", glm::vec3(0.15, 0.92, 0.3));

	std::vector<BVHData> data;

	data.push_back(
		BVHData{
			glm::vec3(0.0f),
			glm::vec3(1.0f)
		}
	);
	data.push_back(
		BVHData{
			glm::vec3(0.0f),
			glm::vec3(0.5f)
		}
	);

	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(BVHData) * data.size(), data.data(), GL_STATIC_DRAW);

	// Specifying the vertex attributes
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(BVHData), (void*)0);

	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(BVHData), (void*)sizeof(glm::vec3));

	glDrawArrays(GL_POINTS, 0, data.size());

	glBindVertexArray(0);
}
