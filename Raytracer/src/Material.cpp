#include "Material.h"

#include "Shader.h"

Material::Material(glm::vec3 color, float reflectiveness, float transparency)
	: color(color),
	reflectiveness(reflectiveness),
	transparency(transparency),
	emission(glm::vec3(1.0f))
{
}

Material::Material(glm::vec3 color, float reflectiveness, float transparency, glm::vec3 emission)
	: color(color),
	reflectiveness(reflectiveness),
	transparency(transparency),
	emission(emission)
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
	shader->setVector3(("materials[" + std::to_string(index) + "].emission").c_str(), emission);
}
