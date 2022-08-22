#include "OutlineRenderer.h"

OutlineRenderer::OutlineRenderer(unsigned int width, unsigned int height, unsigned int screenQuadVAO)
	: width(width)
	, height(height)
	, screenQuadVAO(screenQuadVAO)
	, blurRenderShader("src/Shaders/raymarchVertexShader.shader", "src/Shaders/screenTextureFragment.shader")
	, objectRenderShader("src/Shaders/solidColorVertexShader.shader", "src/Shaders/outlineRenderToTextureFragment.shader")
	, textureBlurrerShader("src/Shaders/blurComputeShader.shader")
{
	setup();
}

OutlineRenderer::~OutlineRenderer()
{
}

void OutlineRenderer::setup()
{
	glGenFramebuffers(1, &framebuffer);
	glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);

	glGenTextures(1, &sharpTexture);
	glGenTextures(1, &blurTexture);


	// First modify the sharp texture
	glBindTexture(GL_TEXTURE_2D, sharpTexture);

	// Making it an empty image
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, textureWidth, textureHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0);

	// Setting texture filter settings
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);


	// Then modify the blur texture
	glBindTexture(GL_TEXTURE_2D, blurTexture);

	// Making it an empty image
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, textureWidth, textureHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0);

	// Setting texture filter settings
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);


	// Binding the textures to be drawn to
	glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, sharpTexture, 0);
	//glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, blurTexture, 0);

	// Set the list of draw buffers
	//GLenum drawBuffers[2] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1 };
	//glDrawBuffers(2, drawBuffers);
	GLenum drawBuffers[1] = { GL_COLOR_ATTACHMENT0 };
	glDrawBuffers(1, drawBuffers);


	// Unbinding the frame buffer
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void OutlineRenderer::render(Scene& scene)
{
	glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);



	/* DRAWING TO THE TEXTURES */

	// Setting viewport size
	glViewport(0, 0, textureWidth, textureHeight);

	objectRenderShader.use();

	// View matrix
	glm::mat4 view = glm::mat4(1.0f);
	view = scene.getActiveCamera().getViewMatrix();
	objectRenderShader.setMat4("view", view);

	// Projection matrix
	glm::mat4 projection;
	projection = scene.getActiveCamera().getProjectionMatrix(textureWidth, textureHeight);
	objectRenderShader.setMat4("projection", projection);

	// Drawing the objects
	scene.drawSelected(&objectRenderShader);
	
	// Stop using our custom framebuffer to write to the screen again
	glBindFramebuffer(GL_FRAMEBUFFER, 0);



	/* BLURRING */


	// Binding the correct sharp texture
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, sharpTexture);

	// And the to-be-blurred texture
	glBindImageTexture(1, blurTexture, 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_RGBA8);

	textureBlurrerShader.use();
	textureBlurrerShader.setInt("originalTexture", 0);
	textureBlurrerShader.setInt("blurredTexture", 1);
	textureBlurrerShader.setFloat("width", 0);
	textureBlurrerShader.setFloat("height", 0);

	glDispatchCompute(textureWidth, textureHeight, 1);
	glMemoryBarrier(GL_ALL_BARRIER_BITS);


	/* DRAWING BLUR */

	glBindVertexArray(screenQuadVAO);

	// Binding the shader which simply renders a texture over the entire screen
	blurRenderShader.use();

	// Binding the texure to render
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, blurTexture);
	blurRenderShader.setInt("textureToRender", 0);

	// Draw call for the quad
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

	glBindBuffer(GL_VERTEX_ARRAY, 0);
}