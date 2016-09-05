#include "PreHeader.h"
#include "CServerIOCP.h"

CServerIOCP::CServerIOCP(VOID)
{

}

CServerIOCP::~CServerIOCP(VOID)
{

}


VOID CServerIOCP::OnIoRead(VOID *_object, DWORD _DataLength)
{
	// ���⼭ ���� �Լ����� �Ѿ�� _object�� OnIoConntected�� ����������
	// ������ ����� ��ü�� �Ѿ���� �˴ϴ�.
	// �̰��� ���⼭ Ŭ���̾�Ʈ�� ������ CConnectedSession���� �� ��ȯ�� �ؼ� �ް� �˴ϴ�.
	// �� ��ȯ�� reinterpret_cast�� ����մϴ�.
	CConnectedSession *connectedSession = reinterpret_cast<CConnectedSession*>(_object);

	// ���� �������ݰ� ��Ŷ ���̸� �����ϴ� ����
	DWORD Protocol = 0, PacketLength = 0;
	// ��Ŷ�� �����ϴ� ����
	BYTE Packet[MAX_BUFFER_LENGTH] = { 0, };

	// CNetworkSession���� CPacketSession���� �����͸� �����ɴϴ�.
	if (connectedSession->ReadPacketForIocp(_DataLength))
	{
		// CPacketSession���� ��Ŷ�� �̾Ƴ��ϴ�.
		while (connectedSession->GetPacket(Protocol, Packet, PacketLength))
		{
			// �������ݿ� ���� Switch��
			switch (Protocol)
			{
			case PT_REG_USER:
				PROC_PT_REG_USER(connectedSession, Protocol, Packet, PacketLength);
				break;

			case PT_QUERY_USER:
				PROC_PT_QUERY_USER(connectedSession, Protocol, Packet, PacketLength);
				break;

			case PT_REG_COMPUTER:
				PROC_PT_REG_COMPUTER(connectedSession, Protocol, Packet, PacketLength);
				break;

			case PT_QUERY_COMPUTER:
				PROC_PT_QUERY_COMPUTER(connectedSession, Protocol, Packet, PacketLength);
				break;

			case PT_REG_PROGRAM:
				PROC_PT_REG_PROGRAM(connectedSession, Protocol, Packet, PacketLength);
				break;

			case PT_QUERY_PROGRAM:
				PROC_PT_QUERY_PROGRAM(connectedSession, Protocol, Packet, PacketLength);
				break;
			}
		}
	}

	if (!connectedSession->InitializeReadForIOCP())
	{
		connectedSession->Restart(listen->GetSocket());
	}
}

VOID CServerIOCP::OnIoWrote(VOID *_object, DWORD _DataLength)
{

}

VOID CServerIOCP::OnIoConnected(VOID *_object)
{
	// ���� ���� �Լ����� �Ѿ�� object�� ������ ����� ��ü�� �Ѿ���� �˴ϴ�.
	// �̰��� ���⼭ Ŭ���̾�Ʈ�� ������ CConnectedSession���� �� ��ȯ�� ���־� �ް� �˴ϴ�.
	// �� ��ȯ�� reinterpret_cast�� ����մϴ�.
	CConnectedSession *ConnectedSession = reinterpret_cast<CConnectedSession*>(_object);

	// ������ ��ü�� IO�� IOCP�� ���ؼ� �ޱ� ���ؼ� IOCP�� ����ϴ� ������ ��ġ�� �˴ϴ�.
	// ���⼭ GetSocket�� �̿��ؼ� ������ ����ϰ� �ش� Ű�� ��ü�� �����͸� �̿��ϰ� �˴ϴ�.
	if (!CIOCP::RegisterSocketToIocp(ConnectedSession->GetSocket(), reinterpret_cast<ULONG_PTR>(ConnectedSession)))
	{
		return;
	}

	// IOCP �ʱ� �ޱ⸦ �������ݴϴ�.
	if (!ConnectedSession->InitializeReadForIOCP())
	{
		// ���� �������� ��� ��ü�� ��������ݴϴ�.
		ConnectedSession->Restart(listen->GetSocket());

		return;
	}

	ConnectedSession->SetConnected(TRUE);
}

VOID CServerIOCP::OnIoDisconnected(VOID *_object)
{
	// ���� ���� �Լ����� �Ѿ�� Object�� OnIoConnected��
	// ���������� ������ ����� ��ü�� �Ѿ���� �˴ϴ�.
	// �̰��� ���⼭ Ŭ���̾�Ʈ�� ������ CConnectedSession���� �� ��ȯ�� ���־� �ް� �˴ϴ�.
	// �� ��ȯ�� reinterpret_castg�� ����մϴ�.
	CConnectedSession *ConnectedSession = reinterpret_cast<CConnectedSession*>(_object);

	// ������ �����Ͽ��� ������ ��ü�� ��������ݴϴ�.
	ConnectedSession->Restart(listen->GetSocket());

	ConnectedSession->SetConnected(FALSE);
}

