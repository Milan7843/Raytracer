#version 460 core

layout(local_size_x = 8, local_size_y = 8, local_size_z = 1) in;

uniform sampler2D originalTexture;

layout(rgba8) uniform image2D blurredTexture;

uniform float width;
uniform float height;
uniform vec3 outlineColor;

vec4 sampleOriginalTexture(vec2 pos)
{
    if (pos.x < 0 || pos.y < 0 || pos.x >= width || pos.y >= height)
        return vec4(0.0);
    
    return texture(originalTexture, pos / vec2(width-1, height-1));
}

void main()
{
    int blurSize = 4;
    float cutoff = blurSize * blurSize;

    vec4 cummulativeSample = vec4(0.0);

    float foundOutline = 0.0;

    if (sampleOriginalTexture(gl_GlobalInvocationID.xy) == vec4(1.0, 1.0, 1.0, 1.0))
    {
        // Pixel was on geometry

        // Test color: writes to previous geometry
        //imageStore(blurredTexture, ivec2(gl_GlobalInvocationID.xy), vec4(1.0, 0.0, 0.0, 1.0));
        return;
    }

    for (int y = -blurSize * 2; y <= blurSize * 2; y++)
    {
        for (int x = -blurSize * 2; x <= blurSize * 2; x++)
        {
            if (x*x + y*y < cutoff && 
                sampleOriginalTexture(gl_GlobalInvocationID.xy + vec2(x, y)) != 0.0)
            {
                foundOutline = 1.0;
                break;
            }
        }
    }

    // Writing outline color
    imageStore(blurredTexture, ivec2(gl_GlobalInvocationID.xy), foundOutline * vec4(outlineColor, 1.0));
}
