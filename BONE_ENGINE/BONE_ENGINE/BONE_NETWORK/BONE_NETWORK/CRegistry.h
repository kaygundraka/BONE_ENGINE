#pragma once
#include "Common.h"

namespace BONE_NETWORK
{
	class CRegistry{
	public:
		CRegistry(VOID);
		virtual ~CRegistry(VOID);

	private:
		HKEY rootKey;
		BOOL IsOpened;

	public:
		BOOL Open(HKEY _rootKey, LPCTSTR _subkey);
		BOOL Close(VOID);

		BOOL CreateKey(HKEY _rootKey, LPCTSTR _subkey);
		BOOL DeleteKey(HKEY _rootKey, LPCTSTR _subkey);

		BOOL SetValue(LPCTSTR _valueName, LPCTSTR _value);
		BOOL SetValue(LPCTSTR _valueName, DWORD _value);
		BOOL SetValueForMultiSz(LPCTSTR _valueName, LPCTSTR _value, DWORD _byteLength);

		BOOL GetValue(LPCTSTR _valueName, LPCTSTR _value, LPDWORD _bufferLength);
		BOOL GetValue(LPCTSTR _valueName, LPDWORD _value);

		BOOL DeleteValue(LPCTSTR _valueName);
	};
}