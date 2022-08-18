#include "Sphere.h"

Sphere::Sphere(std::string& name, glm::vec3 position, float radius, unsigned int materialIndex)
	: Model(materialIndex, "src/models/defaultSphere.obj", &meshCount, &triangleCount, 1),
	shaderSphereIndex(0) // will be set later
{
	this->move(position);
	this->scale(radius);
	this->name = name;
}

Sphere::Sphere(glm::vec3 position, float radius, unsigned int materialIndex, unsigned int shaderSphereIndex)
	: Model(materialIndex, "src/models/defaultSphere.obj", &meshCount, &triangleCount, 1)
	, shaderSphereIndex(shaderSphereIndex)
{
	this->move(position);
	this->scale(radius);
	this->name = "New sphere";
}

Sphere::~Sphere()
{

}

void Sphere::writeDataToStream(std::ofstream& filestream)
{
	Object::writeDataToStream(filestream);

	//filestream << radius << "\n";
	filestream << materialIndex << "\n";
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
	//Object::scale(scale);
	this->radius *= scale;
}

void Sphere::scale(glm::vec3 scale)
{
	Logger::logWarning("Scale with vector was called on a sphere. This is not possible and should be avoided.");
}

void Sphere::setShaderSphereIndex(unsigned int shaderSphereIndex)
{
	this->shaderSphereIndex = shaderSphereIndex;
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

unsigned int* Sphere::getMaterialIndexPointer()
{
	return &materialIndex;
}

std::ostream& operator<<(std::ostream& stream, const Sphere& sphere)
{
	// Writing this object to the stream
	stream << "[Sphere]"
		<< "\nposition: (" << sphere.position.x << ", " << sphere.position.y << ", " << sphere.position.z << ")"
		<< "\nradius: " << sphere.radius
		<< "\nmaterial: " << sphere.materialIndex
		<< std::endl;

	return stream;
}
