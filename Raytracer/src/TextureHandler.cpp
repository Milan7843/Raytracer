#include "TextureHandler.h"

#include "Material.h"

namespace TextureHandler
{
	namespace
	{
		bool newTextureLoaded{ true };

		unsigned int packedTextureID{ 0 };
	}

	Texture loadTexture(const std::string& spritePath, bool pixelPerfect)
	{
		newTextureLoaded = true;
		return ImageLoader::loadTexture(spritePath, pixelPerfect);
	}

	Texture loadTexture(const char* spritePath, bool pixelPerfect)
	{
		return loadTexture(std::string(spritePath), pixelPerfect);
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
		

		std::vector<Texture*> textures = std::vector<Texture*>();
		
		for (Material& material : materials)
		{
			if (material.hasAlbedoTexture())
			{
				std::cout << "material has albedo texture" << std::endl;

				textures.push_back(&material.getAlbedoTexture());
			}
			if (material.hasNormalTexture())
			{
				textures.push_back(&material.getNormalTexture());
			}
		}
		
		unsigned int maxHeight{ 0 };
		unsigned int totalWidth{ 0 };

		// Finding the final texture size: the maximum height and the total width
		for (Texture* texture : textures)
		{
			if (texture->height > maxHeight)
			{
				maxHeight = texture->height;
			}

			totalWidth += texture->width;
		}

		unsigned int atlasTextureHeight{ maxHeight };
		unsigned int atlasTextureWidth{ totalWidth };
		
		unsigned char* data = (unsigned char*)malloc(atlasTextureHeight * atlasTextureWidth * sizeof(unsigned char) * 4);

		unsigned int currentX{ 0 };

		for (Texture* texture : textures)
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

						unsigned int sourceDataIndex{ (sourceX + sourceY * texture->width) * 4 + channel };

						data[atlasDataIndex] = texture->data[sourceDataIndex];
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
};
