#include "TextureHandler.h"

#include "Material.h"

namespace TextureHandler
{
	namespace
	{
		bool newTextureLoaded{ true };

		unsigned int packedTextureID{ 0 };
	}

	std::shared_ptr<AtlasTexture> loadAtlasTexture(const std::string& spritePath, bool pixelPerfect, float previewAspectRatio)
	{
		newTextureLoaded = true;
		return ImageLoader::loadAtlasTexture(spritePath, pixelPerfect, previewAspectRatio);
	}

	std::shared_ptr<AtlasTexture> loadAtlasTexture(const char* spritePath, bool pixelPerfect, float previewAspectRatio)
	{
		return loadAtlasTexture(std::string(spritePath), pixelPerfect, previewAspectRatio);
	}

	std::shared_ptr<Texture> loadTexture(const std::string& spritePath, bool pixelPerfect, float previewAspectRatio)
	{
		return ImageLoader::loadTexture(spritePath, pixelPerfect, previewAspectRatio);
	}

	std::shared_ptr<Texture> loadTexture(const char* spritePath, bool pixelPerfect, float previewAspectRatio)
	{
		return loadTexture(std::string(spritePath), pixelPerfect, previewAspectRatio);
	}

	unsigned int packTextures(std::vector<Material>& materials)
	{
		if (!newTextureLoaded)
		{
			return packedTextureID;
		}

		if (packedTextureID != 0)
		{
			glDeleteTextures(1, &packedTextureID);
		}
		

		std::vector<AtlasTexture*> textures = std::vector<AtlasTexture*>();
		
		for (Material& material : materials)
		{
			if (material.hasAlbedoTexture())
			{
				std::cout << "material has albedo texture" << std::endl;

				textures.push_back(material.getAlbedoTexture());
			}
			if (material.hasNormalTexture())
			{
				textures.push_back(material.getNormalTexture());
			}
		}
		
		unsigned int maxHeight{ 0 };
		unsigned int totalWidth{ 0 };

		// Finding the final texture size: the maximum height and the total width
		for (AtlasTexture* texture : textures)
		{
			if (texture->height > maxHeight)
			{
				maxHeight = texture->height;
			}

			totalWidth += texture->width;
		}

		unsigned int atlasTextureHeight{ maxHeight };
		unsigned int atlasTextureWidth{ totalWidth };
		
		// TODO shouldnt have to be 40x but crashes renderdoc??
		unsigned char* data = (unsigned char*)malloc(atlasTextureHeight * atlasTextureWidth * sizeof(unsigned char) * 4);

		unsigned int currentX{ 0 };

		std::cout << "Creating texture atlas of size " << atlasTextureWidth << "x" << atlasTextureHeight << " (" << (atlasTextureHeight * atlasTextureWidth * sizeof(unsigned char) * 4) << " bytes)" << std::endl;

		for (AtlasTexture* texture : textures)
		{
			// Adding the texture to the atlas
			for (unsigned int sourceY{ 0 }; sourceY < texture->height; sourceY++)
			{
				for (unsigned int sourceX{ 0 }; sourceX < texture->width; sourceX++)
				{
					unsigned int atlasY{ sourceY };
					unsigned int atlasX{ currentX + sourceX };

					for (unsigned int channel{ 0 }; channel < 4; channel++)
					{
						unsigned int atlasDataIndex{ (atlasX + atlasY * atlasTextureWidth) * 4 + channel };

						unsigned int sourceDataIndex{ sourceX + sourceY * texture->width };

						if (texture->components == 3)
						{
							sourceDataIndex *= 3;
							sourceDataIndex += channel;

							if (channel == 3)
							{
								// Predefined fourth channel value
								data[atlasDataIndex] = 0;
							}
							else
							{
								data[atlasDataIndex] = texture->data[sourceDataIndex];
							}
						}
						else if (texture->components == 4)
						{
							sourceDataIndex *= 4;
							sourceDataIndex += channel;

							data[atlasDataIndex] = texture->data[sourceDataIndex];
						}
						else
						{
							Logger::logError("Image encountered with " + std::to_string(texture->components) + " components, which is not supported. Images must have 3 components (RGB) or 4 components (RGBA)");
						}
					}
				}
			}

			// Setting the Height compared to the atlas
			texture->xMin = (float)currentX / atlasTextureWidth;
			texture->xMax = (float)(currentX + texture->width) / atlasTextureWidth;
			texture->yMin = 0.0f;
			texture->yMax = (float)texture->height / atlasTextureHeight;

			// Moving to the right
			currentX += texture->width;
		}

		Logger::log(std::string("Making atlas texture of size ") + std::to_string(atlasTextureWidth) + " by " + std::to_string(atlasTextureHeight));

		// Putting the packed data into a texture

		glGenTextures(1, &packedTextureID);
		glBindTexture(GL_TEXTURE_2D, packedTextureID);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, atlasTextureWidth, atlasTextureHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glBindTexture(GL_TEXTURE_2D, 0);

		for (Material& material : materials)
		{
			if (material.hasAlbedoTexture() || material.hasNormalTexture())
			{
				material.clearShaderWrittenTo();
			}
		}

		newTextureLoaded = false;
		
		return packedTextureID;
	}

	void textureRemoved()
	{
		newTextureLoaded = true;
	}
};
