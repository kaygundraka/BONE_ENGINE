#pragma once
#include "Common.h"

namespace BONE_SYSTEM
{
	class IniFile {
	public:
        IniFile(VOID);
		virtual ~IniFile(VOID);

	private:
		TCHAR fileName[MAX_PATH];

	public:
		BOOL Open(LPCTSTR _fileName);
		BOOL Close(VOID);

		BOOL SetValue(LPCTSTR _keyName, LPCTSTR _valueName, LPCTSTR _value);
		BOOL SetValue(LPCTSTR _keyName, LPCTSTR _valueName, DWORD _value);
		BOOL SetValue(LPCTSTR _keyName, LPCTSTR _valueName, FLOAT _value);

		BOOL GetValue(LPCTSTR _keyName, LPCTSTR _valueName, LPTSTR _value, LPDWORD _bufferLength);
		BOOL GetValue(LPCTSTR _keyName, LPCTSTR _valueName, LPDWORD _value);
		BOOL GetValue(LPCTSTR _keyName, LPCTSTR _valueName, FLOAT *_value);
	};
}