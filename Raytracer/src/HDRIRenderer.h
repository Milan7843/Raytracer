#pragma once

#include "Camera.h"
#include "shaders/Shader.h"
#include "ImageLoader.h"

class HDRIRenderer
{
public:
	HDRIRenderer(const char* vertexShaderPath, const char* fragmentShaderPath);
	~HDRIRenderer();

	void drawHDRI(HDRITexture* hdri, Camera& camera);

private:

	// The shader used to draw the HDRI
	Shader hdriShader;

	// Generate a VAO which holds a cube which can act as a skybox
	void generateSkyboxVAO();

	unsigned int skyboxVAO;
};

