#include "TextureHandler.h"

namespace TextureHandler
{
	Texture loadTexture(const std::string& spritePath, bool pixelPerfect)
	{
		unsigned int id{ ImageLoader::loadImage(spritePath, pixelPerfect) };

		Texture texture{ spritePath, pixelPerfect, id };

		return texture;
	}
};
