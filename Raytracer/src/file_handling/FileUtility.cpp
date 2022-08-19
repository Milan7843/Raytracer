#include "FileUtility.h"

// For render saving
#define __STDC_LIB_EXT1__
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

void removeFolderAndFiletype(std::vector<std::string>& fileNames, const char* folder, const char* filetype);

std::vector<std::string> FileUtility::getFilesOfTypeInFolder(const char* folder, const char* filetype)
{
	std::vector<std::string> filesFound{};
	try
	{
		// Looping through all files in the scene folder, adding each name
		for (const auto& file : std::filesystem::directory_iterator(folder))
		{
			// The file must end with .scene; if it does, add it to the list
			if (file.path().string().ends_with(filetype))
			{
				// Converting scene path to scene name before adding it
				filesFound.push_back(file.path().string());
			}

			else // Throw an error if an unknown filetype was found in the scenes folder
				Logger::logError("Error: unknown file found in /scenes folder: " + file.path().string());
		}
	}
	catch (std::filesystem::filesystem_error e)
	{
		Logger::logError(std::string("Error reading scene names: ") + e.what());
	}

	removeFolderAndFiletype(filesFound, folder, filetype);

	return filesFound;
}

void FileUtility::saveRender(const std::string& imageName, unsigned int width, unsigned int height, unsigned int pixelBuffer)
{
	// Calculating some important values
	unsigned int numberOfChannels = 4; // R, G, B, A
	unsigned int stride = numberOfChannels * width;

	// Getting the pixel data
	std::vector<char> pixelsChars(stride * height);
	glReadPixels(0, 0, width, height, GL_RGBA, GL_UNSIGNED_BYTE, pixelsChars.data());

	// Writing the pixel data to a png
	stbi_flip_vertically_on_write(true);
	stbi_write_png(("renders/" + imageName).c_str(), width, height, numberOfChannels, pixelsChars.data(), stride);
}

bool FileUtility::isValidInput(std::string& input)
{
	if (input.empty())
		return false;
	if (input.find('\\') != std::string::npos)
		return false;
	if (input.find('.') != std::string::npos)
		return false;
	if (input.find('/') != std::string::npos)
		return false;
	return true;
}

void FileUtility::saveSettings(std::string& loadedSceneName)
{
	// The data stream into the file
	std::ofstream filestream{ "saved_settings.save" };

	// Writing all settings data
	filestream << loadedSceneName << "\n";

	// Done writing so flush data and close filestream
	filestream.close();
}

void FileUtility::readSavedSettings(std::string& savedSceneName)
{
	// Opening the save file into a stream
	std::ifstream filestream{ "saved_settings.save" };

	// Throw an error if the file could not be opened
	if (!filestream)
	{
		Logger::logError("Saved settings could not be opened.");
		
		return;
	}

	// Reading the recently opened scene name
	std::getline(filestream, savedSceneName);

	// Finally closing the file
	filestream.close();
}

void removeFolderAndFiletype(std::vector<std::string>& fileNames, const char* folder, const char* filetype)
{
	for (std::string& fileName : fileNames)
	{
		// Removing the folder and file type of each entry (offset by one because folder name does not inlude the backslash)
		fileName = fileName.substr(std::strlen(folder) + 1, fileName.length() - std::strlen(folder) - std::strlen(filetype) - 1);
	}
}
