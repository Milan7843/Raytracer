#include "Cache.h"

namespace Cache
{
	void initialise()
	{
		if (CreateDirectory(L"cache", NULL) ||
			ERROR_ALREADY_EXISTS == GetLastError())
		{
		}
		else
		{
		}
	}

	bool cachedFileExists(std::string& path)
	{
		std::ifstream file(path);
		return file.good();
	}

	void cacheShader(std::string& binaryPath, GLenum binaryFormat, GLvoid* binaryData, GLint binaryLength)
	{
		std::ofstream binaryFile(binaryPath.c_str(), std::ios::binary | std::ios::out);

		Logger::log("Saving shader data to " + binaryPath);

		if (binaryFile.is_open())
		{
			// Write the binary format and data to the file
			binaryFile.write(reinterpret_cast<const char*>(binaryData), binaryLength);
			Logger::log("Binary shader data saved successfully.");
		}
		else
		{
			char errorBuf[100]; // Provide a buffer for strerror_s to write the error message
			if (strerror_s(errorBuf, sizeof(errorBuf), errno) == 0)
			{
				std::cerr << "Error: Unable to save binary shader data. " << errorBuf << std::endl;
			}
			else
			{
				std::cerr << "Error: Unable to save binary shader data. Error message not available." << std::endl;
			}
		}
	}
};