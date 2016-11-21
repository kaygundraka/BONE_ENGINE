#include "Common.h"
#include "IniFile.h"
#pragma warning(disable:4996)

namespace BONE_SYSTEM
{
	CIniFile::CIniFile(VOID)
	{
		ZeroMemory(fileName, sizeof(fileName));
	}

	CIniFile::~CIniFile(VOID)
	{

	}

	
	BOOL CIniFile::Open(LPCTSTR _fileName)
	{
		if (!_fileName)
			return FALSE;

		_tcsncpy(fileName, _fileName, MAX_PATH);

		return TRUE;
	}

	BOOL CIniFile::Close(VOID)
	{
		return TRUE;
	}


	BOOL CIniFile::SetValue(LPCTSTR _keyName, LPCTSTR _valueName, LPCTSTR _value)
	{
		if (!_keyName || !_valueName || !_value)
			return FALSE;

		WritePrivateProfileString(_keyName, _valueName, _value, fileName);

		return TRUE;
	}

	BOOL CIniFile::SetValue(LPCTSTR _keyName, LPCTSTR _valueName, DWORD _value)
	{
		if (!_keyName || !_valueName || !_value)
			return FALSE;

		TCHAR Value[16] = { 0, };
		_itot(_value, Value, 10);

	    WritePrivateProfileString(_keyName, _valueName, Value, fileName);

		return TRUE;
	}

	BOOL CIniFile::SetValue(LPCTSTR _keyName, LPCTSTR _valueName, FLOAT _value)
	{
		if (!_keyName || !_valueName || !_value)
			return FALSE;

		TCHAR Value[16] = { 0, };
		_stprintf(Value, _T("%f"), _value);
		
		WritePrivateProfileString(_keyName, _valueName, Value, fileName);

		return TRUE;
	}


	BOOL CIniFile::GetValue(LPCTSTR _keyName, LPCTSTR _valueName, LPTSTR _value, LPDWORD _bufferLength)
	{
		if (!_keyName || !_valueName || !_value)
			return FALSE;

		*_bufferLength = GetPrivateProfileString(_keyName, _valueName, _T(""), _value, *_bufferLength, fileName);

		return TRUE;
	}

	BOOL CIniFile::GetValue(LPCTSTR _keyName, LPCTSTR _valueName, LPDWORD _value)
	{
		if (!_keyName || !_valueName || !_value)
			return FALSE;

		*_value = GetPrivateProfileInt(_keyName, _valueName, 0, fileName);
	}

	BOOL CIniFile::GetValue(LPCTSTR _keyName, LPCTSTR _valueName, FLOAT *_value)
	{
		if (!_keyName || !_valueName || !_value)
			return FALSE;

		TCHAR Value[16] = { 0, };

		*Value = GetPrivateProfileString(_keyName, _valueName, _T(""), Value, 16, fileName);

		*_value = (FLOAT)_tstof(Value);

		return TRUE;
	}
}