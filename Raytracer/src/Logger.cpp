#include "Logger.h"

void Logger::log(std::string message)
{
    log(message.c_str());
}

void Logger::log(const char* message)
{
    // White text on black background
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 15);
    write(message);
}

void Logger::logWarning(std::string message)
{
    logWarning(message.c_str());
}

void Logger::logWarning(const char* message)
{
    // Yellow text on black background
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 6);
    write(message);
}

void Logger::logError(std::string message)
{
    logError(message.c_str());
}

void Logger::logError(const char* message)
{
    // Red text on black background
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 4);
    write(message);
}

void Logger::write(const char* message)
{
    // Actually write the message in the console
    std::cout << message << std::endl;
}

void Logger::stop()
{
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 15);
}
