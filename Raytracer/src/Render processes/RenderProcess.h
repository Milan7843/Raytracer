#pragma once
#include "ComputeShader.h"

class RenderProcess
{
public:
	~RenderProcess() {}

	// Update this render process; must be called every frame
	virtual void update(float deltaTime, ComputeShader& computeShader) {}

	// The current progress of this render process [0.0f, 1.0f]
	virtual float getRenderProgressPrecise() { return 0.0f; }

	// The time this process has been going for
	float getCurrentProcessTime();

	// Get whether the process is finished
	bool isFinished();

protected:
	RenderProcess(
		ComputeShader& computeShader,
		unsigned int width,
		unsigned int height)
		: computeShader(computeShader)
		, width(width)
		, height(height)
	{}

	// The resolution used to render
	unsigned int width, height;

	// The shader we're rendering with
	ComputeShader& computeShader;

	// The time this process has been going for
	float currentProcessTime{ 0.0f };

	// Whether the process is finished
	bool finished{ false };
};

