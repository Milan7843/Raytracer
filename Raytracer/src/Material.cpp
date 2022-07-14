#include "Material.h"

#include "AbstractShader.h"

Material::Material(glm::vec3 color, float reflectiveness, float transparency, float refractiveness)
	: color(color),
	reflectiveness(reflectiveness),
	transparency(transparency),
	refractiveness(refractiveness),
	emission(glm::vec3(1.0f))
{
}

Material::Material(glm::vec3 color, float reflectiveness, float transparency, glm::vec3 emission)
	: color(color),
	reflectiveness(reflectiveness),
	transparency(transparency),
	refractiveness(0.0f),
	emission(emission)
{
}

Material::~Material()
{

}

void Material::writeToShader(AbstractShader* shader, unsigned int index)
{
	shader->setVector3(("materials[" + std::to_string(index) + "].color").c_str(), color);
	shader->setFloat(("materials[" + std::to_string(index) + "].reflectiveness").c_str(), reflectiveness);
	shader->setFloat(("materials[" + std::to_string(index) + "].transparency").c_str(), transparency);
	shader->setFloat(("materials[" + std::to_string(index) + "].refractiveness").c_str(), refractiveness);
	shader->setVector3(("materials[" + std::to_string(index) + "].emission").c_str(), emission);
}
