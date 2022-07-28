#include "Material.h"

#include "AbstractShader.h"

Material::Material(std::string name, glm::vec3 color, float reflectiveness, float transparency, float refractiveness, glm::vec3 emission)
	: name(name),
	color(color),
	reflectiveness(reflectiveness),
	transparency(transparency),
	refractiveness(refractiveness),
	emission(emission)
{
}
Material::Material(std::string name, glm::vec3 color, float reflectiveness, float transparency, float refractiveness)
	: name(name),
	color(color),
	reflectiveness(reflectiveness),
	transparency(transparency),
	refractiveness(refractiveness),
	emission(glm::vec3(1.0f))
{
}

Material::Material(std::string name, glm::vec3 color, float reflectiveness, float transparency, glm::vec3 emission)
	: name(name),
	color(color),
	reflectiveness(reflectiveness),
	transparency(transparency),
	refractiveness(0.0f),
	emission(emission)
{
}

Material::~Material()
{

}

void Material::writeDataToStream(std::ofstream& filestream)
{
	filestream << name << "\n";
	filestream << color.r << " " << color.g << " " << color.b << "\n";
	filestream << reflectiveness << "\n";
	filestream << transparency << "\n";
	filestream << refractiveness << "\n";
	filestream << emission.r << " " << emission.g << " " << emission.b << "\n";

}

void Material::writeToShader(AbstractShader* shader, unsigned int index)
{
	shader->setVector3(("materials[" + std::to_string(index) + "].color").c_str(), color);
	shader->setFloat(("materials[" + std::to_string(index) + "].reflectiveness").c_str(), reflectiveness);
	shader->setFloat(("materials[" + std::to_string(index) + "].transparency").c_str(), transparency);
	shader->setFloat(("materials[" + std::to_string(index) + "].refractiveness").c_str(), refractiveness);
	shader->setVector3(("materials[" + std::to_string(index) + "].emission").c_str(), emission);
}

std::string* Material::getNamePointer()
{
	return &name;
}

glm::vec3* Material::getColorPointer()
{
	return &color;
}

float* Material::getReflectivenessPointer()
{
	return &reflectiveness;
}

float* Material::getTransparencyPointer()
{
	return &transparency;
}

float* Material::getRefractivenessPointer()
{
	return &refractiveness;
}

glm::vec3* Material::getEmissionPointer()
{
	return &emission;
}
