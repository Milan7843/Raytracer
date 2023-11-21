#version 460 core
layout(local_size_x = 16, local_size_y = 16, local_size_z = 1) in;

layout(std140, binding = 3) buffer Pixels
{
    vec4 colors[];
};

uniform int screenWidth;
uniform vec2 screenSize;


#define PI 3.14159265359

// Returns a random value between 0 and 1 [0, 1)
float rand(float seed)
{
    float seedUsed = mod(seed, 2 * PI);
    return fract(sin(seedUsed) * 43758.5453);
}

float rand(int seed)
{
    float seedUsed = mod(float(seed), 2 * PI);
    return fract(sin(seedUsed) * 43758.5453);
}


int toPixelIndex(int x, int y)
{
    return int(x + screenWidth * y);
}









void main()
{
    // 2 dimensional indices
    int cx = int(gl_GlobalInvocationID.x);
    int cy = int(gl_GlobalInvocationID.y);
    int blockLocalX = cx;
    int blockLocalY = cy;

    // Calculating the total index, used to map the 2D indices to a 1D array
    int pixelIndex = int(cx + screenWidth * cy);

    vec4 avgOfNeighbours = vec4(0.);

    /*
    int radius = 2;

    for (int x = -radius; x <= radius; x++)
    {
        for (int y = -radius; y <= radius; y++)
        {
            int index = toPixelIndex(cx + x, cy + y);
            avgOfNeighbours += colors[index] / ((2* radius + 1) * (2 * radius + 1));
        }
    }*/

    vec3 resultColor = vec3(0.0);
    vec4 centerColor = colors[pixelIndex];
    float totalWeight = 0.0;
    float sigmaSpatial = 1.6; // Spatial standard deviation: lower->faster falloff
    float sigmaRange = 0.3;   // Range standard deviation: lower->faster falloff

    int radius = 3;

    for (int x = -radius; x <= radius; x++)
    {
        for (int y = -radius; y <= radius; y++)
        {
            int neighbourIndex = toPixelIndex(cx + x, cy + y);
            vec4 neighborColor = colors[neighbourIndex];

            //vec2 offset = vec2(float(x), float(y));// *(1.0 / float(screenWidth));
            float offset = sqrt(x * x + y * y);
            // Spatial weight
            // e^-x^2 gives a gaussian looking function that is:
            // 1.0 at 0.0
            // ~0.8 at 0.5
            // ~0.35 at 1.0
            // ~0.1 at 1.5
            // approaching 0.0 at 2.0+
            float spatial_x = (offset / sigmaSpatial);
            float spatialWeight = exp(-spatial_x * spatial_x);

            // Range weight
            float range_x = (length(neighborColor.rgb - centerColor.rgb) / sigmaRange);
            float rangeWeight = exp(-range_x * range_x);

            float weight = spatialWeight * rangeWeight;
            resultColor += neighborColor.rgb * weight;
            totalWeight += weight;
        }
    }

    resultColor /= totalWeight;
    colors[pixelIndex] = vec4(resultColor, 1.0);

    /*
    float neighbourStrength = 0.2;

    colors[pixelIndex] = colors[pixelIndex] * (1.0 - neighbourStrength) + avgOfNeighbours * neighbourStrength;*/
}