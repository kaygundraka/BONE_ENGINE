#include "Common.h"
#include "ClientSession.h"

namespace BONE_NETWORK
{
	BOOL CClientSession::BeginTCP(LPSTR _remoteAddress, USHORT _remotePort)
	{
		if (!_remoteAddress || _remotePort <= 0)
			return FALSE;

		// 사용할 접속 개체를 초기화합니다.
		if (!session.Begin())
		{
			End();

			return FALSE;
		}

		// 접속할 개체를 TCP로 선언합니다.
		if (!session.TCPBind())
		{
			End();

			return FALSE;
		}

		// EventSelect 방식으로 해당 소켓의 이벤트를 받아오는 것을 등록합니다.
		if (!CEventSelect::Begin(session.GetSocket()))
		{
			End();

			return FALSE;
		}

		// 접속할 주소에 접속합니다.
		if (!session.Connect(_remoteAddress, _remotePort))
		{
			End();

			return FALSE;
		}

		// TCP로 사용할 것을 저장합니다.
		IsUDP = FALSE;

		return TRUE;
	}

	BOOL CClientSession::BeginUDP(USHORT _remotePort)
	{
		if (_remotePort <= 0)
			return FALSE;

		// 사용할 접속 개체를 초기화합니다.
		if (!session.Begin())
		{
			End();

			return FALSE;
		}

		// 접속할 개체를 UDP로 선언합니다.
		if (!session.UDPBind(_remotePort))
		{
			End();

			return FALSE;
		}

		// EventSelect에 등록합니다.
		if (!CEventSelect::Begin(session.GetSocket()))
		{
			End();

			return FALSE;
		}

		// UDP로 사용할 것을 저장합니다.
		IsUDP = TRUE;
		
		return TRUE;
	}

	BOOL CClientSession::End(VOID)
	{
		// 연결 개체를 종료합니다.
		session.End();

		// EventSelect 개체를 종료합니다.
		CEventSelect::End();

		return TRUE;
	}
	
	// TCP 데이터를 전송하는 함수
	// 파라미터로 사용할 프로토콜, 패킷, 패킷 길이를 넣어주게 됩니다.
	BOOL CClientSession::WritePacket(DWORD _protocol, const BYTE *_packet, DWORD _packetLength)
	{
		// 데이터 전송 함수를 호출
		if (!session.WritePacket(_protocol, _packet, _packetLength))
			return FALSE;

		// 바로 WriteComplete를 해줍니다.
		// 왜냐하면 CEventSelect의 경우 FD_WRITE가 꼭 보내기 성공 이후 발생하지 않을 수도 있기 때문입니다.
		if (!session.WriteComplete())
			return FALSE;

		return TRUE;
	}
	
	// 패킷 한 개를 받아옵니다.
	BOOL CClientSession::ReadPacket(DWORD &_protocol, BYTE *_packet, DWORD &_packetLength)
	{
		VOID *Object = NULL;

		// 큐에서 패킷 한 개를 받아옵니다.
		return readPacketQueue.Pop(&Object, _protocol, _packet, _packetLength);
	}

	// 데이터가 발생했을 때 호출되는 가상 함수입니다.
	VOID CClientSession::OnIoRead(VOID)
	{
		// 패킷을 저장하는 버퍼
		BYTE Packet[MAX_BUFFER_LENGTH] = { 0, };

		// 패킷의 길이를 저장하는 변수
		DWORD PacketLength = 0;

		// 프로토콜을 저장하는 변수
		DWORD Protocol = 0;

		// UDP일 경우와 TCP일 경우를 구분합니다.
		if (IsUDP)
		{
			// UDP일 경우에는 보낸 주소와 포트 정보를 받아와야 하기 때문에 바로 변수로 지정하였습니다.
			CHAR RemoteAddress[32] = { 0, };
			USHORT RemotePort = 0;

			// CNetworkSession에서 CPacketSession으로 데이터를 복사하고
			if (session.ReadFromPacketForEventSelect(RemoteAddress, RemotePort))
			{
				// 패킷을 가져옵니다.
				while (session.GetPacket(RemoteAddress, RemotePort, Protocol, Packet, PacketLength))
				{
					// 가져온 패킷을 큐에 저장합니다.
					readPacketQueue.Push(this, Protocol, Packet, PacketLength, RemoteAddress, RemotePort);
				}
			}
		}
		else
		{
			// CNetworkSession에서 CPacketSession으로 데이터를 복사하고
			if (session.ReadPacketForEventSelect())
			{
				// 패킷을 가져옵니다.
				while (session.GetPacket(Protocol, Packet, PacketLength))
				{
					// 가져온 패킷을 큐에 저장합니다.
					readPacketQueue.Push(this, Protocol, Packet, PacketLength);
				}
			}
		}
	}
}