#include "FileUtility.h"

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

void removeFolderAndFiletype(std::vector<std::string>& fileNames, const char* folder, const char* filetype)
{
	for (std::string& fileName : fileNames)
	{
		// Removing the folder and file type of each entry (offset by one because folder name does not inlude the backslash)
		fileName = fileName.substr(std::strlen(folder) + 1, fileName.length() - std::strlen(folder) - std::strlen(filetype) - 1);
	}
}
