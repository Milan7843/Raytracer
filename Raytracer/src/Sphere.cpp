#include "Sphere.h"

Sphere::Sphere(glm::vec3 position, float radius, unsigned int materialIndex, unsigned int shaderSphereIndex)
	: Model("src/models/defaultSphere.obj", &meshCount, &triangleCount, materialIndex),
	shaderSphereIndex(shaderSphereIndex)
{
	this->move(position);
	this->scale(radius);
}

Sphere::~Sphere()
{

}

void Sphere::writeToShader(Shader* shader, unsigned int ssbo)
{
	shader->setVector3(("spheres[" + std::to_string(shaderSphereIndex) + "].pos").c_str(), 
		transformation * glm::vec4(0.0f, 0.0f, 0.0f, 1.0f));
	shader->setFloat(("spheres[" + std::to_string(shaderSphereIndex) + "].radius").c_str(), radius);
	shader->setInt(("spheres[" + std::to_string(shaderSphereIndex) + "].material").c_str(), materialIndex);
}

void Sphere::scale(float scale)
{
	Object::scale(scale);
	this->radius *= scale;
}
