#version 460 core
layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aNormal;
layout(location = 2) in vec2 uv_in;
layout(location = 3) in vec3 aTangent;
layout(location = 4) in vec3 aBitangent;

out vec3 FragPos;
out vec3 Color;
out vec3 Normal;
out vec2 uv;
out mat3 tbn;

uniform mat4 model;
uniform mat4 rotation; // Only the rotation part of the model matrix
uniform mat4 view;
uniform mat4 projection;
uniform vec3 inputColor;

void main()
{
	// Applying the model matrix to apply model transformations
	FragPos = vec3(model * vec4(aPos, 1.0));

	// Calculating screen position
	gl_Position = projection * view * vec4(FragPos, 1.0);

    Color = inputColor;

	uv = uv_in;

	// Rotating the model according to the model's rotation
	Normal = normalize(vec3(rotation * vec4(aNormal, 1.0)));

	vec3 T = normalize(vec3(model * vec4(aTangent, 0.0)));
	vec3 B = normalize(vec3(model * vec4(aBitangent, 0.0)));
	vec3 N = normalize(vec3(model * vec4(aNormal, 0.0)));
	tbn = mat3(T, B, N);
}