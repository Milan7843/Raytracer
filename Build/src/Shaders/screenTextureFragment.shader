#version 460 core

in vec2 pixelPos;

out vec4 FragColor;

uniform sampler2D textureToRender;

void main()
{
    // Offset UV then scale, since uv.x and uv.y are in [-1, 1], but we need [0, 1]
    vec2 uv = (pixelPos + vec2(1.0)) * 0.5;
    vec4 col = texture(textureToRender, uv);

    // Discard pixel by alpha
    if (col.a < 0.001)
        discard;

    FragColor = col;
}