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
	// Returns whether an object was clicked
	bool checkObjectClicked(Scene& scene, double x, double y);

	// Render the texture used to decide what object was clicked
	void renderTexturePreview(Scene& scene, unsigned int screenQuadVAO);

private:

	// Set up everything needed for object screen click selection
	void setup();

	// Renders the scene to the texture, giving each a different color
	void renderSceneToTexture(Scene& scene);

	unsigned int framebuffer{ 0 };

	unsigned int objectClickTexture{ 0 };

	// Window parameters
	unsigned int width;
	unsigned int height;

	// Renders each object with a unique color
	Shader objectColorShader;

	// Renders a texture to the screen
	Shader textureRenderShader;
};
