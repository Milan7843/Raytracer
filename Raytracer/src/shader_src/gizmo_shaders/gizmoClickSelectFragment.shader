#version 460 core

in vec2 texCoord;
flat in int objectID;

// Writing to the texture
layout(location = 0) out vec4 colors;

/* Object encoding into color scheme:
* The red channel holds the index of the object [0, 255]
*
*/

void main()
{
    colors = vec4(objectID / 255.0, 0.0, 0.0, 1.0);
}
