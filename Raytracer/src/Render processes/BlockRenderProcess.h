#pragma once

#include "RenderProcess.h"

struct IndirectLightingPixelData
{
	glm::vec3 position;
	glm::vec3 color;
	glm::vec3 normal;
	unsigned int closestTriHit;
	unsigned int closestSphereHit;
};

class BlockRenderProcess: public RenderProcess
{
public:
	BlockRenderProcess(
		ComputeShader& computeShader,
		unsigned int width,
		unsigned int height,
		int blockSize,
		int renderPassCount
	);
	~BlockRenderProcess();

	void update(float deltaTime, ComputeShader& computeShader) override;

	float getRenderProgressPrecise() override;

private:

	void generateIndirectLightingDataBuffer();
	void generateStackBuffer() override;

	void blockRenderStep(ComputeShader& computeShader);

	// Get the top-left coordinate of the currently rendering block
	glm::vec2 getBlockOrigin();

	// The numbers of render passes per block
	int renderPassCount{ 1 };

	// The size in pixels of each block
	int blockSize;
	unsigned int blockIndexX{ 0 }, blockIndexY{ 0 };

	int currentBlockRenderPassIndex{ 0 };

	unsigned int indirectLightingDataBuffer{ 0 };
};

