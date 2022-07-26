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

void Sphere::writeToShader(AbstractShader* shader, unsigned int ssbo)
{
	shader->setVector3(("spheres[" + std::to_string(shaderSphereIndex) + "].pos").c_str(), 
		CoordinateUtility::vec4ToGLSLVec4(getTransformationMatrix() * glm::vec4(0.0f, 0.0f, 0.0f, 1.0f)));
	shader->setFloat(("spheres[" + std::to_string(shaderSphereIndex) + "].radius").c_str(), radius);
	shader->setInt(("spheres[" + std::to_string(shaderSphereIndex) + "].material").c_str(), materialIndex);
}

void Sphere::scale(float scale)
{
	Object::scale(scale);
	this->radius *= scale;
}

void Sphere::scale(glm::vec3 scale)
{
	std::cout << "Scale with vector was called on a sphere. This is not possible and should be avoided." << std::endl;
}

glm::mat4 Sphere::getTransformationMatrix()
{
	// Get the regular transformation matrix
	glm::mat4 base = Object::getTransformationMatrix();

	// Then use the radius to scale it
	return glm::scale(base, glm::vec3(radius));
}

float* Sphere::getRadiusPointer()
{
	return &radius;
}
