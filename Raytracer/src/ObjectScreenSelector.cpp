#include "ObjectScreenSelector.h"

ObjectScreenSelector::ObjectScreenSelector(unsigned int width, unsigned int height)
	: width(width)
	, height(height)
	, objectColorShader("src/shader_src/solidColorVertexShader.shader", "src/shader_src/objectClickColoringShader.shader")
	, textureRenderShader("src/shader_src/raymarchVertexShader.shader", "src/shader_src/screenTextureFragment.shader")
{
	setup();
}

ObjectScreenSelector::~ObjectScreenSelector()
{
}

unsigned int ObjectScreenSelector::checkObjectClicked(Scene& scene, double x, double y)
{
	renderSceneToTexture(scene);

	// Setting up for reading data
	glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
	glReadBuffer(GL_COLOR_ATTACHMENT0);

	// Reading data
	std::vector<unsigned char> colorData(4);
	glReadPixels((unsigned int)x, height-(unsigned int)y, 1, 1, GL_RGBA, GL_UNSIGNED_BYTE, colorData.data());

	// Getting data
	unsigned int objectID{ (unsigned int)colorData[0] };

	// Unbinding the frame buffer
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	// Only found an object if objectID is not 0
	return objectID;
}

void ObjectScreenSelector::renderTexturePreview(Scene& scene, unsigned int screenQuadVAO)
{
	renderSceneToTexture(scene);


	/* DRAWING THE TEXTURE */

	glBindVertexArray(screenQuadVAO);

	// Binding the shader which simply renders a texture over the entire screen
	textureRenderShader.use();

	// Binding the texure to render
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, objectClickTexture);
	textureRenderShader.setInt("textureToRender", 0);

	// Draw call for the quad
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

	glBindBuffer(GL_VERTEX_ARRAY, 0);
}

void ObjectScreenSelector::setup()
{
	glGenFramebuffers(1, &framebuffer);
	glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);

	glGenTextures(1, &objectClickTexture);


	// Setting up the texture
	glBindTexture(GL_TEXTURE_2D, objectClickTexture);

	// Making it an empty image
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0);

	// Setting texture filter settings
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

	// Generate and bind the depth buffer
	unsigned int depthBuffer;
	glGenRenderbuffers(1, &depthBuffer);
	glBindRenderbuffer(GL_RENDERBUFFER, depthBuffer);

	// Set the storage for the depth buffer
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, width, height);

	// Attach the depth buffer to the framebuffer
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthBuffer);

	// Binding the texture to be drawn to
	glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, objectClickTexture, 0);

	// Set the list of draw buffers (only draw to the first slot
	GLenum drawBuffers[1] = { GL_COLOR_ATTACHMENT0 };
	glDrawBuffers(1, drawBuffers);


	// Unbinding the frame buffer
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void ObjectScreenSelector::renderSceneToTexture(Scene& scene)
{
	glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
	
	// Clearing the previous render
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glClear(GL_COLOR_BUFFER_BIT);

	glBindTexture(GL_TEXTURE_2D, objectClickTexture);

	/* DRAWING TO THE TEXTURE */

	objectColorShader.use();

	// View matrix
	glm::mat4 view = glm::mat4(1.0f);
	view = scene.getActiveCamera().getViewMatrix();
	objectColorShader.setMat4("view", view);

	// Projection matrix
	glm::mat4 projection;
	projection = scene.getActiveCamera().getProjectionMatrix();
	objectColorShader.setMat4("projection", projection);

	// Drawing the objects
	scene.draw(&objectColorShader);

	// Stop using our custom framebuffer to write to the screen again
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}
