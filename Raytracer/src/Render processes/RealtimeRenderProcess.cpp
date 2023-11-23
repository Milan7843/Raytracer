#include "RealtimeRenderProcess.h"

RealtimeRenderProcess::RealtimeRenderProcess(ComputeShader& computeShader,
	unsigned int width,
	unsigned int height)
	: RenderProcess(computeShader, width, height)
{
	computeShader.setBool("renderUsingBlocks", false);
	computeShader.setInt("stackSize", stackSize);
	computeShader.setInt("renderPassCount", 1);
	computeShader.setInt("currentBlockRenderPassIndex", 0);
	computeShader.setInt("indirectLightingQuality", 0);
	computeShader.setInt("blockSize", width);
	generateStackBuffer();
}

RealtimeRenderProcess::~RealtimeRenderProcess()
{
}

unsigned int RealtimeRenderProcess::update(float deltaTime, ComputeShader& computeShader)
{
	if (finished)
	{
		return 0;
	}

	computeShader.setInt("pixelRenderSize", currentPixelSize);

	// Running the compute shader once for each pixel
	computeShader.run(std::ceil(width / (16.0f * currentPixelSize)), std::ceil(height / (16.0f * currentPixelSize)), 1);

	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

	// The final step would be rendering at pixel size 1
	if (currentPixelSize == 1)
	{
		finished = true;
	}

	currentPixelSize = currentPixelSize / 2;

	return (unsigned int)((float)width * height / (currentPixelSize * currentPixelSize));
}

void RealtimeRenderProcess::generateStackBuffer()
{
	// Deleting the previous stack buffer
	glDeleteBuffers(1, &stackBuffer);

	// Creating the stack array buffer
	stackBuffer = 0;

	glGenBuffers(1, &stackBuffer);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, stackBuffer);

	glBufferData(GL_SHADER_STORAGE_BUFFER, this->width * this->height * sizeof(int) * stackSize, 0, GL_DYNAMIC_COPY);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 7, stackBuffer);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
}

float RealtimeRenderProcess::getRenderProgressPrecise()
{
	return 0.0f;
}
