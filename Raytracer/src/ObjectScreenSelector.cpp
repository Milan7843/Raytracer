#include "ObjectScreenSelector.h"

ObjectScreenSelector::ObjectScreenSelector(unsigned int width, unsigned int height)
	: objectColorShader("src/shader_src/solidColorVertexShader.shader", "src/shader_src/objectClickColoringShader.shader")
	, textureRenderShader("src/shader_src/raymarchVertexShader.shader", "src/shader_src/screenTextureFragment.shader")
{
	setResolution(width, height);
}

ObjectScreenSelector::~ObjectScreenSelector()
{
}

unsigned int ObjectScreenSelector::checkObjectClicked(Scene& scene, unsigned int x, unsigned int y, GizmoRenderer& objectClickGizmoRenderer)
{
	int framebufferBefore{ 0 };

	glGetIntegerv(GL_DRAW_FRAMEBUFFER_BINDING, &framebufferBefore);

	renderSceneToTexture(scene, objectClickGizmoRenderer);

	// Setting up for reading data
	glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
	glReadBuffer(GL_COLOR_ATTACHMENT0);

	// Reading data
	std::vector<unsigned char> colorData(4);
	glReadPixels(x, height-y, 1, 1, GL_RGBA, GL_UNSIGNED_BYTE, colorData.data());

	// Getting data
	unsigned int objectID{ (unsigned int)colorData[0] };

	std::cout << objectID << " at " << x << " " << y << std::endl;

	// Rebinding the previous framebuffer
	glBindFramebuffer(GL_FRAMEBUFFER, framebufferBefore);

	// Only found an object if objectID is not 0
	return objectID;
}

void ObjectScreenSelector::renderTexturePreview(Scene& scene, unsigned int screenQuadVAO, GizmoRenderer& objectClickGizmoRenderer)
{
	//renderSceneToTexture(scene, objectClickGizmoRenderer);


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

void ObjectScreenSelector::setResolution(unsigned int width, unsigned int height)
{
	deleteBuffers();

	// Saving the new parameters
	this->width = width;
	this->height = height;

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

void ObjectScreenSelector::deleteBuffers()
{
	glDeleteTextures(1, &objectClickTexture);
	glDeleteFramebuffers(1, &framebuffer);
}

void ObjectScreenSelector::renderSceneToTexture(Scene& scene, GizmoRenderer& objectClickGizmoRenderer)
{
	glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
	
	// Clearing the previous render
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glClear(GL_COLOR_BUFFER_BIT);

	glBindTexture(GL_TEXTURE_2D, objectClickTexture);

	/* DRAWING TO THE TEXTURE */

	objectColorShader.use();

	glm::mat4 model = glm::mat4(1.0f);
	objectColorShader.setMat4("model", model);

	glm::mat4 rotation = glm::mat4(1.0f);
	objectColorShader.setMat4("rotation", rotation);

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

	// Drawing the gizmos as clickables
	scene.drawClickSelectGizmos(objectClickGizmoRenderer);

	// Stop using our custom framebuffer to write to the screen again
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}
