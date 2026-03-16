#include "Config.h"
#include <fstream>
#include <iostream>

bool Config::load(const std::string &filePath)
{
    std::ifstream configFile(filePath);
    if (!configFile.is_open())
    {
        std::cerr << "Error: [IConfig] Could not open file: " << filePath << std::endl;
        return false;
    }

    try
    {
        json configJson;
        configFile >> configJson;

        this->ParseJson(configJson);
    }
    catch (json::parse_error &e)
    {
        std::cerr << "Error: [IConfig] Failed to parse " << filePath << ": " << e.what() << std::endl;
        return false;
    }
    if (__SHOWINFO__)
        std::cout << "[IConfig] Loaded config successfully from " << filePath << std::endl;
    return true;
}