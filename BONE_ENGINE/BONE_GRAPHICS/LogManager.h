#pragma once
#include "ISingleton.h"
#include "Common.h"

namespace BONE_GRAPHICS
{
	enum LOG{
		LOG_ERROR, LOG_VALUE, LOG_MESSAGE, LOG_SET
	};

	class LogManager : public ISingleton<LogManager>, public BONE_SYSTEM::MultiThreadSync<LogManager>
	{
	private:
		int index;
		char* logMessage;
		char* preMessage;
		string logFileName;
		bool IsSave;

	public:
		LogManager() {}
		virtual ~LogManager() {}

		void InitializeMembers(bool _outputLog);
		void ShowMessage(int _type, char* _log);
		void ShowMessage(int _type, char* _log, float _value);
		void ShowMessage(int _type, char* _log, char* _value);
		void AddLogToFile(int _type, string _log);
		void SaveLog(bool _flag);
		virtual void ReleaseMembers();
	};
}