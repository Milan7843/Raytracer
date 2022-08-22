#version 460 core

layout(local_size_x = 1, local_size_y = 1, local_size_z = 1) in;

uniform sampler2D originalTexture;

layout(rgba8) uniform image2D blurredTexture;

uniform float width;
uniform float height;

vec4 sampleOriginalTexture(vec2 pos)
{/*
    if (pos.x < 0)
        pos.x = -pos.x;
    if (pos.y < 0)
        pos.y = -pos.y;
    if (pos.x > width)
        pos.x = width;
    if (pos.y > height)
        pos.y = height;
    */
    return texture(originalTexture, pos / vec2(width, height));
}

void main()
{
    // 2 dimensional indices
    int cx = int(gl_GlobalInvocationID.x);
    int cy = int(gl_GlobalInvocationID.y);

    int blurSize = 0;
    float cutoff = 16;

    float samples = (blurSize*2 + 1.) * (blurSize*2 + 1.);

    vec4 cummulativeSample = sampleOriginalTexture(gl_GlobalInvocationID.xy);

    for (int y = -blurSize; y <= blurSize; y++)
    {
        for (int x = -blurSize; x <= blurSize; x++)
        {
            //cummulativeSample += sampleOriginalTexture(gl_GlobalInvocationID.xy + vec2(x, y)) / samples;
        }
    }

    //vec4 originalColor = sampleOriginalTexture(gl_GlobalInvocationID.xy);

    //imageStore(blurredTexture, ivec2(gl_GlobalInvocationID.xy), texture(originalTexture, gl_GlobalInvocationID.xy / vec2(width, height))/ 2.);
    imageStore(blurredTexture, ivec2(gl_GlobalInvocationID.xy), vec4(0.8, 0.3, 0.6, 1.0));
}
