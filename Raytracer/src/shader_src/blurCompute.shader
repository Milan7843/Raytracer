#version 460 core

layout(local_size_x = 8, local_size_y = 8, local_size_z = 1) in;

uniform sampler2D originalTexture;

layout(rgba8) uniform image2D blurredTexture;

uniform float width;
uniform float height;

vec4 sampleOriginalTexture(vec2 pos)
{
    if (pos.x < 0 || pos.y < 0 || pos.x >= width || pos.y >= height)
        return vec4(0.0);
    
    return texture(originalTexture, pos / vec2(width, height));
}

float gaussian2D(float x, float y, float sigma)
{
    // Calculate the normalization factor
    float norm = 1.0 / (2.0 * 3.14159265358979323846 * sigma * sigma);

    // Calculate the exponent factor
    float exponent = -(x * x + y * y) / (2.0 * sigma * sigma);

    //return 1.0;

    // Return the value of the Gaussian function
    return norm * exp(exponent);
}

void main()
{
    int blurSize = 100;
    float cutoff = blurSize * blurSize * 2;
    float kernelSize = 0.2;
    float dist = kernelSize / (2 * blurSize);

    vec4 cummulativeSample = vec4(0.0);

    float samples = 0.0;

    vec3 sum = vec3(0.);
    vec2 uv = gl_GlobalInvocationID.xy / vec2(width, height);

    for (int y = -blurSize; y <= blurSize; y++)
    {
        for (int x = -blurSize; x <= blurSize; x++)
        {
            if (x * x + y * y < cutoff)
            {
                vec2 sampleUV = uv + vec2(x, y) * dist;

                float t = gaussian2D(x * dist, y * dist, 0.05);

                sum += min(vec3(10.), texture(originalTexture, sampleUV).rgb) * t;

                samples += t;
            }
        }
    }

    // Writing outline color
    imageStore(blurredTexture, ivec2(gl_GlobalInvocationID.xy)-ivec2(1,1), vec4(sum / samples, 1.0));
}
