#pragma once

#include "Shader.h"
#include "ComputeShader.h"
#include "Scene.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>

/*
* Outline Renderer
* - Renders an outline around a given object or set of objects.
* 
* How it works:
* The OutlineRenderer holds a framebuffer, holding 2 textures to which the given objects are rendered in black/white.
* Then, it takes the first texture and applies a blur.
* This blurred texture is then rendered to the screen, which works as an outline.
 */
class OutlineRenderer
{
public:
	OutlineRenderer(unsigned int width, unsigned int height, unsigned int screenQuadVAO);
	~OutlineRenderer();

	// Set up everything needed for outline rendering
	void setup();

	// Render an outline for the given objects
	void render(Scene& scene);



private:

	glm::vec3 outlineColor{ glm::vec3(0.9f, 0.3f, 0.8f) };

	unsigned int screenQuadVAO{ 0 };

	// The framebuffer which keeps the render of the object
	unsigned int framebuffer{ 0 };

	// The unblurred texture
	unsigned int sharpTexture{ 0 };
	// The blurred texture
	unsigned int blurTexture{ 0 };

	// Window parameters
	unsigned int width;
	unsigned int height;

	// Texture parameters
	const unsigned int textureWidth{ 1200 };
	const unsigned int textureHeight{ 700 };

	// The shader to render the objects to the textures
	Shader objectRenderShader;

	// The shader to render the blur over top the screen
	Shader blurRenderShader;

	// The shader to blur the texture
	ComputeShader textureBlurrerShader;
};