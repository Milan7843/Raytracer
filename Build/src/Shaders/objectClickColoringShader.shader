#version 460 core

// Writing to the texture
layout(location = 0) out vec4 colors;

in vec3 FragPos;
in vec3 Normal;

uniform int objectIndex;
uniform int objectType;

/* Object encoding into color scheme:
* The red channel holds the type of object:
* - model: 1
* - sphere: 2
* The green channel holds the index of the object [0, 255]
* 
*/

void main()
{
    colors = vec4(objectType/255.0, objectIndex/255.0, 0.0, 1.0);
    //colors = vec4(1.0, 1.0, 0.0, 1.0);
}
