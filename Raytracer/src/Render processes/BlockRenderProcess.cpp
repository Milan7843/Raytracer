#include "BlockRenderProcess.h"

BlockRenderProcess::BlockRenderProcess(
	ComputeShader& computeShader,
	unsigned int width,
	unsigned int height,
	int blockSize,
	int renderPassCount)
	: RenderProcess(computeShader, width, height)
	, renderPassCount(renderPassCount)
	, blockSize(blockSize)
{
	// Setting the variable of the compute shader
	computeShader.setBool("renderUsingBlocks", true);
	computeShader.setInt("blockSize", blockSize);
	computeShader.setInt("renderPassCount", renderPassCount);
	computeShader.setInt("stackSize", stackSize);

	generateIndirectLightingDataBuffer();
	generateStackBuffer();
}

BlockRenderProcess::~BlockRenderProcess()
{
	glDeleteBuffers(1, &indirectLightingDataBuffer);
}

unsigned int BlockRenderProcess::update(float deltaTime, ComputeShader& computeShader)
{
	if (finished)
	{
		return 0;
	}

	// Render a block
	blockRenderStep(computeShader);

	// Add past time to current render time
	currentProcessTime += deltaTime;

	unsigned int pixelsRendered{ (unsigned int)blockSize * blockSize };

	// Check if we need to render more passes on this specific block
	if (currentBlockRenderPassIndex < renderPassCount)
	{
		// Stay on this block
		return pixelsRendered;
	}

	// Check if it is at the right side of the screen
	if (getBlockOrigin().x + blockSize >= width)
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
		finished = true;
	}

	return pixelsRendered;
}

void BlockRenderProcess::generateIndirectLightingDataBuffer()
{
	// Deleting the previous pixel buffer
	glDeleteBuffers(1, &indirectLightingDataBuffer);

	// Creating the pixel array buffer
	indirectLightingDataBuffer = 0;

	glGenBuffers(1, &indirectLightingDataBuffer);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, indirectLightingDataBuffer);

	unsigned int indirectLightingPixelDataSize = {
		// 3 times 16 bytes: vec3, the integers fit in the padding
		3*16
	};
	glBufferData(GL_SHADER_STORAGE_BUFFER, this->blockSize * this->blockSize * indirectLightingPixelDataSize, 0, GL_DYNAMIC_COPY);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 6, indirectLightingDataBuffer);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

	std::cout << "Indirect lighting buffer " << indirectLightingDataBuffer << " with " << this->blockSize * this->blockSize * sizeof(IndirectLightingPixelData) << "bytes" << std::endl;
}

void BlockRenderProcess::generateStackBuffer()
{
	// Deleting the previous stack buffer
	glDeleteBuffers(1, &stackBuffer);

	// Creating the stack array buffer
	stackBuffer = 0;

	glGenBuffers(1, &stackBuffer);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, stackBuffer);

	glBufferData(GL_SHADER_STORAGE_BUFFER, this->blockSize * this->blockSize * sizeof(int) * stackSize, 0, GL_DYNAMIC_COPY);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 7, stackBuffer);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
}

void BlockRenderProcess::blockRenderStep(ComputeShader& computeShader)
{
	computeShader.use();
	computeShader.setVector2("currentBlockOrigin", getBlockOrigin());
	computeShader.setInt("currentBlockRenderPassIndex", currentBlockRenderPassIndex);

	computeShader.run(blockSize / 16, blockSize / 16, 1);

	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

	currentBlockRenderPassIndex++;
}

float BlockRenderProcess::getRenderProgressPrecise()
{
	float blocksInHeight = (float)height / (float)blockSize;
	float blocksInWidth = (float)width / (float)blockSize;

	int blocksDone = blockIndexX + blockIndexY * blocksInWidth;
	float iterationsDone = blocksDone * renderPassCount + currentBlockRenderPassIndex;

	float totalIterations = blocksInWidth * blocksInHeight * renderPassCount;

	float progress = iterationsDone / totalIterations;
	return progress;
}

glm::vec2 BlockRenderProcess::getBlockOrigin()
{
	return glm::vec2(blockIndexX * blockSize, blockIndexY * blockSize);
}
