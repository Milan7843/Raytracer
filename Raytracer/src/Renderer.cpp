#include "Renderer.h"

Renderer::Renderer(const char* raytraceComputeShaderPath, unsigned int width, unsigned int height)
	: computeShader(raytraceComputeShaderPath), width(width), height(height)
{
	// Immediately creating the pixel buffer with the given width and height
	setResolution(width, height);
	// Retrieving the render settings from the save file
	readRenderSettings();
}

Renderer::~Renderer()
{
	// Saving all render settings on quitting
	writeRenderSettingsToFile();
}

void Renderer::bindSceneManager(SceneManager* sceneManager)
{
	this->sceneManagerBound = sceneManager;
}

void Renderer::render()
{
	currentlyBlockRendering = false;

	currentFrameSampleCount++;

	setUpForRender(sceneManagerBound->getCurrentScene(), &sceneManagerBound->getCurrentScene().getActiveCamera());

	computeShader.setBool("renderUsingBlocks", false);

	// Running the compute shader once for each pixel
	glDispatchCompute(width, height, 1);
	//glDispatchCompute(std::ceil(float(size / 8.0f)), std::ceil(float(size / 8.0f)), 1);
	glMemoryBarrier(GL_ALL_BARRIER_BITS);

	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
}

void Renderer::startBlockRender()
{
	currentlyBlockRendering = true;

	// Resetting block indices
	blockIndexX = 0;
	blockIndexY = 0;
	currentBlockRenderPassIndex = 0;

	blockSizeRendering = blockSize;

	setUpForRender(sceneManagerBound->getCurrentScene(), &sceneManagerBound->getCurrentScene().getActiveCamera());

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

void Renderer::setUpForRender(Scene& scene, Camera* camera)
{
	// Reset current render time
	currentRenderTime = 0.0f;

	computeShader.use();

	scene.checkObjectUpdates(&computeShader);

	scene.bindTriangleBuffer();
	scene.writeLightsToShader(&computeShader, true);
	scene.writeMaterialsToShader(&computeShader);

	// Writing camera data to the compute shader
	computeShader.setVector3("cameraPosition", CoordinateUtility::vec3ToGLSLVec3(camera->getPosition()));
	computeShader.setVector3("cameraRotation", camera->getRotation());
	computeShader.setFloat("fov", camera->getFov());

	computeShader.setBool("useHDRIAsBackground", *scene.getUseHDRIAsBackgroundPointer());

	// Writing rendering data to the compute shader
	computeShader.setInt("screenWidth", width);
	computeShader.setVector2("screenSize", width, height);
	computeShader.setInt("sampleCount", sampleCount);
	computeShader.setInt("multisamples", multisamples);
	computeShader.setInt("currentFrameSampleCount", currentFrameSampleCount);

	// Binding the hdri
	computeShader.setInt("hdri", 0);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, scene.getHDRI());

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

unsigned int Renderer::getWidth()
{
	return width;
}

unsigned int Renderer::getHeight()
{
	return height;
}

void Renderer::bindPixelBuffer()
{
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, pixelBuffer);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 3, pixelBuffer);
}

unsigned int Renderer::getPixelBuffer()
{
	return pixelBuffer;
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
	float iterationsDone = blocksDone * renderPassCount + currentBlockRenderPassIndex;

	float totalIterations = blocksInWidth * blocksInHeight * renderPassCount;

	float progress = iterationsDone / totalIterations;
	return progress;
}

float Renderer::getRenderProgress()
{
	float progress = std::floor(getRenderProgressPrecise() * 100.0f) / 100.0f;
	return progress;
}

float Renderer::getTimeLeft()
{
	float totalTime = currentRenderTime / std::max(getRenderProgressPrecise(), 0.001f);

	return totalTime - getRenderProgress() * totalTime;
}

void Renderer::readRenderSettings()
{
	std::ifstream filestream{ "render_settings/saved_render_settings.settings" };

	if (!filestream)
	{
		// No file was found to read: use default settings
		return;
	}

	// Reading the data form the file
	filestream >> blockSize;
	filestream >> sampleCount;
	filestream >> renderPassCount;
	filestream >> multisamples;
	filestream >> blockSize;

	// Finally closing the file
	filestream.close();
}

void Renderer::writeRenderSettingsToFile()
{
	// The data stream into the file
	std::ofstream filestream{ "render_settings/saved_render_settings.settings" };

	// Writing all render data
	filestream << blockSize << "\n";
	filestream << sampleCount << "\n";
	filestream << renderPassCount << "\n";
	filestream << multisamples << "\n";
	filestream << blockSize << "\n";

	// Done writing so flush data and close filestream
	filestream.close();
}
