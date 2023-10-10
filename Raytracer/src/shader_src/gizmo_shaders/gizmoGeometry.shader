#version 330 core
layout(points) in;
layout(triangle_strip, max_vertices = 4) out;

in vec3 toGeo_color[];
out vec2 texCoord;
out vec3 color;

uniform float gizmoSize;
uniform float aspectRatio; // width/height

void main()
{
    vec4 position = gl_in[0].gl_Position;
    position = position / position.w;

    float iconSize = gizmoSize;

    // Emit vertices for a square icon centered at the point
    gl_Position = position + vec4(-iconSize / aspectRatio, -iconSize, 0.0, 0.0);
    texCoord = vec2(0.0, 0.0);
    color = toGeo_color[0];
    EmitVertex();

    gl_Position = position + vec4(iconSize / aspectRatio, -iconSize, 0.0, 0.0);
    texCoord = vec2(1.0, 0.0);
    color = toGeo_color[0];
    EmitVertex();

    gl_Position = position + vec4(-iconSize / aspectRatio, iconSize, 0.0, 0.0);
    texCoord = vec2(0.0, 1.0);
    color = toGeo_color[0];
    EmitVertex();

    gl_Position = position + vec4(iconSize / aspectRatio, iconSize, 0.0, 0.0);
    texCoord = vec2(1.0, 1.0);
    color = toGeo_color[0];
    EmitVertex();
    
    
    EndPrimitive();
}
