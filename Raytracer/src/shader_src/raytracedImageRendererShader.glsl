#version 460 core
out vec4 FragColor;

in vec2 pixelPos;

uniform vec2 screenSize;

// Pixels: holds information about the rendered pixels
layout(std140, binding = 3) buffer Pixels
{
    vec4 pixelColors[];
};

void main()
{
    vec2 pixelPosIndex = vec2((pixelPos.x / 2. + 0.5) * screenSize.x, (pixelPos.y / 2. + 0.5) * screenSize.y);
    int pixelIndex = int(int(pixelPosIndex.x) + int(pixelPosIndex.y) * screenSize.x);

    // Set the fragment color to the raytraced pixel
    FragColor = pixelColors[pixelIndex];
}