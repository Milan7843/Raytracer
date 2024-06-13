#include "ComputeShader.h"

ComputeShader::ComputeShader(const char* shaderPath)
{

	std::string binaryPath = ("cache/" + extractFileName(shaderPath) + ".bin");

	ID = glCreateProgram();

	bool succesfullyLoadedCachedShader{ true };

	if (Cache::cachedFileExists(binaryPath))
	{
		Logger::log("Loading cached data for " + binaryPath);

		// There is a cached file, just load that
		std::ifstream binaryFile(binaryPath, std::ios::binary | std::ios::in);

		if (binaryFile.is_open())
		{
			std::ifstream inputStream(binaryPath.c_str(), std::ios::binary);
			std::istreambuf_iterator<char> startIt(inputStream), endIt;
			std::vector<char> binaryData(startIt, endIt);
			inputStream.close();

			// Load the binary data and format into the shader program
			glProgramParameteri(ID, GL_PROGRAM_BINARY_RETRIEVABLE_HINT, GL_TRUE);
			glProgramBinary(ID, 36385, binaryData.data(), binaryData.size());

			int success;
			char infoLog[512];

			glGetProgramiv(ID, GL_LINK_STATUS, &success);

			if (!success)
			{
				// Getting the error log
				glGetProgramInfoLog(ID, 512, NULL, infoLog);

				// Printing the error log
				Logger::logError("Error: shader program linking failed.");
				Logger::logError(infoLog);
				//return;
			}
			else
			{
				succesfullyLoadedCachedShader = true;
				std::cout << "Cached shader data loaded successfully for " << binaryPath << std::endl;
			}

			//free(binaryData);
		}
		else
		{
			std::cerr << "Error: Unable to open binary shader data file." << std::endl;
		}

		//linkProgram();
	}
	
	if (!succesfullyLoadedCachedShader)
	{
		Logger::log("Compiling shader " + binaryPath);

		std::string shaderCode = readFile(shaderPath);
		const char* shaderCodeChars = shaderCode.c_str();


		/* Compiling the shaders */

		unsigned int shader;
		shader = compileShader(GL_COMPUTE_SHADER, shaderCodeChars);


		/* Creating the shader program */

		ID = glCreateProgram();
		glAttachShader(ID, shader);
		linkProgram();

		// Saving the shader binary
		Logger::log("Caching shader " + binaryPath);

		GLint binaryLength;
		glGetProgramiv(ID, GL_PROGRAM_BINARY_LENGTH, &binaryLength);

		if (binaryLength > 0)
		{
			GLenum binaryFormat;
			GLvoid* binaryData = malloc(binaryLength);

			glGetProgramBinary(ID, binaryLength, NULL, &binaryFormat, binaryData);

			std::cout << "binary format new " << binaryFormat << std::endl;

			if (binaryData)
			{
				Cache::cacheShader(binaryPath, binaryFormat, binaryData, binaryLength);
			}
			else
			{
				std::cerr << "Error: Unable to allocate memory for binary shader data." << std::endl;
			}

			free(binaryData);
		}
		else
		{
			char errorBuf[100]; // Provide a buffer for strerror_s to write the error message
			if (strerror_s(errorBuf, sizeof(errorBuf), errno) == 0)
			{
				std::cerr << "Error: Unable to retrieve binary shader data. " << errorBuf << std::endl;
			}
			else
			{
				std::cerr << "Error: Unable to retrieve binary shader data. Error message not available." << std::endl;
			}
		}

		// Deleting the shader as it's linked into our program now and no longer necessary
		glDeleteShader(shader);
	}
}

ComputeShader::~ComputeShader()
{
	Logger::log("Compute shader destroyed.");
}

ComputeShader::ComputeShader()
{
}
