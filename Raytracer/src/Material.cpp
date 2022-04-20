#include "Material.h"

#include "Shader.h"

Material::Material(glm::vec3 color, float reflectiveness, float transparency)
	: color(color),
	reflectiveness(reflectiveness),
	transparency(transparency)
{
}

Material::~Material()
{

}

void Material::writeToShader(Shader* shader, unsigned int index)
{
	shader->setVector3(("materials[" + std::to_string(index) + "].color").c_str(), color);
	shader->setFloat(("materials[" + std::to_string(index) + "].reflectiveness").c_str(), reflectiveness);
	shader->setFloat(("materials[" + std::to_string(index) + "].transparency").c_str(), transparency);
}
