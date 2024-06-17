#include "Renderer.h"

Renderer::Renderer(ComputeShader& raytraceComputeShader, unsigned int width, unsigned int height)
	: computeShader(raytraceComputeShader), width(width), height(height)
	, denoiseShader("src/shader_src/denoise.shader")
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
	computeShader.run(width / 16 * 4, height / 16 * 4, 1);

	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
}

void Renderer::startBlockRender()
{
	delete currentRenderProcess;

	setUpForRender(sceneManagerBound->getCurrentScene(), &sceneManagerBound->getCurrentScene().getActiveCamera());

	// Starting a new render process
	currentRenderProcess = new BlockRenderProcess(computeShader, width, height, blockSize, renderPassCount);

	unpause();
}

void Renderer::startRealtimeFrameRender()
{
	delete currentRenderProcess;

	setUpForRender(sceneManagerBound->getCurrentScene(), &sceneManagerBound->getCurrentScene().getActiveCamera());

	// Starting a new render process
	currentRenderProcess = new RealtimeRenderProcess(computeShader, width, height);

	unpause();
}

void Renderer::setUpForRender(Scene& scene, Camera* camera)
{
	computeShader.use();

	scene.writeObjectsToShader(&computeShader);

	scene.bindTriangleBuffer();
	scene.bindMaterialsBuffer();
	scene.writeLightsToShader(&computeShader, true);
	scene.writeMaterialsToShader(&computeShader);
	Random::bindRandomTexture();
	computeShader.setInt("randomTexture", 5);

	// Writing camera data to the compute shader
	computeShader.setVector3("cameraPosition", CoordinateUtility::vec3ToGLSLVec3(camera->getPosition()));
	computeShader.setVector3("cameraRotation", camera->getRotation());
	computeShader.setFloat("fov", camera->getFov());

	computeShader.setBool("useHDRIAsBackground", *scene.getUseHDRIAsBackgroundPointer());
	computeShader.setFloat("hdriLightStrength", hdriLightStrength);

	// Writing rendering data to the compute shader
	computeShader.setInt("screenWidth", width);
	computeShader.setVector2("screenSize", width, height);
	computeShader.setInt("sampleCount", sampleCount);
	computeShader.setInt("multisamples", multisamples);
	computeShader.setInt("currentFrameSampleCount", currentFrameSampleCount);
	computeShader.setInt("pixelRenderSize", 1);
	computeShader.setInt("indirectLightingQuality", indirectLightingQuality);

	// Binding the hdri
	computeShader.setInt("hdri", 3);

	if (scene.hasHDRI())
	{
		glActiveTexture(GL_TEXTURE3);
		glBindTexture(GL_TEXTURE_2D, scene.getHDRI()->textureID);
	}

	// Binding the texture atlas
	computeShader.setInt("textureAtlas", 4);

	glActiveTexture(GL_TEXTURE4);
	glBindTexture(GL_TEXTURE_2D, TextureHandler::packTextures(scene.getMaterials()));

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

	if (currentRenderProcess == nullptr || paused)
	{
		raysPerSecond = 0;
		return;
	}

	// Check if the process is finished; if it is, no need to update it anymore
	if (currentRenderProcess->isFinished())
	{
		bindPixelBuffer();
		denoiseShader.use();
		// Denoising
		denoiseShader.setVector2("screenSize", width, height);
		denoiseShader.setInt("screenWidth", width);
		//denoiseShader.run((width-1) / 16 + 1, (height-1) / 16 + 1, 1);

		Logger::log("Finished render in " + formatTime(currentRenderProcess->getCurrentProcessTime()));
		delete currentRenderProcess;
		currentRenderProcess = nullptr;
		return;
	}

	computeShader.use();

	bindPixelBuffer();

	// There is a process going on, so we update it
	unsigned int pixelsRendered{ currentRenderProcess->update(deltaTime, computeShader) };
	raysSinceLastRaysPerSecondUpdate += pixelsRendered * sampleCount;
	timeSinceRaysPerSecondUpdate += deltaTime;

	// If it is time for a new update
	if (timeSinceRaysPerSecondUpdate > timeBetweenRaysPerSecondUpdate)
	{
		raysPerSecond = (float)raysSinceLastRaysPerSecondUpdate / timeSinceRaysPerSecondUpdate;

		timeSinceRaysPerSecondUpdate = 0.0f;
		raysSinceLastRaysPerSecondUpdate = 0;
	}

}

