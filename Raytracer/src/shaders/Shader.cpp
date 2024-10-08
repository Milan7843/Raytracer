
#include "Shader.h"

#include "Scene.h"

Shader::Shader(const char* vertexPath, const char* fragmentPath)
{
	std::string binaryPath = ("cache/" + extractFileName(vertexPath) + "_" + extractFileName(fragmentPath) + ".bin");

	ID = glCreateProgram();

	bool succesfullyLoadedCachedShader{ false };

	if (Cache::cachedFileExists(binaryPath))
	{
		Logger::logDebug("Loading cached data for " + binaryPath);

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
				Logger::logError("Error: shader program linking failed. Falling back to compilation.");
				Logger::logError(infoLog);
				//return;
			}
			else
			{
				succesfullyLoadedCachedShader = true;
				Logger::logDebug("Cached shader data loaded successfully for " + binaryPath);
			}

			//free(binaryData);
		}
		else
		{
			Logger::logError("Error: Unable to open binary shader data file.");
		}

		//linkProgram();
	}

	if (!succesfullyLoadedCachedShader)
	{
		Logger::logDebug("Compiling shader " + binaryPath);

		std::string vertexCode = readFile(vertexPath);
		std::string fragmentCode = readFile(fragmentPath);
		const char* vShaderCode = vertexCode.c_str();
		const char* fShaderCode = fragmentCode.c_str();

		/* Compiling the shaders */

		unsigned int vertex, fragment;
		vertex = compileShader(GL_VERTEX_SHADER, vShaderCode);
		fragment = compileShader(GL_FRAGMENT_SHADER, fShaderCode);


		/* Creating the shader program */
		glAttachShader(ID, vertex);
		glAttachShader(ID, fragment);

		linkProgram();

		// Saving the shader binary
		Logger::logDebug("Caching shader " + binaryPath);

		GLint binaryLength;
		glGetProgramiv(ID, GL_PROGRAM_BINARY_LENGTH, &binaryLength);

		if (binaryLength > 0)
		{
			GLenum binaryFormat;
			GLvoid* binaryData = malloc(binaryLength);

			glGetProgramBinary(ID, binaryLength, NULL, &binaryFormat, binaryData);

			if (binaryData)
			{
				Cache::cacheShader(binaryPath, binaryFormat, binaryData, binaryLength);
			}
			else
			{
				Logger::logError("Error: Unable to allocate memory for binary shader data.");
			}

			free(binaryData);
		}
		else
		{
			char errorBuf[100]; // Provide a buffer for strerror_s to write the error message
			if (strerror_s(errorBuf, sizeof(errorBuf), errno) == 0)
			{
				Logger::logError("Error: Unable to retrieve binary shader data. " + std::string(errorBuf));
			}
			else
			{
				Logger::logError("Error: Unable to retrieve binary shader data. Error message not available.");
			}
		}

		// Deleting the shaders as they're linked into our program now and no longer necessary
		glDeleteShader(vertex);
		glDeleteShader(fragment);
	}
}

Shader::Shader(const char* vertexPath, const char* fragmentPath, const char* geometryPath)
{
	std::string binaryPath = ("cache/" + extractFileName(vertexPath) + "_" + extractFileName(fragmentPath) + "_" + extractFileName(geometryPath) + ".bin");

	ID = glCreateProgram();

	bool succesfullyLoadedCachedShader{ false };

	if (Cache::cachedFileExists(binaryPath))
	{
		Logger::logDebug("Loading cached data for " + binaryPath);

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
				Logger::logError("Error: shader program linking failed. Falling back to compilation.");
				Logger::logError(infoLog);
				//return;
			}
			else
			{
				succesfullyLoadedCachedShader = true;
				Logger::logDebug("Cached shader data loaded successfully for " + binaryPath);
			}

			//free(binaryData);
		}
		else
		{
			Logger::logError("Error: Unable to open binary shader data file.");
		}

		//linkProgram();
	}
	
	if (!succesfullyLoadedCachedShader)
	{
		Logger::logDebug("Compiling shader " + binaryPath);

		std::string vertexCode = readFile(vertexPath);
		std::string fragmentCode = readFile(fragmentPath);
		std::string geometryCode = readFile(geometryPath);
		const char* vShaderCode = vertexCode.c_str();
		const char* fShaderCode = fragmentCode.c_str();
		const char* gShaderCode = geometryCode.c_str();


		/* Compiling the shaders */

		unsigned int vertex, fragment, geometry;
		vertex = compileShader(GL_VERTEX_SHADER, vShaderCode);
		fragment = compileShader(GL_FRAGMENT_SHADER, fShaderCode);
		geometry = compileShader(GL_GEOMETRY_SHADER, gShaderCode);


		/* Creating the shader program */

		ID = glCreateProgram();
		glAttachShader(ID, vertex);
		glAttachShader(ID, fragment);
		glAttachShader(ID, geometry);

		linkProgram();

		// Saving the shader binary
		Logger::logDebug("Caching shader " + binaryPath);

		GLint binaryLength;
		glGetProgramiv(ID, GL_PROGRAM_BINARY_LENGTH, &binaryLength);

		if (binaryLength > 0)
		{
			GLenum binaryFormat;
			GLvoid* binaryData = malloc(binaryLength);

			glGetProgramBinary(ID, binaryLength, NULL, &binaryFormat, binaryData);

			if (binaryData)
			{
				Cache::cacheShader(binaryPath, binaryFormat, binaryData, binaryLength);
			}
			else
			{
				Logger::logError("Error: Unable to allocate memory for binary shader data.");
			}

			free(binaryData);
		}
		else
		{
			char errorBuf[100]; // Provide a buffer for strerror_s to write the error message
			if (strerror_s(errorBuf, sizeof(errorBuf), errno) == 0)
			{
				Logger::logError("Error: Unable to retrieve binary shader data. " + std::string(errorBuf));
			}
			else
			{
				Logger::logError("Error: Unable to retrieve binary shader data. Error message not available.");
			}
		}

		// Deleting the shaders as they're linked into our program now and no longer necessary
		glDeleteShader(vertex);
		glDeleteShader(fragment);
		glDeleteShader(geometry);
	}
}

Shader::~Shader()
{
	Logger::logDebug("Shader destroyed.");
}