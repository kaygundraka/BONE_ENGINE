#include "Common.h"
#include "ClientSession.h"

namespace BONE_NETWORK
{
	BOOL CClientSession::BeginTCP(LPSTR _remoteAddress, USHORT _remotePort)
	{
		if (!_remoteAddress || _remotePort <= 0)
			return FALSE;

		// ����� ���� ��ü�� �ʱ�ȭ�մϴ�.
		if (!session.Begin())
		{
			End();

			return FALSE;
		}

		// ������ ��ü�� TCP�� �����մϴ�.
		if (!session.TCPBind())
		{
			End();

			return FALSE;
		}

		// EventSelect ������� �ش� ������ �̺�Ʈ�� �޾ƿ��� ���� ����մϴ�.
		if (!CEventSelect::Begin(session.GetSocket()))
		{
			End();

			return FALSE;
		}

		// ������ �ּҿ� �����մϴ�.
		if (!session.Connect(_remoteAddress, _remotePort))
		{
			End();

			return FALSE;
		}

		// TCP�� ����� ���� �����մϴ�.
		IsUDP = FALSE;

		return TRUE;
	}

	BOOL CClientSession::BeginUDP(USHORT _remotePort)
	{
		if (_remotePort <= 0)
			return FALSE;

		// ����� ���� ��ü�� �ʱ�ȭ�մϴ�.
		if (!session.Begin())
		{
			End();

			return FALSE;
		}

		// ������ ��ü�� UDP�� �����մϴ�.
		if (!session.UDPBind(_remotePort))
		{
			End();

			return FALSE;
		}

		// EventSelect�� ����մϴ�.
		if (!CEventSelect::Begin(session.GetSocket()))
		{
			End();

			return FALSE;
		}

		// UDP�� ����� ���� �����մϴ�.
		IsUDP = TRUE;
		
		return TRUE;
	}

	BOOL CClientSession::End(VOID)
	{
		// ���� ��ü�� �����մϴ�.
		session.End();

		// EventSelect ��ü�� �����մϴ�.
		CEventSelect::End();

		return TRUE;
	}
	
	// TCP �����͸� �����ϴ� �Լ�
	// �Ķ���ͷ� ����� ��������, ��Ŷ, ��Ŷ ���̸� �־��ְ� �˴ϴ�.
	BOOL CClientSession::WritePacket(DWORD _protocol, const BYTE *_packet, DWORD _packetLength)
	{
		// ������ ���� �Լ��� ȣ��
		if (!session.WritePacket(_protocol, _packet, _packetLength))
			return FALSE;

		// �ٷ� WriteComplete�� ���ݴϴ�.
		// �ֳ��ϸ� CEventSelect�� ��� FD_WRITE�� �� ������ ���� ���� �߻����� ���� ���� �ֱ� �����Դϴ�.
		if (!session.WriteComplete())
			return FALSE;

		return TRUE;
	}
	
	// ��Ŷ �� ���� �޾ƿɴϴ�.
	BOOL CClientSession::ReadPacket(DWORD &_protocol, BYTE *_packet, DWORD &_packetLength)
	{
		VOID *Object = NULL;

		// ť���� ��Ŷ �� ���� �޾ƿɴϴ�.
		return readPacketQueue.Pop(&Object, _protocol, _packet, _packetLength);
	}

	// �����Ͱ� �߻����� �� ȣ��Ǵ� ���� �Լ��Դϴ�.
	VOID CClientSession::OnIoRead(VOID)
	{
		// ��Ŷ�� �����ϴ� ����
		BYTE Packet[MAX_BUFFER_LENGTH] = { 0, };

		// ��Ŷ�� ���̸� �����ϴ� ����
		DWORD PacketLength = 0;

		// ���������� �����ϴ� ����
		DWORD Protocol = 0;

		// UDP�� ���� TCP�� ��츦 �����մϴ�.
		if (IsUDP)
		{
			// UDP�� ��쿡�� ���� �ּҿ� ��Ʈ ������ �޾ƿ;� �ϱ� ������ �ٷ� ������ �����Ͽ����ϴ�.
			CHAR RemoteAddress[32] = { 0, };
			USHORT RemotePort = 0;

			// CNetworkSession���� CPacketSession���� �����͸� �����ϰ�
			if (session.ReadFromPacketForEventSelect(RemoteAddress, RemotePort))
			{
				// ��Ŷ�� �����ɴϴ�.
				while (session.GetPacket(RemoteAddress, RemotePort, Protocol, Packet, PacketLength))
				{
					// ������ ��Ŷ�� ť�� �����մϴ�.
					readPacketQueue.Push(this, Protocol, Packet, PacketLength, RemoteAddress, RemotePort);
				}
			}
		}
		else
		{
			// CNetworkSession���� CPacketSession���� �����͸� �����ϰ�
			if (session.ReadPacketForEventSelect())
			{
				// ��Ŷ�� �����ɴϴ�.
				while (session.GetPacket(Protocol, Packet, PacketLength))
				{
					// ������ ��Ŷ�� ť�� �����մϴ�.
					readPacketQueue.Push(this, Protocol, Packet, PacketLength);
				}
			}
		}
	}
}