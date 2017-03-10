#include "Common.h"
#include "ConfigManager.h"

namespace BONE_GRAPHICS
{
    void ConfigManager::InitializeMembers()
    {
    }

    void ConfigManager::ReleaseMembers()
    {

    }

    std::string ConfigManager::GetStr(std::string fileName, std::string name)
    {
        std::ifstream file(fileName + ".json");

        json j;
        file >> j;

        auto iter = j.find(name);
        if (iter != j.end())
            return iter.value().get<std::string>();

        file.close();

        return "";
    }

    std::vector<std::string> ConfigManager::GetPaths(std::string fileName, std::string name)
    {
        std::ifstream file(fileName + ".json");
        json j;
        file >> j;

        auto iter = j.find(name);
        if (iter != j.end())
        {
            file.close();
            return iter.value().get<std::vector<std::string>>();
        }

        std::vector<std::string> paths;
                
        file.close();
        return paths;
    }

    float ConfigManager::GetFloat(std::string fileName, std::string name)
    {
        std::ifstream file(fileName + ".json");
        json j;
        file >> j;

        auto iter = j.find(name);
        if (iter != j.end())
        {
            file.close();
            return iter.value().get<float>();
        }

        file.close();
        return 0;
    }

    bool ConfigManager::GetBool(std::string fileName, std::string name)
    {
        std::ifstream file(fileName + ".json");
        json j;
        file >> j;

        auto iter = j.find(name);
        if (iter != j.end())
        {
            file.close();
            return iter.value().get<bool>();
        }

        file.close();
        return 0;
    }
}