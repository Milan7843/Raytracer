#include "Material.h"

#include "shaders/AbstractShader.h"
#include "Scene.h"

Material::Material()
	: name("New material"),
	color(glm::vec3(0.8f)),
	reflectiveness(0.0f),
	transparency(0.0f),
	refractiveness(0.0f),
	reflectionDiffusion(0.0f),
	emission(glm::vec3(1.0f)),
	emissionStrength(0.0f),
	fresnelReflectionStrength(0.0f)
{
	setType(MATERIAL);
}

Material::Material(std::string name, glm::vec3 color, float reflectiveness, float transparency, float refractiveness, float reflectionDiffusion, glm::vec3 emission, float emissionStrength, float fresnelReflectionStrength)
	: name(name),
	color(color),
	reflectiveness(reflectiveness),
	transparency(transparency),
	refractiveness(refractiveness),
	reflectionDiffusion(reflectionDiffusion),
	emission(emission),
	emissionStrength(emissionStrength),
	fresnelReflectionStrength(fresnelReflectionStrength)
{
	setType(MATERIAL);
}
Material::Material(std::string name, glm::vec3 color, float reflectiveness, float transparency, float refractiveness)
	: name(name),
	color(color),
	reflectiveness(reflectiveness),
	transparency(transparency),
	refractiveness(refractiveness),
	reflectionDiffusion(0.0f),
	emission(glm::vec3(1.0f)),
	emissionStrength(0.0f),
	fresnelReflectionStrength(0.0f)
{
	setType(MATERIAL);
}

Material::Material(std::string name, glm::vec3 color, float reflectiveness, float transparency, glm::vec3 emission, float emissionStrength)
	: name(name),
	color(color),
	reflectiveness(reflectiveness),
	transparency(transparency),
	refractiveness(0.0f),
	reflectionDiffusion(0.0f),
	emission(emission),
	emissionStrength(emissionStrength),
	fresnelReflectionStrength(0.0f)
{
	setType(MATERIAL);
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
	filestream << emissionStrength << "\n";
	filestream << fresnelReflectionStrength << "\n";

	filestream << m_hasAlbedoTexture << "\n";
	if (m_hasAlbedoTexture)
		filestream << albedoTexture->path << "\n";

	filestream << m_hasNormalTexture << "\n";
	if (m_hasNormalTexture)
		filestream << normalTexture->path << "\n";
}

bool Material::drawInterface(Scene& scene)
{
	bool anyPropertiesChanged{ false };

	anyPropertiesChanged |= ImGui::InputText("Name", getNamePointer());
	anyPropertiesChanged |= ImGui::ColorEdit3("Color", (float*)getColorPointer());
	anyPropertiesChanged |= ImGui::ColorEdit3("Emission", (float*)getEmissionPointer());
	anyPropertiesChanged |= ImGui::DragFloat("Emission strength", &emissionStrength, 0.01f, 0.0f, 10.0f, "%.2f");
	anyPropertiesChanged |= ImGui::DragFloat("Reflectiveness", getReflectivenessPointer(), 0.01f, 0.0f, 1.0f, "%.2f");
	anyPropertiesChanged |= ImGui::DragFloat("Transparency", getTransparencyPointer(), 0.01f, 0.0f, 1.0f, "%.2f");
	anyPropertiesChanged |= ImGui::DragFloat("Refractiveness", getRefractivenessPointer(), 0.01f, 0.0f, 1.0f, "%.2f");
	anyPropertiesChanged |= ImGui::DragFloat("Reflective diffusion", getReflectionDiffusionPointer(), 0.01f, 0.0f, 1.0f, "%.2f");
	anyPropertiesChanged |= ImGui::DragFloat("Fresnel reflection strength", &fresnelReflectionStrength, 0.01f, 0.0f, 1.0f, "%.2f");
	ImGuiUtility::drawHelpMarker("How much the reflection can be diffused. Basically acts as a blur.");

	// The albedo texture must have some data
	//if (!albedoTexture->data.empty())
	{
		ImGui::Text("Color texture");
		unsigned int previewTextureID{ 0 };
		unsigned int previewTextureWidth{ 80 };
		unsigned int previewTextureHeight{ 80 };

		if (m_hasAlbedoTexture)
		{
			previewTextureID = albedoTexture->previewTextureID;
			previewTextureWidth = albedoTexture->previewWidth;
			previewTextureHeight = albedoTexture->previewHeight;
		}

		if (ImGui::ImageButton((void*)(intptr_t)previewTextureID, ImVec2(previewTextureWidth, previewTextureHeight)))
		{
			std::string imagePath = WindowUtility::openImageFileChooseDialog();

			if (imagePath != std::string(""))
			{
				setAlbedoTexture(imagePath, false);
			}
		}
	}

	{
		ImGui::Text("Normal map");
		unsigned int previewTextureID{ 0 };
		unsigned int previewTextureWidth{ 80 };
		unsigned int previewTextureHeight{ 80 };

		if (m_hasNormalTexture)
		{
			previewTextureID = normalTexture->previewTextureID;
			previewTextureWidth = normalTexture->previewWidth;
			previewTextureHeight = normalTexture->previewHeight;
		}

		if (ImGui::ImageButton((void*)(intptr_t)previewTextureID, ImVec2(previewTextureWidth, previewTextureHeight)))
		{
			std::string imagePath = WindowUtility::openImageFileChooseDialog();

			if (imagePath != std::string(""))
			{
				setNormalTexture(imagePath, false);
			}
		}
	}

	// If anything changed, no shader will have the updated data
	if (anyPropertiesChanged)
	{
		clearShaderWrittenTo();
	}

	return anyPropertiesChanged;
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
		glm::vec3(0.0f),
		0.0f,
		0.0f);
}

