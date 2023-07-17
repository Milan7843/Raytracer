#pragma once

#include "Scene.h"
#include "shaders/Shader.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>

class ObjectScreenSelector
{
public:
	ObjectScreenSelector(unsigned int width, unsigned int height);
	~ObjectScreenSelector();

	// Set the clicked object in the given scene at the coordinates to be selected
	// Returns the id of the object that was clicked, or 0 for no object hit
	unsigned int checkObjectClicked(Scene& scene, unsigned int x, unsigned int y);

	// Render the texture used to decide what object was clicked
	void renderTexturePreview(Scene& scene, unsigned int screenQuadVAO);

	// Set the resolution the object screen selector is rendering at
	void setResolution(unsigned int width, unsigned int height);

private:

	// Renders the scene to the texture, giving each a different color
	void renderSceneToTexture(Scene& scene);

	unsigned int framebuffer{ 0 };

	unsigned int objectClickTexture{ 0 };

	void deleteBuffers();

	// Window parameters
	unsigned int width{ 0 };
	unsigned int height{ 0 };

	// Renders each object with a unique color
	Shader objectColorShader;

	// Renders a texture to the screen
	Shader textureRenderShader;
};
