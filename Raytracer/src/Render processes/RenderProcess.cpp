#include "RenderProcess.h"

float RenderProcess::getCurrentProcessTime() const
{
    return currentProcessTime;
}

bool RenderProcess::isFinished() const
{
    return finished;
}