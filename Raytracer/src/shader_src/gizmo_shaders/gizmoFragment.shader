#version 460 core

in vec2 texCoord;
in vec3 color;
out vec4 fragColor;

uniform sampler2D gizmoTexture;

void main()
{
    vec4 iconColor = texture(gizmoTexture, texCoord) * vec4(color, 1.);

    fragColor = iconColor;
}