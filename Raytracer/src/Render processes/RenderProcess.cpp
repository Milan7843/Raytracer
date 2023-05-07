#include "RenderProcess.h"

float RenderProcess::getCurrentProcessTime()
{
    return currentProcessTime;
}

bool RenderProcess::isFinished()
{
    return finished;
}
