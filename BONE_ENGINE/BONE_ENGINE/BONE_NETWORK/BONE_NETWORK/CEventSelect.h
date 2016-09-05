#pragma once
#include "Common.h"

namespace BONE_NETWORK
{
	DWORD WINAPI SelectThreadCallback(LPVOID parameter);

	class CEventSelect {
	private:
		HANDLE selectEventHandle;
		HANDLE destroyEventHandle;
		HANDLE selectThreadHandle;
		HANDLE startupEventHandle;

		SOCKET sock;

	public:
		CEventSelect(VOID);
		virtual ~CEventSelect(VOID);

		virtual VOID OnIoRead() = 0;
		//virtual VOID OnIoWrote() = 0;
		virtual VOID OnIoConnected() = 0;
		virtual VOID OnIoDisconnected() = 0;

		BOOL Begin(SOCKET _socket);
		BOOL End(VOID);

		VOID SelectThreadCallback(VOID);

	};
}