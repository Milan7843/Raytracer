#include "ImageLoader.h"

#define STB_IMAGE_IMPLEMENTATION
#include "../stb_image.h"

namespace ImageLoader
{
	Texture loadTexture(std::string imagePath, bool pixelPerfect)
	{
		std::string fileName = imagePath;

		unsigned int textureID;
		glGenTextures(1, &textureID);

		int width, height, nrComponents;
		unsigned char* data = stbi_load(fileName.c_str(), &width, &height, &nrComponents, 0);
		
		if (data)
		{
			size_t dataSize = width * height * nrComponents;

			// Use vector constructor to copy the data from the pointer
			std::vector<unsigned char> dataVector(data, data + dataSize);

			stbi_image_free(data);

			return Texture{
				imagePath,
				pixelPerfect,
				width,
				height,
				0.0f,
				0.0f,
				0.0f,
				0.0f,
				dataVector
			};
		}
		else
		{
			Logger::logError("Error loading texture " + imagePath);
		}

		return Texture{
			imagePath,
			pixelPerfect,
			width,
			height,
			0.0f,
			0.0f,
			0.0f,
			0.0f,
			std::vector<unsigned char>()
		};
	}

	unsigned int loadImage(std::string imagePath, bool pixelPerfect)
	{
		std::string fileName = imagePath;

		unsigned int textureID;
		glGenTextures(1, &textureID);

		int width, height, nrComponents;
		unsigned char* data = stbi_load(fileName.c_str(), &width, &height, &nrComponents, 0);
		stbi_set_flip_vertically_on_load(true);
		if (data)
		{
			GLenum format;
			if (nrComponents == 1)
				format = GL_RED;
			else if (nrComponents == 3)
				format = GL_RGB;
			else if (nrComponents == 4)
				format = GL_RGBA;

			glBindTexture(GL_TEXTURE_2D, textureID);
			glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
			glGenerateMipmap(GL_TEXTURE_2D);


			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, pixelPerfect ? GL_NEAREST_MIPMAP_NEAREST : GL_LINEAR_MIPMAP_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, pixelPerfect ? GL_NEAREST : GL_LINEAR);
		}
		else
		{
			stbi_image_free(data);
			throw std::exception(stbi_failure_reason());
		}

		stbi_image_free(data);

		return textureID;
	}
}