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
	// 여기서 가상 함수에서 넘어온 _object는 OnIoConntected와 마찬가지로
	// 접속을 담당한 개체가 넘어오게 됩니다.
	// 이것을 여기서 클라이언트를 관리할 CConnectedSession으로 형 변환을 해서 받게 됩니다.
	// 형 변환은 reinterpret_cast를 사용합니다.
	CConnectedSession *connectedSession = reinterpret_cast<CConnectedSession*>(_object);

	// 받은 프로토콜과 패킷 길이를 저장하는 변수
	DWORD Protocol = 0, PacketLength = 0;
	// 패킷을 저장하는 변수
	BYTE Packet[MAX_BUFFER_LENGTH] = { 0, };

	// CNetworkSession에서 CPacketSession으로 데이터를 가져옵니다.
	if (connectedSession->ReadPacketForIocp(_DataLength))
	{
		// CPacketSession에서 패킷을 뽑아냅니다.
		while (connectedSession->GetPacket(Protocol, Packet, PacketLength))
		{
			// 프로토콜에 따른 Switch문
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
	// 여기 가상 함수에서 넘어온 object는 접속을 담당한 개체가 넘어오게 됩니다.
	// 이것을 여기서 클라이언트를 관리할 CConnectedSession으로 형 변환을 해주어 받게 됩니다.
	// 형 변환은 reinterpret_cast를 사용합니다.
	CConnectedSession *ConnectedSession = reinterpret_cast<CConnectedSession*>(_object);

	// 접속한 개체의 IO를 IOCP를 통해서 받기 위해서 IOCP에 등록하는 과정을 거치게 됩니다.
	// 여기서 GetSocket을 이용해서 소켓을 등록하고 해당 키는 개체의 포인터를 이용하게 됩니다.
	if (!CIOCP::RegisterSocketToIocp(ConnectedSession->GetSocket(), reinterpret_cast<ULONG_PTR>(ConnectedSession)))
	{
		return;
	}

	// IOCP 초기 받기를 실행해줍니다.
	if (!ConnectedSession->InitializeReadForIOCP())
	{
		// 만일 실패했을 경우 개체를 재시작해줍니다.
		ConnectedSession->Restart(listen->GetSocket());

		return;
	}

	ConnectedSession->SetConnected(TRUE);
}

VOID CServerIOCP::OnIoDisconnected(VOID *_object)
{
	// 여기 가상 함수에서 넘어온 Object는 OnIoConnected와
	// 마찬가지로 접속을 담당한 개체가 넘어오게 됩니다.
	// 이것을 여기서 클라이언트를 관리할 CConnectedSession으로 형 변환을 해주어 받게 됩니다.
	// 형 변환은 reinterpret_castg를 사용합니다.
	CConnectedSession *ConnectedSession = reinterpret_cast<CConnectedSession*>(_object);

	// 접속을 종료하였기 때문에 개체를 재시작해줍니다.
	ConnectedSession->Restart(listen->GetSocket());

	ConnectedSession->SetConnected(FALSE);
}

BOOL CServerIOCP::Begin(VOID)
{
	// 상속 받은 IOCP 개체를 시작합니다.
	if (!CIOCP::Begin()) return FALSE;

	// Listen에 사용할 개체를 생성합니다.
	listen = new CNetworkSession();

	// 개체를 시작합니다.
	if (!listen->Begin())
	{
		CServerIOCP::End();

		return FALSE;
	}

	// TCP로 사용할 것을 선언합니다.
	if (!listen->TCPBind())
	{
		CServerIOCP::End();

		return FALSE;
	}

	// 포트 1820으로 Listen을 하고 최대 100명으로 설정합니다.
	if (!listen->Listen(DEFAULT_PORT, MAX_USER))
	{
		CServerIOCP::End();

		return FALSE;
	}

	// IOCP에 Listen 소켓을 등록시켜 줍니다. 그리고 키로는 해당 개체의 포인터를 설정합니다.
	if (!CIOCP::RegisterSocketToIocp(listen->GetSocket(), reinterpret_cast<ULONG_PTR>(listen)))
	{
		// 실패했을 경우 End 함수를 호출하고 종료합니다.
		CServerIOCP::End();

		return FALSE;
	}

	// CConnectedSessionManager를 시작합니다.
	// 시작 함수내에는 CConnectedSession을 생성하고 Accept상태로 만드는 코드가 추가되어 있습니다.
	if (!connectedSessionManager.Begin(listen->GetSocket()))
	{
		CServerIOCP::End();

		return FALSE;
	}

	// KeepAlive 종료 이벤트를 생성합니다.
	keepThreadDestroyEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
	if (!keepThreadDestroyEvent)
	{
		// 실패했을 경우 종료합니다.
		CServerIOCP::End();
		return FALSE;
	}

	// KeepAlive Thread를 생성합니다.
	keepThread = CreateThread(NULL, 0, ::KeepThreadCallback, this, 0, NULL);
	if (!keepThread)
	{
		// 생성 실패했을 경우 종료합니다.
		CServerIOCP::End();

		return FALSE;
	}

	return TRUE;
}

VOID CServerIOCP::End(VOID)
{
	// CServerIocp를 종료할 때
	
	for (auto Iter = users.begin(); Iter != users.end(); Iter++)
		delete Iter->second;
	users.clear();

	for (auto Iter = computers.begin(); Iter != computers.end(); Iter++)
		delete Iter->second;
	computers.clear();

	for (auto Iter = programs.begin(); Iter != programs.end(); Iter++)
		delete Iter->second;
	programs.clear();


	// KeepThread가 NULL이 아니면 KeepAlive Thread를 종료해줍니다.
	if (keepThread)
	{
		// 종료 이벤트를 발생시킵니다.
		SetEvent(keepThreadDestroyEvent);

		// Thread가 종료될 때까지 종료합니다.
		WaitForSingleObject(keepThread, INFINITE);

		// 핸들을 닫아줍니다.
		CloseHandle(keepThread);
		keepThread = NULL;
	}

	// KeepAlive 종료 이벤트가 NULL이 아니면
	if (keepThreadDestroyEvent)
	{
		// 이벤트를 닫아줍니다
		CloseHandle(keepThreadDestroyEvent);
		keepThreadDestroyEvent = NULL;
	}
	
	// IOCP를 종료합니다.
	CIOCP::End();

	// CConnectedSessionManager를 종료합니다.
	connectedSessionManager.End();

	// 생성했던 Listen용 개체를 종료하고 삭제해줍니다.
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
	// KeepAlive 신호
	DWORD keepAlive = 0xFFFF;

	while (TRUE)
	{
		// 30초 동안 종료 이벤트가 발생하지 않으면 넘어갑니다.
		DWORD Result = WaitForSingleObject(keepThreadDestroyEvent, 30000);

		// 종료 이밴트가 발생했을 경우 Thread를 종료합니다.
		if (Result == WAIT_OBJECT_0)
			return;

		// 접속해있는 모든 Session에 패킷을 전송합니다.
		connectedSessionManager.WriteAll(0x3000000, (BYTE*)&keepAlive, sizeof(DWORD));
	}
}