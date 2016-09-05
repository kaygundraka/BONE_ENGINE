#include "Common.h"
#include "CIOCP.h"

namespace BONE_NETWORK
{
	DWORD WINAPI WorkerThreadCallback(LPVOID parameter)
	{
		CIOCP *Owner = (CIOCP*)parameter;
		Owner->WorkThreadCallback();

		return 0;
	}

	CIOCP::CIOCP(VOID)
	{
		iocpHandle = NULL;
		startupEventHandle = NULL;
		workThreadCount = 0;
	}
	
	CIOCP::~CIOCP(VOID)
	{
	}

	BOOL CIOCP::Begin(VOID)
	{
		iocpHandle = NULL;

		SYSTEM_INFO SystemInfo;
		GetSystemInfo(&SystemInfo);

		workThreadCount = SystemInfo.dwNumberOfProcessors * 2;
		iocpHandle = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, 0, 0);

		if (!iocpHandle)
			return FALSE;

		startupEventHandle = CreateEvent(0, FALSE, FALSE, 0);
		if (startupEventHandle == NULL)
		{
			End();

			return FALSE;
		}

		for (DWORD i = 0; i < workThreadCount; i++)
		{
			HANDLE WorkerThread = CreateThread(NULL, 0, BONE_NETWORK::WorkerThreadCallback, this, 0, NULL);
			workerThreadVector.push_back(WorkerThread);
			WaitForSingleObject(startupEventHandle, INFINITE);
		}

		return TRUE;
	}

	BOOL CIOCP::End(VOID)
	{
		for (DWORD i = 0; i < workerThreadVector.size(); i++)
			PostQueuedCompletionStatus(iocpHandle, 0, 0, NULL);

		for (DWORD i = 0; i < workerThreadVector.size(); i++)
		{
			WaitForSingleObject(workerThreadVector[i], INFINITE);
			CloseHandle(workerThreadVector[i]);
		}

		if (iocpHandle)
			CloseHandle(iocpHandle);

		workerThreadVector.clear();

		if (startupEventHandle)
			CloseHandle(startupEventHandle);

		return TRUE;
	}

	BOOL CIOCP::RegisterSocketToIocp(SOCKET _socket, ULONG_PTR _completionKey)
	{
		if (!_socket || !_completionKey)
			return FALSE;

		iocpHandle = CreateIoCompletionPort((HANDLE)_socket, iocpHandle, _completionKey, 0);

		if (!iocpHandle)
			return FALSE;

		return TRUE;
	}

	VOID CIOCP::WorkThreadCallback(VOID)
	{
		BOOL			Successed = FALSE;
		DWORD			NumberOfByteTransfered = 0;
		VOID			*CompletionKey = NULL;
		OVERLAPPED		*Overlapped = NULL;
		OVERLAPPED_EX	*OverlappedEx = NULL;
		VOID			*Object = NULL;


		while (TRUE)
		{
			SetEvent(startupEventHandle);

			Successed = GetQueuedCompletionStatus(
				iocpHandle, &NumberOfByteTransfered, 
				(PULONG_PTR)&CompletionKey, &Overlapped,
				INFINITE);

			if (!CompletionKey)
				return;

			OverlappedEx = (OVERLAPPED_EX*)Overlapped;

			Object = OverlappedEx->Object;

			if (!Successed || (Successed && !NumberOfByteTransfered))
			{
				if (OverlappedEx->IoType == IO_ACCPET)
					OnIoConnected(Object);
				else
					OnIoDisconnected(Object);

				continue;
			}

			switch (OverlappedEx->IoType)
			{
			case IO_READ:
				OnIoRead(Object, NumberOfByteTransfered);
				break;

			case IO_WRITE:
				OnIoWrote(Object, NumberOfByteTransfered);
				break;
			}
		}
	}
}