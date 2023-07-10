#include "RealtimeRenderProcess.h"

RealtimeRenderProcess::RealtimeRenderProcess(ComputeShader& computeShader,
	unsigned int width,
	unsigned int height)
	: RenderProcess(computeShader, width, height)
{
}

RealtimeRenderProcess::~RealtimeRenderProcess()
{
}

void RealtimeRenderProcess::update(float deltaTime, ComputeShader& computeShader)
{
	if (finished)
	{
		return;
	}

	computeShader.setBool("renderUsingBlocks", false);
	computeShader.setInt("pixelRenderSize", currentPixelSize);
	computeShader.setInt("renderPassCount", 1);
	computeShader.setInt("currentBlockRenderPassIndex", 0);

	// Running the compute shader once for each pixel
	//glDispatchCompute(width / (16 * 4), height / (16 * 4), 1);
	glDispatchCompute(std::ceil(width / (16.0f * currentPixelSize)), std::ceil(height / (16.0f * currentPixelSize)), 1);
	//glDispatchCompute(std::ceil(float(size / 8.0f)), std::ceil(float(size / 8.0f)), 1);
	glMemoryBarrier(GL_ALL_BARRIER_BITS);

	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

	// The final step would be rendering at pixel size 1
	if (currentPixelSize == 1)
	{
		finished = true;
	}

	currentPixelSize = currentPixelSize / 2;
}

float RealtimeRenderProcess::getRenderProgressPrecise()
{
	return 0.0f;
}
