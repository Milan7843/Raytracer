#include "Logger.h"

namespace Logger
{
    namespace
    {
        bool debug{ true };
    }

    void setDebug(bool enabled)
    {
        debug = enabled;
    }

    void log(std::string message)
    {
        log(message.c_str());
    }

    void log(const char* message)
    {
        // White text on black background
        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 15);
        write(message);
    }

    void logWarning(std::string message)
    {
        logWarning(message.c_str());
    }

    void logWarning(const char* message)
    {
        // Yellow text on black background
        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 6);
        write(message);
    }

    void logDebug(std::string message)
    {
        if (!debug)
        {
            return;
        }

        logDebug(message.c_str());
    }

    void logDebug(const char* message)
    {
        if (!debug)
        {
            return;
        }

        // Green text on black background
        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 2);
        write(message);
    }

    void logError(std::string message)
    {
        logError(message.c_str());
    }

    void logError(const char* message)
    {
        // Red text on black background
        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 4);
        write(message);
    }

    void write(const char* message)
    {
        // Actually write the message in the console
        std::cout << message << std::endl;
    }

    void stop()
    {
        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 15);
    }

    void logMatrix(glm::mat4& matrix, const char* message)
    {
        std::cout << message << ":\n";
        std::cout << std::fixed << std::setprecision(2); // Set fixed-point format with 4 decimal places

        for (int i = 0; i < 4; i++)
        {
            for (int j = 0; j < 4; j++)
            {
                std::cout << std::setw(5) << matrix[i][j] << " "; // Set width to 7 to accommodate 4 characters plus 3 decimal places
            }
            std::cout << "\n";
        }
    }
}
