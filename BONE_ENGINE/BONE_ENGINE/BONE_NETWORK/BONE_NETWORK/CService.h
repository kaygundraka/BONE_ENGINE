#pragma once
#include "Common.h"

namespace BONE_NETWORK
{
	class CService abstract {
	public:
		static TCHAR serviceName[MAX_PATH];
		static SERVICE_STATUS_HANDLE serviceStatusHandle;
		static SERVICE_STATUS serviceStatus;

		BOOL Install(LPCTSTR _serviceName);
		BOOL Uninstall(LPCTSTR _serviceName);

		BOOL Begin(LPTSTR _serviceName);
		BOOL End(VOID);

		virtual VOID OnStarted() = 0;
		virtual VOID OnStopped() = 0;

		static BOOL SetServiceName(LPTSTR _serviceName);
		static VOID WINAPI RunCallback(DWORD _argumentCount, LPTSTR *_arguments);
		static VOID WINAPI CtrlHandlerCallBack(DWORD _opCode);
	};
}