BOOL CServerIOCP::Begin(VOID)
{
	// ��� ���� IOCP ��ü�� �����մϴ�.
	if (!CIOCP::Begin()) return FALSE;

	// Listen�� ����� ��ü�� �����մϴ�.
	listen = new CNetworkSession();

	// ��ü�� �����մϴ�.
	if (!listen->Begin())
	{
		CServerIOCP::End();

		return FALSE;
	}

	// TCP�� ����� ���� �����մϴ�.
	if (!listen->TCPBind())
	{
		CServerIOCP::End();

		return FALSE;
	}

	// ��Ʈ 1820���� Listen�� �ϰ� �ִ� 100������ �����մϴ�.
	if (!listen->Listen(DEFAULT_PORT, MAX_USER))
	{
		CServerIOCP::End();

		return FALSE;
	}

	// IOCP�� Listen ������ ��Ͻ��� �ݴϴ�. �׸��� Ű�δ� �ش� ��ü�� �����͸� �����մϴ�.
	if (!CIOCP::RegisterSocketToIocp(listen->GetSocket(), reinterpret_cast<ULONG_PTR>(listen)))
	{
		// �������� ��� End �Լ��� ȣ���ϰ� �����մϴ�.
		CServerIOCP::End();

		return FALSE;
	}

	// CConnectedSessionManager�� �����մϴ�.
	// ���� �Լ������� CConnectedSession�� �����ϰ� Accept���·� ����� �ڵ尡 �߰��Ǿ� �ֽ��ϴ�.
	if (!connectedSessionManager.Begin(listen->GetSocket()))
	{
		CServerIOCP::End();

		return FALSE;
	}

	// KeepAlive ���� �̺�Ʈ�� �����մϴ�.
	keepThreadDestroyEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
	if (!keepThreadDestroyEvent)
	{
		// �������� ��� �����մϴ�.
		CServerIOCP::End();
		return FALSE;
	}

	// KeepAlive Thread�� �����մϴ�.
	keepThread = CreateThread(NULL, 0, ::KeepThreadCallback, this, 0, NULL);
	if (!keepThread)
	{
		// ���� �������� ��� �����մϴ�.
		CServerIOCP::End();

		return FALSE;
	}

	return TRUE;
}

VOID CServerIOCP::End(VOID)
{
	// CServerIocp�� ������ ��
	
	for (auto Iter = users.begin(); Iter != users.end(); Iter++)
		delete Iter->second;
	users.clear();

	for (auto Iter = computers.begin(); Iter != computers.end(); Iter++)
		delete Iter->second;
	computers.clear();

	for (auto Iter = programs.begin(); Iter != programs.end(); Iter++)
		delete Iter->second;
	programs.clear();


	// KeepThread�� NULL�� �ƴϸ� KeepAlive Thread�� �������ݴϴ�.
	if (keepThread)
	{
		// ���� �̺�Ʈ�� �߻���ŵ�ϴ�.
		SetEvent(keepThreadDestroyEvent);

		// Thread�� ����� ������ �����մϴ�.
		WaitForSingleObject(keepThread, INFINITE);

		// �ڵ��� �ݾ��ݴϴ�.
		CloseHandle(keepThread);
		keepThread = NULL;
	}

	// KeepAlive ���� �̺�Ʈ�� NULL�� �ƴϸ�
	if (keepThreadDestroyEvent)
	{
		// �̺�Ʈ�� �ݾ��ݴϴ�
		CloseHandle(keepThreadDestroyEvent);
		keepThreadDestroyEvent = NULL;
	}
	
	// IOCP�� �����մϴ�.
	CIOCP::End();

	// CConnectedSessionManager�� �����մϴ�.
	connectedSessionManager.End();

	// �����ߴ� Listen�� ��ü�� �����ϰ� �������ݴϴ�.
	if (listen)
	{
		listen->End();

		delete listen;
	}
}

DWORD WINAPI KeepThreadCallback(LPVOID parameter)
{
	CServerIOCP* owner = (CServerIOCP*)parameter;
	owner->KeepThreadCallback();

	return 0;
}

VOID CServerIOCP::KeepThreadCallback(VOID)
{
	// KeepAlive ��ȣ
	DWORD keepAlive = 0xFFFF;

	while (TRUE)
	{
		// 30�� ���� ���� �̺�Ʈ�� �߻����� ������ �Ѿ�ϴ�.
		DWORD Result = WaitForSingleObject(keepThreadDestroyEvent, 30000);

		// ���� �̹�Ʈ�� �߻����� ��� Thread�� �����մϴ�.
		if (Result == WAIT_OBJECT_0)
			return;

		// �������ִ� ��� Session�� ��Ŷ�� �����մϴ�.
		connectedSessionManager.WriteAll(0x3000000, (BYTE*)&keepAlive, sizeof(DWORD));
	}
}