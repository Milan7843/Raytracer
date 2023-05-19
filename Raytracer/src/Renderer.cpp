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
	currentFrameSampleCount++;

	setUpForRender(sceneManagerBound->getCurrentScene(), &sceneManagerBound->getCurrentScene().getActiveCamera());

	computeShader.setBool("renderUsingBlocks", false);
	computeShader.setInt("pixelRenderSize", 4);

	// Running the compute shader once for each pixel
	glDispatchCompute(width / 16 * 4, height / 16 * 4, 1);
	//glDispatchCompute(std::ceil(float(size / 8.0f)), std::ceil(float(size / 8.0f)), 1);
	glMemoryBarrier(GL_ALL_BARRIER_BITS);

	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
}

void Renderer::startBlockRender()
{
	delete currentRenderProcess;

	setUpForRender(sceneManagerBound->getCurrentScene(), &sceneManagerBound->getCurrentScene().getActiveCamera());

	// Starting a new render process
	currentRenderProcess = new BlockRenderProcess(computeShader, width, height, blockSize, renderPassCount);
}

void Renderer::startRealtimeFrameRender()
{
	delete currentRenderProcess;

	setUpForRender(sceneManagerBound->getCurrentScene(), &sceneManagerBound->getCurrentScene().getActiveCamera());

	// Starting a new render process
	currentRenderProcess = new RealtimeRenderProcess(computeShader, width, height);
}

void Renderer::setUpForRender(Scene& scene, Camera* camera)
{
	computeShader.use();

	scene.writeObjectsToShader(&computeShader);

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
	computeShader.setInt("pixelRenderSize", 1);

	// Binding the hdri
	computeShader.setInt("hdri", 3);

	glActiveTexture(GL_TEXTURE3);
	glBindTexture(GL_TEXTURE_2D, scene.getHDRI());

	bindPixelBuffer();
}

void Renderer::update(float deltaTime, bool realtimeRaytracing, bool cameraMoved, Scene& currentScene)
{
	// If we should be realtime rendering, and the camera has actually moved,
	// we start a new realtime render
	if (realtimeRaytracing && (cameraMoved || currentScene.checkObjectUpdates(&computeShader)))
	{
		startRealtimeFrameRender();
	}

	if (currentRenderProcess == nullptr)
	{
		return;
	}

	// Check if the process is finished; if it is, no need to update it anymore
	if (currentRenderProcess->isFinished())
	{
		Logger::log("Finished render in " + formatTime(currentRenderProcess->getCurrentProcessTime()));
		delete currentRenderProcess;
		currentRenderProcess = nullptr;
		return;
	}

	computeShader.use();

	bindPixelBuffer();

	// There is a process going on, so we update it
	return currentRenderProcess->update(deltaTime, computeShader);
}

void Renderer::updateMeshData(Scene* scene)
{
	scene->writeObjectsToShader(&computeShader);
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

void Renderer::setSampleCount(unsigned int sampleCount)
{
	this->sampleCount = sampleCount;
}

void Renderer::verifyBlockSize()
{
	blockSize = (blockSize / 16)*16;
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
	if (currentRenderProcess == nullptr)
	{
		return 0.0f;
	}

	// There is a process going on, so we get the precise render progress from it directly
	return currentRenderProcess->getRenderProgressPrecise();
}

float Renderer::getRenderProgress()
{
	float progress = std::floor(getRenderProgressPrecise() * 100.0f) / 100.0f;
	return progress;
}

float Renderer::getTimeLeft()
{
	// If there is no process currently running, return 0.0
	if (currentRenderProcess == nullptr)
	{
		return 0.0f;
	}

	float totalTime = currentRenderProcess->getCurrentProcessTime() / std::max(getRenderProgressPrecise(), 0.001f);

	return totalTime - getRenderProgress() * totalTime;
}

std::string Renderer::formatTime(float time)
{
	int secondsTotal = (int)time;
	int seconds = secondsTotal % 60;
	int minutes = (secondsTotal / 60) % 60;
	int hours = secondsTotal / 3600;

	return std::format("{}h {}m {}s", hours, minutes, seconds);
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
