#include "Common.h"
#include "RuntimeCompilerLogger.h"
#include "LogManager.h"

#pragma warning( disable : 4996 4800 )

namespace BONE_GRAPHICS
{
    void RuntimeCompileLogger::LogError(const char * format, ...)
    {
        va_list args;
        va_start(args, format);
        LogMgr->Error(format, args);
    }

    void RuntimeCompileLogger::LogWarning(const char * format, ...)
    {
        va_list args;
        va_start(args, format);
        LogMgr->Warning(format, args);
    }

    void RuntimeCompileLogger::LogInfo(const char * format, ...)
    {
        va_list args;
        va_start(args, format);
        LogMgr->Info(format, args);
    }
}