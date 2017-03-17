#pragma once
#include "Common.h"

namespace BONE_GRAPHICS
{
    class RuntimeCompileLogger : public ICompilerLogger
    {
    public:
        virtual void LogError(const char * format, ...);
        virtual void LogWarning(const char * format, ...);
        virtual void LogInfo(const char * format, ...);
    };
}