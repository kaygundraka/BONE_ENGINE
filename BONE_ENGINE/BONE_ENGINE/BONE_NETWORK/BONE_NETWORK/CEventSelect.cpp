#include "Common.h"
#include "CEventSelect.h"

namespace BONE_NETWORK
{
	DWORD WINAPI SelectThreadCallback(LPVOID parameter)
	{
		CEventSelect* owner = (CEventSelect*)parameter;
		owner->SelectThreadCallback();
		
		return 0;
	}

	CEventSelect::CEventSelect(VOID)
	{
		selectEventHandle = NULL;	// 소켓 이벤트를 관리
		destroyEventHandle = NULL;	// 스레드를 종료할 때 사용하는 이벤트
		selectThreadHandle = NULL;	// 소켓 이벤트를 관리하는 스레드 핸들 값
		startupEventHandle = NULL;	// 소켓 이벤트를 관리하는 스레드 시작을 체크하는 이벤트

		sock = NULL;	// 사용한 소켓 핸들
	}

	CEventSelect::~CEventSelect(VOID)
	{

	}

	BOOL CEventSelect::Begin(SOCKET _socket)
	{
		if (!_socket)
			return FALSE;

		if (sock)
			return FALSE;

		sock = _socket;

		selectEventHandle = WSACreateEvent();
		if (selectEventHandle == WSA_INVALID_EVENT)
		{
			End();

			return FALSE;
		}

		destroyEventHandle = CreateEvent(0, FALSE, FALSE, 0);
		if (destroyEventHandle == NULL)
		{
			End();

			return FALSE;
		}

		startupEventHandle = CreateEvent(0, FALSE, FALSE, 0);
		if (startupEventHandle == NULL)
		{
			End();

			return FALSE;
		}

		DWORD Result = WSAEventSelect(sock, selectEventHandle, FD_CONNECT | FD_READ | FD_WRITE | FD_CLOSE);
		if (Result == SOCKET_ERROR)
		{
			End();

			return FALSE;
		}

		DWORD SelectThreadID = 0;
		selectThreadHandle = CreateThread(NULL, 0, BONE_NETWORK::SelectThreadCallback, this, 0, &SelectThreadID);

		if (!selectThreadHandle)
		{
			End();

			return FALSE;
		}

		// 스레드가 생성 완료될 때까지 대기합니다.
		// 스레드가 Wait 될 때까지 시간을 벌어줍니다.
		// 스레드가 생성되면서 startupEventHandle 이벤트가 발생하면 함수가 종료됩니다.
		WaitForSingleObject(startupEventHandle, INFINITE);

		return TRUE;
	}
	
	BOOL CEventSelect::End(VOID)
	{
		// 소켓 멤버가 이미 NULL 일 경우 실행하지 않습니다.
		if (!sock)
			return FALSE;

		if (selectThreadHandle)
		{
			// 스레드를 종료하는 이벤트 발생
			SetEvent(destroyEventHandle);

			WaitForSingleObject(selectThreadHandle, INFINITE);

			CloseHandle(selectThreadHandle);
		}

		if (selectEventHandle)
			CloseHandle(selectEventHandle);

		if (destroyEventHandle)
			CloseHandle(destroyEventHandle);

		if (startupEventHandle)
			CloseHandle(startupEventHandle);

		return TRUE;
	}

	VOID CEventSelect::SelectThreadCallback(VOID)
	{
		// 네트워크 이벤트를 관리하는 함수
		WSANETWORKEVENTS NetworkEvents;
		DWORD EventID = 0;
		DWORD Result = 0;

		// 스레드에 직접적으로 영향을 미치는 이벤트는 종료와 네트워크 이벤트
		HANDLE ThreadEvents[2] = { destroyEventHandle, selectEventHandle };

		while (true)
		{
			// 스레드가 정상적으로 시작되었다는 것을 Begin 함수에 알려줍니다
			SetEvent(startupEventHandle);

			// 여기서는 스레드의 시작과 종료를 관리합니다.
			// 어떠한 이벤트가 발생할 때까지 계속 대기합니다.
			EventID = ::WaitForMultipleObjects(2, ThreadEvents, FALSE, INFINITE);
			switch (EventID)
			{
				// 스레드 종료 이벤트가 발생했을 경우
			case WAIT_OBJECT_0:
				return;

			case WAIT_OBJECT_0 + 1:
				// 소켓이벤트가 발생했을 때 받아옵니다.
				// selectEventHandle에서 네트워크 이벤트를 뽑아옵니다.
				EventID = WSAEnumNetworkEvents(sock, selectEventHandle, &NetworkEvents);

				// 만약 EventID가 SOCKET_ERROR 일 경우 무엇이 잘못된 것이므로 스레드를 종료합니다.
				// 에러의 내용은 WSAGetLastError 함수를 토해서 확인할 수 있습니다.
				if (EventID == SOCKET_ERROR)
					return;
				else
				{
					// Connect 이벤트가 발생했을 경우
					if (NetworkEvents.lNetworkEvents & FD_CONNECT)
					{
						// 가상 함수를 호출해줍니다.
						OnIoConnected();

						if (NetworkEvents.iErrorCode[FD_CONNECT_BIT])
							return;
					}
					else if (NetworkEvents.lNetworkEvents & FD_WRITE)
					{
						// 사용하지 않습니다.
						// OnIoWrote();
					}
					else if (NetworkEvents.lNetworkEvents & FD_READ)
					{
						OnIoRead();
					}
					else if (NetworkEvents.lNetworkEvents & FD_CLOSE)
					{
						OnIoDisconnected();

						return;
					}
				}
				break;

			default:
				return;
			}
		}
	}
}