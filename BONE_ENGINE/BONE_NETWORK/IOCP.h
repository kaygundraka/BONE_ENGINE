#pragma once
#include "Common.h"

namespace BONE_NETWORK
{
	DWORD WINAPI WorkerThreadCallback(LPVOID parameter);

	class CIOCP {
	public:
		CIOCP(VOID);
		virtual ~CIOCP(VOID);

	private:
		HANDLE iocpHandle;
		HANDLE startupEventHandle;
		INT workThreadCount;

		std::vector<HANDLE>	workerThreadVector;

	protected:
		virtual VOID OnIoRead(VOID *object, DWORD dataLength) = 0;
		virtual VOID OnIoWrote(VOID *object, DWORD dataLength) = 0;
		virtual VOID OnIoConnected(VOID *object) = 0;
		virtual VOID OnIoDisconnected(VOID *object) = 0;
	
	public:
		BOOL Begin(VOID);
		BOOL End(VOID);

		BOOL RegisterSocketToIocp(SOCKET _socket, ULONG_PTR _completionKey);
		VOID WorkThreadCallback(VOID);
	};
}