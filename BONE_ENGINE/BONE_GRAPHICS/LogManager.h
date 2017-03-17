#pragma once
#include "ISingleton.h"
#include "Common.h"

const size_t LOGSYSTEM_MAX_BUFFER = 4096;

namespace BONE_GRAPHICS
{
	enum LOG {
		LOG_ERROR, LOG_WARNING, LOG_INFO
	};

	class LogManager : public ISingleton<LogManager>, public BONE_SYSTEM::MultiThreadSync<LogManager>
	{
	private:
		int index;
		char* logMessage;
		char* preMessage;
		std::string logFileName;
		bool IsSave;

        char buff[LOGSYSTEM_MAX_BUFFER];

        void ShowMessage(int _type, const char* _log, va_list args);

	public:
		LogManager() {}
		virtual ~LogManager() {}

		void InitializeMembers(bool _outputLog);

        void Error(const char * format, ...);
        void Warning(const char * format, ...);
        void Info(const char * format, ...);

		void AddLogToFile(int _type, std::string _log);

		void SaveLog(bool _flag);
		virtual void ReleaseMembers();
	};
}