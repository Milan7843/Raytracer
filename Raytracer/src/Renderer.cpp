#include "Renderer.h"

Renderer::Renderer(const char* raytraceComputeShaderPath, unsigned int width, unsigned int height, Scene* scene)
	: computeShader(raytraceComputeShaderPath, scene), width(width), height(height)
{
	// Immediately creating the pixel buffer with the given width and height
	setResolution(width, height);
}

Renderer::~Renderer()
{
}

void Renderer::render(Scene* scene, Camera* camera)
{
	currentlyBlockRendering = false;

	currentFrameSampleCount++;

	setUpForRender(scene, camera);

	computeShader.setBool("renderUsingBlocks", false);

	// Running the compute shader once for each pixel
	glDispatchCompute(width, height, 1);
	//glDispatchCompute(std::ceil(float(size / 8.0f)), std::ceil(float(size / 8.0f)), 1);
	glMemoryBarrier(GL_ALL_BARRIER_BITS);

	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
}

void Renderer::startBlockRender(Scene* scene, Camera* camera)
{
	currentlyBlockRendering = true;

	// Resetting block indices
	blockIndexX = 0;
	blockIndexY = 0;
	currentBlockRenderPassIndex = 0;

	blockSizeRendering = blockSize;

	setUpForRender(scene, camera);

	computeShader.setBool("renderUsingBlocks", true);
	computeShader.setInt("blockSize", blockSizeRendering);
	computeShader.setInt("renderPassCount", renderPassCount);

	blockRenderStep();
}

void Renderer::blockRenderStep()
{
	computeShader.use();
	computeShader.setVector2("currentBlockOrigin", getBlockOrigin());
	computeShader.setInt("currentBlockRenderPassIndex", currentBlockRenderPassIndex);

	// Running the compute shader once for each pixel in the block
	glDispatchCompute(blockSizeRendering, blockSizeRendering, 1);
	//glDispatchCompute(std::ceil(float(size / 8.0f)), std::ceil(float(size / 8.0f)), 1);
	glMemoryBarrier(GL_ALL_BARRIER_BITS);

	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

	currentBlockRenderPassIndex++;
}

void Renderer::setUpForRender(Scene* scene, Camera* camera)
{
	// Reset current render time
	currentRenderTime = 0.0f;

	computeShader.use();

	scene->checkObjectUpdates(&computeShader);

	scene->bindTriangleBuffer();
	scene->writeLightsToShader(&computeShader);
	scene->writeMaterialsToShader(&computeShader);

	// Writing camera data to the compute shader
	computeShader.setVector3("cameraPosition", CoordinateUtility::vec3ToGLSLVec3(camera->getPosition()));
	computeShader.setVector3("cameraRotation", camera->getRotation());
	computeShader.setFloat("fov", camera->getFov());

	// Writing rendering data to the compute shader
	computeShader.setInt("screenWidth", width);
	computeShader.setVector2("screenSize", width, height);
	computeShader.setInt("sampleCount", sampleCount);
	computeShader.setInt("multisamples", multisamples);
	computeShader.setInt("currentFrameSampleCount", currentFrameSampleCount);

	bindPixelBuffer();
}

void Renderer::update(float deltaTime)
{
	if (currentlyBlockRendering)
	{
		// Render a block
		blockRenderStep();

		// Add past time to current render time
		currentRenderTime += deltaTime;

		// Check if we need to render more passes on this specific block
		if (currentBlockRenderPassIndex < renderPassCount)
		{
			// Stay on this block
			return;
		}

		// Check if it is at the right side of the screen
		if (getBlockOrigin().x + blockSizeRendering >= width)
		{
			// Move down a layer
			blockIndexY++;
			blockIndexX = 0;

			currentBlockRenderPassIndex = 0;
		}
		else
		{
			// Just move right
			blockIndexX++;

			currentBlockRenderPassIndex = 0;
		}
		// Check for finished render
		if (getBlockOrigin().y >= height)
		{
			currentlyBlockRendering = false;
		}
	}
}

void Renderer::updateMeshData(Scene* scene)
{
	scene->checkObjectUpdates(&computeShader);
}

void Renderer::setResolution(unsigned int width, unsigned int height)
{
	// Deleting the previous pixel buffer
	glDeleteBuffers(1, &pixelBuffer);

	this->width = width;
	this->height = height;

	// Creating the pixel array buffer
	pixelBuffer = 0;
	glGenBuffers(1, &pixelBuffer);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, pixelBuffer);

	// Setting buffer size to be width x height x 16, for 16 bytes per pixel
	glBufferData(GL_SHADER_STORAGE_BUFFER, width * height * 16, 0, GL_DYNAMIC_COPY);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 3, pixelBuffer);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
}

void Renderer::bindPixelBuffer()
{
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, pixelBuffer);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 3, pixelBuffer);
}

glm::vec2 Renderer::getBlockOrigin()
{
	return glm::vec2(blockIndexX * blockSizeRendering, blockIndexY * blockSizeRendering);
}

void Renderer::setSampleCount(unsigned int sampleCount)
{
	this->sampleCount = sampleCount;
}

int* Renderer::getBlockSizePointer()
{
	return &blockSize;
}

int* Renderer::getMultisamplePointer()
{
	return &multisamples;
}

int* Renderer::getSampleCountPointer()
{
	return &sampleCount;
}

int* Renderer::getRenderPassCountPointer()
{
	return &renderPassCount;
}

float Renderer::getRenderProgressPrecise()
{
	float blocksInHeight = (float)height / (float)blockSizeRendering;
	float blocksInWidth = (float)width / (float)blockSizeRendering;

	int blocksDone = blockIndexX + blockIndexY * blocksInWidth;

	float progress = blocksDone / (blocksInWidth * blocksInHeight);
	return progress;
}

float Renderer::getRenderProgress()
{
	float progress = std::floor(getRenderProgressPrecise() * 100.0f) / 100.0f;
	return progress;
}

float Renderer::getTimeLeft()
{
	float totalTime = currentRenderTime / std::max(getRenderProgressPrecise(), 0.01f);

	return totalTime - getRenderProgress() * totalTime;
}
