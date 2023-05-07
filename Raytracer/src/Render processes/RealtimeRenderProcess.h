#pragma once

#include "RenderProcess.h"

class RealtimeRenderProcess : public RenderProcess
{
public:
	RealtimeRenderProcess(
		ComputeShader& computeShader,
		unsigned int width,
		unsigned int height
	);
	~RealtimeRenderProcess();

	void update(float deltaTime, ComputeShader& computeShader) override;

	float getRenderProgressPrecise() override;

private:
	unsigned int currentPixelSize{ 8 };
};

