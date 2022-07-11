#version 460 core
out vec4 FragColor;

in vec2 pixelPos;

uniform vec3 cameraPosition;
uniform vec3 cameraRotation;
uniform vec2 screenSize;

// Pixels: holds information about the rendered pixels
#define NUM_PIXELS 5//$numPixels
layout(std140, binding = 3) buffer Pixels
{
    vec4 pixelColors[NUM_PIXELS];
    int sampleCount;
};

/*
 * The idea of the lighting system:
 * Fire a ray, then calculate the lighting at that point with simple techniques (e.g. dot product normal with point light with shadow check)
 * Then, also fire a random ray in a hemisphere from the normal,
   then calculate the light at that point, mixing it with the light calculated in the previous step.
 */


void main()
{
    vec2 pixelPosIndex = vec2((pixelPos.x / 2. + 0.5) * screenSize.x, (pixelPos.y / 2. + 0.5) * screenSize.y);
    int pixelIndex = int(int(pixelPosIndex.x) + int(pixelPosIndex.y) * screenSize.x);

    // Set the fragment color to the raytraced pixel
    FragColor = pixelColors[pixelIndex];
}