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

	unsigned int update(float deltaTime, ComputeShader& computeShader) override;

	float getRenderProgressPrecise() const override;

private:
	void generateStackBuffer() override;
	unsigned int currentPixelSize{ 8 };
};

