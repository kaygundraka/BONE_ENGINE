#include "Common.h"
#include "Service.h"
#include <functional>
#pragma comment(lib, "Advapi32.lib")

namespace BONE_NETWORK
{
	TCHAR CService::serviceName[MAX_PATH];
	SERVICE_STATUS_HANDLE CService::serviceStatusHandle;
	SERVICE_STATUS CService::serviceStatus;

	BOOL CService::Install(LPCTSTR _serviceName)
	{
		TCHAR ServiceFileName[MAX_PATH] = { 0, };
		SC_HANDLE ServiceControlManager = NULL;
		SC_HANDLE ServiceHandle = NULL;

		if (!_serviceName)
			return FALSE;
		
		ServiceFileName[0] = '"';
		GetModuleFileName(NULL, ServiceFileName + 1, MAX_PATH - 1);
		_tcscat(ServiceFileName, _T("\""));

		ServiceControlManager = OpenSCManager(NULL, NULL, SC_MANAGER_CREATE_SERVICE);
		if (!ServiceControlManager)
		{
			DWORD Err = GetLastError();
			return FALSE;
		}

		ServiceHandle = CreateService(ServiceControlManager,
			_serviceName,
			_serviceName,
			SERVICE_ALL_ACCESS,
			SERVICE_WIN32_OWN_PROCESS,
			SERVICE_DEMAND_START,
			SERVICE_ERROR_NORMAL,
			ServiceFileName,
			NULL,
			NULL,
			NULL,
			NULL,
			NULL);

		if (!ServiceHandle)
		{
			CloseServiceHandle(ServiceControlManager);
			return FALSE;
		}

		CloseServiceHandle(ServiceHandle);
		CloseServiceHandle(ServiceControlManager);

		return TRUE;
	}

	BOOL CService::Uninstall(LPCTSTR _serviceName)
	{
		SC_HANDLE ServiceControlManager = NULL;
		SC_HANDLE ServiceHandle = NULL;

		if (!_serviceName)
			return FALSE;

		ServiceControlManager = OpenSCManager(NULL, NULL, SC_MANAGER_CREATE_SERVICE);
		
		if (!ServiceControlManager)
			return FALSE;
		
		ServiceHandle = OpenService(ServiceControlManager, _serviceName, DELETE);

		if (!ServiceHandle)
		{
			CloseServiceHandle(ServiceControlManager);
			return FALSE;
		}

		if (!DeleteService(ServiceHandle))
		{
			CloseServiceHandle(ServiceHandle);
			CloseServiceHandle(ServiceControlManager);

			return FALSE;
		}

		CloseServiceHandle(ServiceHandle);
		CloseServiceHandle(ServiceControlManager);
	}


	BOOL CService::Begin(LPTSTR _serviceName)
	{
		if (!_serviceName)
			return FALSE;

		SERVICE_TABLE_ENTRY DispatchTable[]
		{
			{_serviceName, CService::RunCallback},
			{NULL, NULL}
		};

		OnStarted();

		if (!StartServiceCtrlDispatcher(DispatchTable))
		{
			_tprintf(_T("## Debug Console Mode ##\n"));
			getchar();
		}

		return TRUE;
	}

	BOOL CService::SetServiceName(LPTSTR _serviceName)
	{
		if (!_serviceName)
			return FALSE;

		_tcscpy(serviceName, _serviceName);

		return TRUE;
	}

	BOOL CService::End(VOID)
	{
		OnStopped();

		return TRUE;
	}

	VOID WINAPI CService::RunCallback(DWORD _argumentCount, LPTSTR *_arguments)
	{
		DWORD Status;

		serviceStatus.dwServiceType = SERVICE_WIN32;
		serviceStatus.dwCurrentState = SERVICE_START_PENDING;
		serviceStatus.dwControlsAccepted = SERVICE_ACCEPT_STOP | SERVICE_ACCEPT_PAUSE_CONTINUE;

		serviceStatus.dwWin32ExitCode = 0;
		serviceStatus.dwServiceSpecificExitCode = 0;
		serviceStatus.dwCheckPoint = 0;
		serviceStatus.dwWaitHint = 0;

		serviceStatusHandle = RegisterServiceCtrlHandler(serviceName, CService::CtrlHandlerCallBack);

		if (serviceStatusHandle == (SERVICE_STATUS_HANDLE)0)
			return;

		Status = NO_ERROR;

		serviceStatus.dwCurrentState = SERVICE_RUNNING;
		serviceStatus.dwCheckPoint = 0;
		serviceStatus.dwWaitHint = 0;

		SetServiceStatus(serviceStatusHandle, &serviceStatus);	
	}

	VOID WINAPI CService::CtrlHandlerCallBack(DWORD _opCode)
	{
		switch (_opCode)
		{
		case SERVICE_CONTROL_PAUSE:
			serviceStatus.dwCurrentState = SERVICE_PAUSED;
			break;

		case SERVICE_CONTROL_CONTINUE:
			serviceStatus.dwCurrentState = SERVICE_RUNNING;
			break;

		case SERVICE_CONTROL_STOP:
			serviceStatus.dwWin32ExitCode = SERVICE_STOP;
			serviceStatus.dwCurrentState = SERVICE_STOPPED;
			serviceStatus.dwCheckPoint = 0;
			serviceStatus.dwWaitHint = 0;
			break;

		case SERVICE_CONTROL_INTERROGATE:
			break;
		}

		SetServiceStatus(serviceStatusHandle, &serviceStatus);
	}
}