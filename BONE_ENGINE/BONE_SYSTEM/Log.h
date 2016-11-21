#pragma once
#include "Common.h"

#define MAX_BUFFER_LENGTH 512

namespace BONE_SYSTEM
{
	class CLog {
	public:
		static BOOL WriteLog(LPTSTR _data, ...)
		{
			SYSTEMTIME	SystemTime					= { 0, };
			TCHAR		CurrentData[32]				= { 0, };
			TCHAR		CurrentFileName[MAX_PATH]	= { 0, };
			FILE*		FilePtr						= NULL;
			TCHAR		DebugLog[MAX_BUFFER_LENGTH]	= { 0. };

			va_list		ap;
			TCHAR		Log[MAX_BUFFER_LENGTH]		= { 0, };

			va_start(ap, _data);
			_vstprintf(Log, _data, ap);
			va_end(ap);

			GetLocalTime(&SystemTime);
			_sntprintf(CurrentData, 32, _T("%d-%d-%d %d:%d:%d"),
				SystemTime.wYear,
				SystemTime.wMonth,
				SystemTime.wDay,
				SystemTime.wHour,
				SystemTime.wMinute,
				SystemTime.wSecond);

			_sntprintf(CurrentFileName, MAX_PATH, _T("LOG_%d-%d-%d %d. log"),
				SystemTime.wYear,
				SystemTime.wMonth,
				SystemTime.wDay,
				SystemTime.wHour
				);

			FilePtr = _tfopen(CurrentFileName, _T("a"));

			if (!FilePtr)
				return FALSE;

			_ftprintf(FilePtr, _T("[%s] %s\n"), CurrentData, Log);
			_sntprintf(DebugLog, MAX_BUFFER_LENGTH, _T("[%s] %s\n"), CurrentData, Log);

			fflush(FilePtr);

			fclose(FilePtr);

			OutputDebugString(DebugLog);
			_tprintf(_T("%s"), DebugLog);

			return TRUE;
		}
	};
}