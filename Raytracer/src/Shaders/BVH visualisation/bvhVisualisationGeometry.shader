#version 460 core

layout(points) in;
layout(line_strip, max_vertices = 24) out;

in vec3 pos[];
in vec3 size[];

uniform mat4 view;
uniform mat4 projection;

void main()
{
    // Calculating the eight vertices of the box
    vec4 vertices[8];
    vertices[0] = projection * view * vec4((pos[0] + 0.5 * size[0] * vec3(-1.0, -1.0, -1.0)), 1.0);
    vertices[1] = projection * view * vec4((pos[0] + 0.5 * size[0] * vec3(1.0, -1.0, -1.0)), 1.0);
    vertices[2] = projection * view * vec4((pos[0] + 0.5 * size[0] * vec3(-1.0, 1.0, -1.0)), 1.0);
    vertices[3] = projection * view * vec4((pos[0] + 0.5 * size[0] * vec3(1.0, 1.0, -1.0)), 1.0);
    vertices[4] = projection * view * vec4((pos[0] + 0.5 * size[0] * vec3(-1.0, -1.0, 1.0)), 1.0);
    vertices[5] = projection * view * vec4((pos[0] + 0.5 * size[0] * vec3(1.0, -1.0, 1.0)), 1.0);
    vertices[6] = projection * view * vec4((pos[0] + 0.5 * size[0] * vec3(-1.0, 1.0, 1.0)), 1.0);
    vertices[7] = projection * view * vec4((pos[0] + 0.5 * size[0] * vec3(1.0, 1.0, 1.0)), 1.0);


    // Emitting lines to form the box
    gl_Position = vertices[0];
    EmitVertex();
    gl_Position = vertices[1];
    EmitVertex();
    EndPrimitive();

    gl_Position = vertices[1];
    EmitVertex();
    gl_Position = vertices[3];
    EmitVertex();
    EndPrimitive();

    gl_Position = vertices[3];
    EmitVertex();
    gl_Position = vertices[2];
    EmitVertex();
    EndPrimitive();

    gl_Position = vertices[2];
    EmitVertex();
    gl_Position = vertices[0];
    EmitVertex();
    EndPrimitive();

    gl_Position = vertices[0];
    EmitVertex();
    gl_Position = vertices[4];
    EmitVertex();
    EndPrimitive();

    gl_Position = vertices[1];
    EmitVertex();
    gl_Position = vertices[5];
    EmitVertex();
    EndPrimitive();

    gl_Position = vertices[3];
    EmitVertex();
    gl_Position = vertices[7];
    EmitVertex();
    EndPrimitive();

    gl_Position = vertices[2];
    EmitVertex();
    gl_Position = vertices[6];
    EmitVertex();
    EndPrimitive();

    gl_Position = vertices[4];
    EmitVertex();
    gl_Position = vertices[5];
    EmitVertex();
    EndPrimitive();

    gl_Position = vertices[5];
    EmitVertex();
    gl_Position = vertices[7];
    EmitVertex();
    EndPrimitive();

    gl_Position = vertices[7];
    EmitVertex();
    gl_Position = vertices[6];
    EmitVertex();
    EndPrimitive();

    gl_Position = vertices[6];
    EmitVertex();
    gl_Position = vertices[4];
    EmitVertex();
    EndPrimitive();
}