bool Material::writeToShader(AbstractShader* shader, unsigned int index)
{
	if (hasWrittenToShader(shader))
	{
		// No data was updated
		return false;
	}

	//Logger::log("wrote material light to shader");

	shader->setVector3(("materials[" + std::to_string(index) + "].color").c_str(), color);
	shader->setFloat(("materials[" + std::to_string(index) + "].reflectiveness").c_str(), reflectiveness);
	shader->setFloat(("materials[" + std::to_string(index) + "].transparency").c_str(), transparency);
	shader->setFloat(("materials[" + std::to_string(index) + "].refractiveness").c_str(), refractiveness);
	shader->setFloat(("materials[" + std::to_string(index) + "].reflectionDiffusion").c_str(), reflectionDiffusion);
	shader->setVector3(("materials[" + std::to_string(index) + "].emission").c_str(), emission);
	shader->setFloat(("materials[" + std::to_string(index) + "].emissionStrength").c_str(), emissionStrength);
	shader->setFloat(("materials[" + std::to_string(index) + "].fresnelReflectionStrength").c_str(), fresnelReflectionStrength);

	// Writing texture data
	shader->setBool(("materials[" + std::to_string(index) + "].hasAlbedoTexture").c_str(), m_hasAlbedoTexture);
	shader->setBool(("materials[" + std::to_string(index) + "].hasNormalTexture").c_str(), m_hasNormalTexture);

	if (hasAlbedoTexture())
	{
		shader->setFloat(("materials[" + std::to_string(index) + "].albedoTexture_xMin").c_str(), albedoTexture->xMin);
		shader->setFloat(("materials[" + std::to_string(index) + "].albedoTexture_xMax").c_str(), albedoTexture->xMax);
		shader->setFloat(("materials[" + std::to_string(index) + "].albedoTexture_yMin").c_str(), albedoTexture->yMin);
		shader->setFloat(("materials[" + std::to_string(index) + "].albedoTexture_yMax").c_str(), albedoTexture->yMax);
	}
	if (hasNormalTexture())
	{
		shader->setFloat(("materials[" + std::to_string(index) + "].normalTexture_xMin").c_str(), normalTexture->xMin);
		shader->setFloat(("materials[" + std::to_string(index) + "].normalTexture_xMax").c_str(), normalTexture->xMax);
		shader->setFloat(("materials[" + std::to_string(index) + "].normalTexture_yMin").c_str(), normalTexture->yMin);
		shader->setFloat(("materials[" + std::to_string(index) + "].normalTexture_yMax").c_str(), normalTexture->yMax);
	}

	// The given shader now has updated data
	markShaderAsWrittenTo(shader);

	// New data was written
	return true;
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

void Material::setTexture(std::string& path, bool pixelPerfect)
{
}

bool Material::hasAlbedoTexture()
{
	return m_hasAlbedoTexture;
}

void Material::setAlbedoTexture(std::string& path, bool pixelPerfect)
{
	m_hasAlbedoTexture = true;
	albedoTexture = TextureHandler::loadTexture(path, pixelPerfect);
}

void Material::setAlbedoTexture(const char* path, bool pixelPerfect)
{
	m_hasAlbedoTexture = true;
	albedoTexture = TextureHandler::loadTexture(path, pixelPerfect);
}

Texture* Material::getAlbedoTexture()
{
	return albedoTexture.get();
}

bool Material::hasNormalTexture()
{
	return m_hasNormalTexture;
}

void Material::setNormalTexture(std::string& path, bool pixelPerfect)
{
	m_hasNormalTexture = true;
	normalTexture = TextureHandler::loadTexture(path, pixelPerfect);
}

void Material::setNormalTexture(const char* path, bool pixelPerfect)
{
	m_hasNormalTexture = true;
	normalTexture = TextureHandler::loadTexture(path, pixelPerfect);
}

Texture* Material::getNormalTexture()
{
	return normalTexture.get();
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
		<< "\nemission strength: (" << material.emissionStrength << ")"
		<< "\n fresnel reflection strength: (" << material.fresnelReflectionStrength << ")"
		<< std::endl;
	
	return stream;
}