void Renderer::updateMeshData(Scene* scene)
{
	scene->writeObjectsToShader(&computeShader);
}

void Renderer::setResolution(unsigned int width, unsigned int height)
{
	// Generate the buffer if it didn't exist yet
	if (pixelBuffer == 0)
	{
		glGenBuffers(1, &pixelBuffer);
	}

	this->width = width;
	this->height = height;

	glBindBuffer(GL_SHADER_STORAGE_BUFFER, pixelBuffer);

	// Setting buffer size to be width x height x 16, for 16 bytes per pixel
	glBufferData(GL_SHADER_STORAGE_BUFFER, width * height * 16, 0, GL_DYNAMIC_COPY);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 3, pixelBuffer);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);


	// Generate the buffer if it didn't exist yet
	if (pixelDataBuffer == 0)
	{
		glGenBuffers(1, &pixelDataBuffer);
	}
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, pixelDataBuffer);

	// Setting buffer size to be width x height x 16, for 16 bytes per pixel
	glBufferData(GL_SHADER_STORAGE_BUFFER, width * height * 16, 0, GL_STATIC_COPY);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 10, pixelDataBuffer);
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
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, pixelDataBuffer);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 10, pixelDataBuffer);
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

void Renderer::drawInterface()
{
	ImGui::SliderInt("Block size", &blockSize, 16, 160, "%d", ImGuiSliderFlags_AlwaysClamp);
	verifyBlockSize();
	ImGuiUtility::drawHelpMarker("The size of a render block in pixels. Must be a multiple of 16 and will be snapped to the nearest multiple of 16 if it is not.");

	ImGui::SliderInt("Multisamples", &multisamples, 1, 5, "%d", ImGuiSliderFlags_AlwaysClamp);
	ImGuiUtility::drawHelpMarker("The number of different sample points per pixel, works as anti-aliasing.");

	// Samples per render pass
	ImGui::SliderInt("Sample count", &sampleCount, 1, 1000, "%d", ImGuiSliderFlags_AlwaysClamp | ImGuiSliderFlags_Logarithmic);
	ImGuiUtility::drawHelpMarker("The number of samples per pixel per render pass.");

	// Render passes per block
	ImGui::SliderInt("Block passes", &renderPassCount, 1, 100, "%d", ImGuiSliderFlags_AlwaysClamp);
	ImGuiUtility::drawHelpMarker("The number of passes per block. Each pass will take the full number of samples for each pixel.");

	// Render passes per block
	ImGui::SliderInt("Indirect lighting quality", &indirectLightingQuality, 0, 10, "%d", ImGuiSliderFlags_AlwaysClamp);
	ImGuiUtility::drawHelpMarker("The quality of the indirect lighting calculation. Higher quality increases the number of possible light bounces and reduces noise. Set to 0 for no indirect lighting.");

	// The strength of the HDRI lighting calculation
	ImGui::DragFloat("HDRI light strength", &hdriLightStrength, 0.01f, 0.0f, 1.0f, "%.2f");
	ImGuiUtility::drawHelpMarker("How much the HDRI influences the lighting of the scene.");
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

unsigned int Renderer::getRaysPerSecond()
{
	return raysPerSecond;
}

bool Renderer::isPaused()
{
	return paused;
}

void Renderer::pause()
{
	this->setPaused(true);
}

void Renderer::unpause()
{
	this->setPaused(false);
}

void Renderer::setPaused(bool pause)
{
	this->paused = pause;
}

BVHRenderMode Renderer::getBVHRenderMode()
{
	return bvhRenderMode;
}

void Renderer::setBVHRenderMode(BVHRenderMode newBVHRenderMode)
{
	bvhRenderMode = newBVHRenderMode;
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
	filestream >> indirectLightingQuality;
	filestream >> hdriLightStrength;

	int readBVHRenderMode;
	filestream >> readBVHRenderMode;
	bvhRenderMode = (BVHRenderMode)readBVHRenderMode;

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
	filestream << indirectLightingQuality << "\n";
	filestream << hdriLightStrength << "\n";
	filestream << (int)bvhRenderMode << "\n";

	// Done writing so flush data and close filestream
	filestream.close();
}
