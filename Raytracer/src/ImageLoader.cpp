#include "ImageLoader.h"

#define STB_IMAGE_IMPLEMENTATION
#include "../stb_image.h"

namespace ImageLoader
{
	std::shared_ptr<AtlasTexture> loadAtlasTexture(std::string imagePath, bool pixelPerfect, float previewAspectRatio)
	{
		std::string fileName = imagePath;

		int width, height, nrComponents;
		stbi_set_flip_vertically_on_load(true);
		unsigned char* data = stbi_load(fileName.c_str(), &width, &height, &nrComponents, 0);
		
		if (data)
		{
			size_t dataSize = width * height * nrComponents;

			// Use vector constructor to copy the data from the pointer
			std::vector<unsigned char> dataVector(data, data + dataSize);

			stbi_image_free(data);

			// Creating preview data from the original data vector
			int previewWidth = (int)(80 * previewAspectRatio);
			int previewHeight = 80;

			std::vector<unsigned char> previewDataVector(previewWidth * previewHeight * nrComponents);

			copyImageToFixedSize(width, height, previewWidth, previewHeight, nrComponents, dataVector, previewDataVector);

			unsigned int previewTextureID{ 0 };

			glGenTextures(1, &previewTextureID);
			glBindTexture(GL_TEXTURE_2D, previewTextureID);

			if (nrComponents == 3)
			{
				glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, previewWidth, previewHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, previewDataVector.data());
			}
			else if (nrComponents == 4)
			{
				glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, previewWidth, previewHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, previewDataVector.data());
			}
			else
			{
				Logger::logError("Texture loaded with invalid number of channels");
			}

			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glBindTexture(GL_TEXTURE_2D, 0);

			return std::make_shared<AtlasTexture>(
				imagePath,
				pixelPerfect,
				width,
				height,
				previewWidth,
				previewHeight,
				0.0f,
				0.0f,
				0.0f,
				0.0f,
				nrComponents,
				dataVector,
				previewTextureID
			);
		}
		else
		{
			Logger::logError("Error loading texture " + imagePath);
		}

		return std::make_shared<AtlasTexture>(
			imagePath,
			pixelPerfect,
			width,
			height,
			0,
			0,
			0.0f,
			0.0f,
			0.0f,
			0.0f,
			0,
			std::vector<unsigned char>(),
			0
		);
	}

	std::shared_ptr<Texture> loadTexture(std::string imagePath, bool pixelPerfect, float previewAspectRatio)
	{
		std::string fileName = imagePath;

		int width, height, nrComponents;
		stbi_set_flip_vertically_on_load(true);
		unsigned char* data = stbi_load(fileName.c_str(), &width, &height, &nrComponents, 0);

		if (data)
		{
			size_t dataSize = width * height * nrComponents;

			// Use vector constructor to copy the data from the pointer
			std::vector<unsigned char> dataVector(data, data + dataSize);

			stbi_image_free(data);

			unsigned int textureID{ 0 };

			glGenTextures(1, &textureID);
			glBindTexture(GL_TEXTURE_2D, textureID);

			if (nrComponents == 3)
			{
				glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, dataVector.data());
			}
			else if (nrComponents == 4)
			{
				glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, dataVector.data());
			}
			else
			{
				Logger::logError("Texture loaded with invalid number of channels");
			}

			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glBindTexture(GL_TEXTURE_2D, 0);

			// Creating preview data from the original data vector
			int previewWidth = (int)(80 * previewAspectRatio);
			int previewHeight = 80;

			std::vector<unsigned char> previewDataVector(previewWidth * previewHeight * nrComponents);

			copyImageToFixedSize(width, height, previewWidth, previewHeight, nrComponents, dataVector, previewDataVector);

			unsigned int previewTextureID{ 0 };

			glGenTextures(1, &previewTextureID);
			glBindTexture(GL_TEXTURE_2D, previewTextureID);

			if (nrComponents == 3)
			{
				glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, previewWidth, previewHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, previewDataVector.data());
			}
			else if (nrComponents == 4)
			{
				glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, previewWidth, previewHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, previewDataVector.data());
			}
			else
			{
				Logger::logError("Texture loaded with invalid number of channels");
			}

			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glBindTexture(GL_TEXTURE_2D, 0);

			return std::make_shared<Texture>(
				imagePath,
				pixelPerfect,
				width,
				height,
				previewWidth,
				previewHeight,
				0.0f,
				0.0f,
				0.0f,
				0.0f,
				nrComponents,
				dataVector,
				previewTextureID,
				textureID
			);
		}
		else
		{
			Logger::logError("Error loading texture " + imagePath);
		}

		return std::make_shared<Texture>(
			imagePath,
			pixelPerfect,
			width,
			height,
			0,
			0,
			0.0f,
			0.0f,
			0.0f,
			0.0f,
			0,
			std::vector<unsigned char>(),
			0,
			0
			);
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

	void copyImageToFixedSize(int width, int height, int newWidth, int newHeight, int nrComponents,
		const std::vector<unsigned char>& data,
		std::vector<unsigned char>& newData)
	{
		// Calculate scaling factors
		float scaleX = (float)width / newWidth;
		float scaleY = (float)height / newHeight;

		if (newWidth == 0 || newHeight == 0)
		{
			return;
		}

		// Loop through the new image dimensions
		for (int y = 0; y < newHeight; ++y)
		{
			for (int x = 0; x < newWidth; ++x)
			{
				// Calculate the starting pixel in the original image
				int startX = static_cast<int>(x * scaleX);
				int startY = static_cast<int>(y * scaleY);

				// Flipping the image
				y = newHeight - 1 - y;

				// Average the pixel values in the original image to determine the pixel value in the resized image
				for (int c = 0; c < nrComponents; ++c)
				{
					// Calculate the average pixel value for the resized image
					newData[(y * newWidth + x) * nrComponents + c] = data[(startX + startY * width) * nrComponents + c];
				}
			}
		}
	}
}