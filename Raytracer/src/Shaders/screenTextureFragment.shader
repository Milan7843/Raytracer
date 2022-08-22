#version 460 core

in vec2 pixelPos;

out vec4 FragColor;

uniform sampler2D textureToRender;

void main()
{
    vec2 uv = pixelPos * 0.5 + vec2(0.5);
    vec4 col = texture(textureToRender, uv);
    if (col.a < 0.001)
        discard;
    FragColor = col;
}