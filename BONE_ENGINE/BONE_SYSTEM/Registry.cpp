#include "Common.h"
#include "Registry.h"

namespace BONE_SYSTEM
{
	Registry::Registry(VOID)
	{
		rootKey = NULL;
		IsOpened = FALSE;
	}
	
	Registry::~Registry(VOID)
	{

	}

	BOOL Registry::Open(HKEY _rootKey, LPCTSTR _subkey)
	{
		if (!_subkey)
			return FALSE;

		if (IsOpened)
			return FALSE;

		if (RegOpenKey(_rootKey, _subkey, &rootKey) != ERROR_SUCCESS)
			return FALSE;

		IsOpened = TRUE;

		return TRUE;
	}

	BOOL Registry::Close(VOID)
	{
		if (RegCloseKey(rootKey) != ERROR_SUCCESS)
			return FALSE;
	
		IsOpened = FALSE;

		return TRUE;
	}

	BOOL Registry::CreateKey(HKEY _rootKey, LPCTSTR _subkey)
	{
		if (!_subkey)
			return FALSE;

		if (IsOpened)
			return FALSE;

		if (RegCreateKey(_rootKey, _subkey, &rootKey) != ERROR_SUCCESS)
			return FALSE;

		IsOpened = TRUE;

		return TRUE;
	}
	
	BOOL Registry::DeleteKey(HKEY _rootKey, LPCTSTR _subkey)
	{
		if (!_subkey)
			return FALSE;

		if (RegDeleteKey(_rootKey, _subkey) != ERROR_SUCCESS)
			return FALSE;

		return TRUE;
	}

	BOOL Registry::SetValue(LPCTSTR _valueName, LPCTSTR _value)
	{
		if (!_valueName || !_value)
			return FALSE;

		if (!IsOpened)
			return FALSE;


		// REG_SZ 형으로 데이터를 입력
		if (RegSetValueEx(rootKey, _valueName, 0, REG_SZ, (BYTE*)_value,
			(DWORD)_tcslen(_value) * sizeof(TCHAR)) != ERROR_SUCCESS)
		{
			return FALSE;
		}

		return TRUE;
	}

	BOOL Registry::SetValue(LPCTSTR _valueName, DWORD _value)
	{
		if (!_valueName)
			return FALSE;

		if (!IsOpened)
			return FALSE;


		// REG_DWORD 형으로 데이터를 입력
		if (RegSetValueEx(rootKey, _valueName, 0, REG_DWORD, (BYTE*)_value, sizeof(DWORD)) != ERROR_SUCCESS)
		{
			return FALSE;
		}

		return TRUE;
	}
	
	BOOL Registry::SetValueForMultiSz(LPCTSTR _valueName, LPCTSTR _value, DWORD _byteLength)
	{
		if (!_valueName || !_value)
			return FALSE;

		if (!IsOpened)
			return FALSE;


		// REG_MULTI_SZ 형으로 데이터를 입력
		if (RegSetValueEx(rootKey, _valueName, 0, REG_MULTI_SZ, (BYTE*)_value, _byteLength) != ERROR_SUCCESS)
		{
			return FALSE;
		}

		return TRUE;
	}

	BOOL Registry::GetValue(LPCTSTR _valueName, LPCTSTR _value, LPDWORD _bufferLength)
	{
		DWORD ValueType = 0;

		if (!_valueName || !_value || !_bufferLength)
			return FALSE;

		if (!IsOpened)
			return FALSE;

		// 문자열 데이터를 읽어 옵니다.
		if (RegQueryValueEx(rootKey, _valueName, 0, &ValueType, (BYTE*)_value, _bufferLength) != ERROR_SUCCESS)
			return FALSE;

		return TRUE;
	}

	BOOL Registry::GetValue(LPCTSTR _valueName, LPDWORD _value)
	{
		DWORD ValueType = 0;
		DWORD BufferLength = sizeof(DWORD);

		if (!_valueName || !_value )
			return FALSE;

		if (!IsOpened)
			return FALSE;

		// 문자열 데이터를 읽어 옵니다.
		if (RegQueryValueEx(rootKey, _valueName, 0, &ValueType, (BYTE*)_value, &BufferLength) != ERROR_SUCCESS)
			return FALSE;

		return TRUE;
	}

	BOOL Registry::DeleteValue(LPCTSTR _valueName)
	{
		if (!_valueName)
			return FALSE;

		if (!IsOpened)
			return FALSE;

		if (RegDeleteValue(rootKey, _valueName) != ERROR_SUCCESS)
			return FALSE;


		return TRUE;
	}
}