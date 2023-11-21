#pragma once

#include "ShaderWritable.h"
#include "ImGuiUtility.h"
#include "ImGuiEditorInterface.h"
#include "TextureHandler.h"

#include <glm/glm.hpp>
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>

#include "WindowUtility.h"

// Forward declaration of the Shader class
class AbstractShader;

class Scene;

class Material : public ShaderWritable, public ImGuiEditorInterface
{
public:
	// Default constructor for new material
	Material();

	// Actual constructors
	Material(std::string name, glm::vec3 color, float reflectiveness, float transparency, float refractiveness, float reflectionDiffusion, glm::vec3 emission, float emissionStrength, float fresnelReflectionStrength);
	Material(std::string name, glm::vec3 color, float reflectiveness, float transparency, glm::vec3 emission, float emissionStrength);
	Material(std::string name, glm::vec3 color, float reflectiveness, float transparency, float refractiveness);
	~Material();

	// Write this light to the given filestream
	void writeDataToStream(std::ofstream& filestream);

	// Draw an interface for this model using ImGui
	virtual bool drawInterface(Scene& scene);

	// Generate a material with easily distinguishable error properties
	static Material generateErrorMaterial();

	glm::vec3 color;
	float reflectiveness;
	float transparency;
	float refractiveness;
	float reflectionDiffusion;
	glm::vec3 emission;
	float emissionStrength;
	float fresnelReflectionStrength;

	// Getters
	glm::vec3 getColor() const;
	float getReflectiveness() const;
	float getTransparency() const;
	float getRefractiveness() const;
	float getReflectionDiffusion() const;
	glm::vec3 getEmission() const;
	float getEmissionStrength() const;
	float getFresnelReflectionStrength() const;

	// Setters
	void setColor(const glm::vec3& newColor);
	void setReflectiveness(float newReflectiveness);
	void setTransparency(float newTransparency);
	void setRefractiveness(float newRefractiveness);
	void setReflectionDiffusion(float newReflectionDiffusion);
	void setEmission(const glm::vec3& newEmission);
	void setEmissionStrength(float newEmissionStrength);
	void setFresnelReflectionStrength(float newFresnelReflectionStrength);

	// Write this material into the shader at the provided index
	// Returns whether any data was written
	bool writeToShader(AbstractShader* shader, unsigned int index);

	// Get a pointer to the name of this material
	std::string* getNamePointer();
	glm::vec3* getColorPointer();
	float* getReflectivenessPointer();
	float* getTransparencyPointer();
	float* getRefractivenessPointer();
	float* getReflectionDiffusionPointer();
	glm::vec3* getEmissionPointer();

	void setTexture(std::string& path, bool pixelPerfect);

	bool hasAlbedoTexture();
	void setAlbedoTexture(std::string& path, bool pixelPerfect);
	void setAlbedoTexture(const char* path, bool pixelPerfect);
	void removeAlbedoTexture();
	AtlasTexture* getAlbedoTexture();

	bool hasNormalTexture();
	void setNormalTexture(std::string& path, bool pixelPerfect);
	void setNormalTexture(const char* path, bool pixelPerfect);
	void removeNormalTexture();
	void setNormalMapStrength(float strength);
	float getNormalMapStrength() const;
	AtlasTexture* getNormalTexture();

	// Write this material to the stream (human readable format)
	friend std::ostream& operator<< (std::ostream& stream, const Material& material);
protected:


private:
	std::string name;
	bool m_hasAlbedoTexture;
	std::shared_ptr<AtlasTexture> albedoTexture;
	bool m_hasNormalTexture;
	std::shared_ptr<AtlasTexture> normalTexture;
	float normalMapStrength;
};