#version 460 core
layout(location = 0) in vec3 inPosition;
layout(location = 1) in int objectID;
out int toGeo_objectID;

uniform mat4 projection;
uniform mat4 view;

void main()
{
    vec4 viewPosition = view * vec4(inPosition, 1.0);
    gl_Position = projection * viewPosition;
    toGeo_objectID = objectID;
}