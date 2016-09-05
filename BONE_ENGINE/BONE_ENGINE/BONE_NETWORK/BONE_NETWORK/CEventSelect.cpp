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
		selectEventHandle = NULL;	// ���� �̺�Ʈ�� ����
		destroyEventHandle = NULL;	// �����带 ������ �� ����ϴ� �̺�Ʈ
		selectThreadHandle = NULL;	// ���� �̺�Ʈ�� �����ϴ� ������ �ڵ� ��
		startupEventHandle = NULL;	// ���� �̺�Ʈ�� �����ϴ� ������ ������ üũ�ϴ� �̺�Ʈ

		sock = NULL;	// ����� ���� �ڵ�
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

		// �����尡 ���� �Ϸ�� ������ ����մϴ�.
		// �����尡 Wait �� ������ �ð��� �����ݴϴ�.
		// �����尡 �����Ǹ鼭 startupEventHandle �̺�Ʈ�� �߻��ϸ� �Լ��� ����˴ϴ�.
		WaitForSingleObject(startupEventHandle, INFINITE);

		return TRUE;
	}
	
	BOOL CEventSelect::End(VOID)
	{
		// ���� ����� �̹� NULL �� ��� �������� �ʽ��ϴ�.
		if (!sock)
			return FALSE;

		if (selectThreadHandle)
		{
			// �����带 �����ϴ� �̺�Ʈ �߻�
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
		// ��Ʈ��ũ �̺�Ʈ�� �����ϴ� �Լ�
		WSANETWORKEVENTS NetworkEvents;
		DWORD EventID = 0;
		DWORD Result = 0;

		// �����忡 ���������� ������ ��ġ�� �̺�Ʈ�� ����� ��Ʈ��ũ �̺�Ʈ
		HANDLE ThreadEvents[2] = { destroyEventHandle, selectEventHandle };

		while (true)
		{
			// �����尡 ���������� ���۵Ǿ��ٴ� ���� Begin �Լ��� �˷��ݴϴ�
			SetEvent(startupEventHandle);

			// ���⼭�� �������� ���۰� ���Ḧ �����մϴ�.
			// ��� �̺�Ʈ�� �߻��� ������ ��� ����մϴ�.
			EventID = ::WaitForMultipleObjects(2, ThreadEvents, FALSE, INFINITE);
			switch (EventID)
			{
				// ������ ���� �̺�Ʈ�� �߻����� ���
			case WAIT_OBJECT_0:
				return;

			case WAIT_OBJECT_0 + 1:
				// �����̺�Ʈ�� �߻����� �� �޾ƿɴϴ�.
				// selectEventHandle���� ��Ʈ��ũ �̺�Ʈ�� �̾ƿɴϴ�.
				EventID = WSAEnumNetworkEvents(sock, selectEventHandle, &NetworkEvents);

				// ���� EventID�� SOCKET_ERROR �� ��� ������ �߸��� ���̹Ƿ� �����带 �����մϴ�.
				// ������ ������ WSAGetLastError �Լ��� ���ؼ� Ȯ���� �� �ֽ��ϴ�.
				if (EventID == SOCKET_ERROR)
					return;
				else
				{
					// Connect �̺�Ʈ�� �߻����� ���
					if (NetworkEvents.lNetworkEvents & FD_CONNECT)
					{
						// ���� �Լ��� ȣ�����ݴϴ�.
						OnIoConnected();

						if (NetworkEvents.iErrorCode[FD_CONNECT_BIT])
							return;
					}
					else if (NetworkEvents.lNetworkEvents & FD_WRITE)
					{
						// ������� �ʽ��ϴ�.
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