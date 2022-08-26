#include "Material.h"

#include "AbstractShader.h"

Material::Material()
	: name("Material 1"),
	color(glm::vec3(0.8f)),
	reflectiveness(0.0f),
	transparency(0.0f),
	refractiveness(0.0f),
	reflectionDiffusion(0.0f),
	emission(glm::vec3(0.0f))
{
}

Material::Material(std::string name, glm::vec3 color, float reflectiveness, float transparency, float refractiveness, float reflectionDiffusion, glm::vec3 emission)
	: name(name),
	color(color),
	reflectiveness(reflectiveness),
	transparency(transparency),
	refractiveness(refractiveness),
	reflectionDiffusion(reflectionDiffusion),
	emission(emission)
{
}
Material::Material(std::string name, glm::vec3 color, float reflectiveness, float transparency, float refractiveness)
	: name(name),
	color(color),
	reflectiveness(reflectiveness),
	transparency(transparency),
	refractiveness(refractiveness),
	reflectionDiffusion(0.0f),
	emission(glm::vec3(1.0f))
{
}

Material::Material(std::string name, glm::vec3 color, float reflectiveness, float transparency, glm::vec3 emission)
	: name(name),
	color(color),
	reflectiveness(reflectiveness),
	transparency(transparency),
	refractiveness(0.0f),
	reflectionDiffusion(0.0f),
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
	filestream << reflectionDiffusion << "\n";
	filestream << emission.r << " " << emission.g << " " << emission.b << "\n";

}

void Material::drawInterface(Scene& scene)
{
	ImGui::InputText("Name", getNamePointer());
	ImGui::ColorEdit3("Color", (float*)getColorPointer());
	ImGui::ColorEdit3("Emission", (float*)getEmissionPointer());
	ImGui::DragFloat("Reflectiveness", getReflectivenessPointer(), 0.01f, 0.0f, 1.0f, "%.2f");
	ImGui::DragFloat("Transparency", getTransparencyPointer(), 0.01f, 0.0f, 1.0f, "%.2f");
	ImGui::DragFloat("Refractiveness", getRefractivenessPointer(), 0.01f, 0.0f, 1.0f, "%.2f");
	ImGui::DragFloat("Reflective diffusion", getReflectionDiffusionPointer(), 0.01f, 0.0f, 1.0f, "%.2f");
	ImGuiUtility::drawHelpMarker("How much the reflection can be diffused. Basically acts as a blur.");
}

Material Material::generateErrorMaterial()
{
	return Material(
		"None",
		glm::vec3(1.0f, 0.18f, 0.9f),
		0.0f,
		0.0f,
		0.0f,
		0.0f,
		glm::vec3(0.0f));
}

void Material::writeToShader(AbstractShader* shader, unsigned int index)
{
	shader->setVector3(("materials[" + std::to_string(index) + "].color").c_str(), color);
	shader->setFloat(("materials[" + std::to_string(index) + "].reflectiveness").c_str(), reflectiveness);
	shader->setFloat(("materials[" + std::to_string(index) + "].transparency").c_str(), transparency);
	shader->setFloat(("materials[" + std::to_string(index) + "].refractiveness").c_str(), refractiveness);
	shader->setFloat(("materials[" + std::to_string(index) + "].reflectionDiffusion").c_str(), reflectionDiffusion);
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

float* Material::getReflectionDiffusionPointer()
{
	return &reflectionDiffusion;
}

glm::vec3* Material::getEmissionPointer()
{
	return &emission;
}

std::ostream& operator<<(std::ostream& stream, const Material& material)
{
	// Writing this object to the stream
	stream << "[Material] name: " << material.name
		<< "\ncolor: (" << material.color.x << ", " << material.color.y << ", " << material.color.z << ")"
		<< "\nreflectiveness: " << material.reflectiveness
		<< "\nrefractiveness: " << material.refractiveness
		<< "\ntransparency: " << material.transparency
		<< "\nemission: (" << material.emission.x << ", " << material.emission.y << ", " << material.emission.z << ")"
		<< std::endl;

	return stream;
}

