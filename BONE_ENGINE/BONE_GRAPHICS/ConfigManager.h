#pragma once
#include "ISingleton.h"
#include "Common.h"

namespace BONE_GRAPHICS
{
    class ConfigManager : public ISingleton <ConfigManager>, public BONE_SYSTEM::MultiThreadSync <ConfigManager>
    {
    private:
        
    public:
        ConfigManager() {}
        virtual ~ConfigManager() {}

        void InitializeMembers();
        virtual void ReleaseMembers();

    public:
        std::string GetStr(std::string fileName, std::string name);
        std::vector<std::string> GetPaths(std::string fileName, std::string name);
        float GetFloat(std::string fileName, std::string name);
        bool GetBool(std::string fileName, std::string name);
    };
}