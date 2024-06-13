#include "Sphere.h"

Sphere::Sphere(unsigned int materialIndex)
	: Model("src/models/defaultSphere.obj", &meshCount, &triangleCount, 1),
	shaderSphereIndex(0) // will be set later
{
	this->scale(1.0f);
	this->name = "Sphere";
	this->setMaterialIndex(materialIndex);
	setType(SPHERE);
}

Sphere::Sphere(std::string& name, glm::vec3 position, float radius, unsigned int materialIndex)
	: Model("src/models/defaultSphere.obj", &meshCount, &triangleCount, 1),
	shaderSphereIndex(0) // will be set later
{
	this->move(position);
	this->scale(radius);
	this->name = name;
	this->setMaterialIndex(materialIndex);
	setType(SPHERE);
}

Sphere::Sphere(glm::vec3 position, float radius, unsigned int materialIndex, unsigned int shaderSphereIndex)
	: Model("src/models/defaultSphere.obj", &meshCount, &triangleCount, 1)
	, shaderSphereIndex(shaderSphereIndex)
{
	this->move(position);
	this->scale(radius);
	this->name = "New sphere";
	this->setMaterialIndex(materialIndex);
	setType(SPHERE);
}

Sphere::~Sphere()
{

}

bool Sphere::writeToShader(AbstractShader* shader, unsigned int ssbo)
{
	if (hasWrittenToShader(shader))
	{
		// No data was updated
		return false;
	}

	shader->setVector3(("spheres[" + std::to_string(shaderSphereIndex) + "].pos").c_str(), 
		CoordinateUtility::vec4ToGLSLVec4(getTransformationMatrix() * glm::vec4(0.0f, 0.0f, 0.0f, 1.0f)));
	shader->setFloat(("spheres[" + std::to_string(shaderSphereIndex) + "].radius").c_str(), radius);
	shader->setInt(("spheres[" + std::to_string(shaderSphereIndex) + "].material").c_str(), meshes[0].getMaterialIndex());

	// The given shader now has updated data
	markShaderAsWrittenTo(shader);

	// New data was written
	return true;
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

void Sphere::drawInterface()
{
	bool anyPropertiesChanged{ false };

	anyPropertiesChanged |= ImGui::InputText("Name##", getNamePointer());

	// Showing transformations
	anyPropertiesChanged |= ImGui::DragFloat3("Position##", (float*)getPositionPointer(), 0.01f);

	// Drawing a dragfloat for the radius
	anyPropertiesChanged |= ImGui::DragFloat("Radius##", getRadiusPointer(), 0.01f, 0.01f, 100.0f, "%.02f");

	// Draw the first and only mesh
	//drawMesh(getMeshes()[0], scene, materialSlotsCharArray, index);

	// If anything changed, no shader will have the updated data
	if (anyPropertiesChanged)
	{
		clearShaderWrittenTo();
	}

	if (anyPropertiesChanged)
	{
		markUnsavedChanges();
	}
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

float Sphere::getRadius() const
{
	return radius;
}

unsigned int Sphere::getMaterialIndex() const
{
	return meshes[0].getMaterialIndex();
}

void Sphere::setRadius(float radius)
{
	this->radius = radius;
}

std::ostream& operator<<(std::ostream& stream, const Sphere& sphere)
{
	// Writing this object to the stream
	stream << "[Sphere]"
		<< "\nposition: (" << sphere.position.x << ", " << sphere.position.y << ", " << sphere.position.z << ")"
		<< "\nradius: " << sphere.radius
		<< "\nmaterial: " << sphere.meshes[0].getMaterialIndex()
		<< std::endl;

	return stream;
}
