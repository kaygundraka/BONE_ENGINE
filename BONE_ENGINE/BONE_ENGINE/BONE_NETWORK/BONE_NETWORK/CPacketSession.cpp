#include "Common.h"
#include "CPacketSession.h"
#include "CCrypt.h"

namespace BONE_NETWORK
{
	CPacketSession::CPacketSession(VOID)
	{
		// CNetworkSession�� readBuffer�� �����͸� ������ ����
		ZeroMemory(packetBuffer, sizeof(packetBuffer));
		
		// packetBuffer�� ���� ����, �� ��Ŷ�� ó���ϰ� 
		// nagle �˰��� ���� �ϼ����� ���� ��Ŷ�� ���� �����Դϴ�.
		remainLength = 0;

		// ������ ���¿��� ���� �� ����� ��Ŷ ��ȣ
		currentPacketNumber = 0;

		// ���������� ���� ��Ŷ ��ȣ, �̰����� ������ ���� ��Ŷ ��ȣ������ �����մϴ�.
		lastReadPacketNumber = 0;
	}

	CPacketSession::~CPacketSession(VOID)
	{

	}

	BOOL CPacketSession::Begin(VOID)
	{
		CThreadSync Sync;

		// ��Ŷ �м� ���� CNetworkSession���� ������ �����͸� ��Ƶ� ���۸� �ʱ�ȭ�մϴ�.
		ZeroMemory(packetBuffer, sizeof(packetBuffer));

		remainLength = 0;
		currentPacketNumber = 0;
		lastReadPacketNumber = 0;

		// ������ �����͸� ������ ť�� �ʱ�ȭ�մϴ�.
		if (!writeQueue.Begin())
			return FALSE;

		// ��ӹ��� ��ä�� CNetworkSession�� �ʱ�ȭ �մϴ�.
		return CNetworkSession::Begin();
	}

	BOOL CPacketSession::End(VOID)
	{
		CThreadSync Sync;

		// UDP ���� �еi ���� ���� ����
		lastReadPacketInfoVectorForUdp.clear();

		// ť ����
		if (!writeQueue.End())
			return FALSE;

		// CNetworkSession ��ü ����
		return CNetworkSession::End();
	}

	BOOL CPacketSession::GetPacket(DWORD &_protocol, BYTE *_packet, DWORD &_packetLength)
	{
		CThreadSync Sync;

		if (!_packet)
			return FALSE;

		// remainLength�� packetBuffer�� ������ ���̸� ��Ÿ���µ�,
		// 4����Ʈ �̻��� ���� ������ ó���� ���� �ʽ��ϴ�.
		if (remainLength < sizeof(DWORD))
			return FALSE;

		INT PacketLength = 0;

		// ��Ŷ �տ� 4����Ʈ�� �о� ��Ŷ ���̸� �˾ƿɴϴ�.
		CopyMemory(&PacketLength, packetBuffer, sizeof(INT));

		// ��Ŷ ���̰� �ִ� ���� ���̸� �Ѱų� 0���� ���� ��� 
		// �߸��� ��Ŷ�� �Ǵ��ϰ� ���۸� ����ϴ�.
		if (PacketLength > MAX_BUFFER_LENGTH || PacketLength <= 0)
		{
			remainLength = 0;
			return FALSE;
		}

		// ���� ��Ŷ ���̰� ���ۿ� �ִ� ���̺��� �۰ų�,
		// �� ������ �� ���� ��Ŷ�� �ְų� �� �̻��� ���� ��
		if (PacketLength <= remainLength)
		{
			// ��Ŷ ���� ������ �κ��� ��ȣȭ�� Ǯ���ݴϴ�.
			CCrypt::Decrypt(
				packetBuffer + sizeof(DWORD)/*LENGTH*/,
				packetBuffer + sizeof(DWORD)/*LENGTH*/,
				PacketLength - sizeof(DWORD)/*LENGTH*/);

			DWORD PacketNumber = 0;
			DWORD Protocol = 0;
			
			// ��Ŷ ���̿� ��ȣ�� �˾ƿɴϴ�.
			CopyMemory(&PacketNumber, packetBuffer
				+ sizeof(DWORD)/*LENGTH*/
				, sizeof(DWORD));

			CopyMemory(&Protocol, packetBuffer
				+ sizeof(DWORD)/*LENGTH*/
				+ sizeof(DWORD)/*PACKETNUMBER*/
				, sizeof(DWORD));

			// �Ķ������ ���� �Է����ݴϴ�.
			_protocol = Protocol;
			_packetLength = PacketLength
				- sizeof(DWORD)/*LENGTH*/
				- sizeof(DWORD)/*PACKETNUMBER*/
				- sizeof(DWORD)/*PROTOCOL*/;

			CopyMemory(_packet, packetBuffer
				+ sizeof(DWORD)/*LENGTH*/
				+ sizeof(DWORD)/*PACKETNUMBER*/
				+ sizeof(DWORD)/*PROTOCOL*/
				, _packetLength);

			// �� ��Ŷ�� ó���ϰ� ���� ���̰� 0���� Ŭ ��� ���� �޸𸮸� ������ ����ݴϴ�.
			if (remainLength - PacketLength > 0)
				MoveMemory(packetBuffer, packetBuffer + PacketLength, remainLength - PacketLength);

			// �� ��Ŷ�� ó���� ��ŭ ���� ���̸� �ٿ��ݴϴ�.
			remainLength -= PacketLength;

			// ���� ���̰� 0�̰ų� 0���� ���� ��� �������ݴϴ�.
			if (remainLength <= 0)
			{
				remainLength = 0;
				ZeroMemory(packetBuffer, sizeof(packetBuffer));
			}

			// ��Ŷ ��ȣ�� ���� ���� ���� ��� ���ۿ��� ���ܽ�Ű�� �����մϴ�.
			if (PacketNumber <= lastReadPacketNumber)
				return FALSE;
			else
			{
				// ���ο� ��Ŷ ��ȣ�� ���, ������ ���� ��Ŷ�� �����ϴ� ������ ������Ʈ�մϴ�.
				lastReadPacketNumber = PacketNumber;

				return TRUE;
			}
		}

		return FALSE;
	}

	BOOL CPacketSession::GetPacket(LPSTR _remoteAddress, USHORT _remotePort, DWORD &_protocol, BYTE *_packet, DWORD &_packetLength)
	{
		CThreadSync Sync;

		if (!_packet)
			return FALSE;

		// remainLength�� packetBuffer�� ������ ���̸� ��Ÿ���µ�,
		// 4����Ʈ �̻��� ���� ������ ó���� ���� �ʽ��ϴ�.
		if (remainLength < sizeof(DWORD))
			return FALSE;

		INT PacketLength = 0;

		// ��Ŷ �տ� 4����Ʈ�� �о� ��Ŷ ���̸� �˾ƿɴϴ�.
		CopyMemory(&PacketLength, packetBuffer, sizeof(INT));

		// ��Ŷ ���̰� �ִ� ���� ���̸� �Ѱų� 0���� ���� ��� 
		// �߸��� ��Ŷ�� �Ǵ��ϰ� ���۸� ����ϴ�.
		if (PacketLength > MAX_BUFFER_LENGTH || PacketLength <= 0)
		{
			remainLength = 0;
			return FALSE;
		}

		// ���� ��Ŷ ���̰� ���ۿ� �ִ� ���̺��� �۰ų�,
		// �� ������ �� ���� ��Ŷ�� �ְų� �� �̻��� ���� ��
		if (PacketLength <= remainLength)
		{
			// ��Ŷ ���� ������ �κ��� ��ȣȭ�� Ǯ���ݴϴ�.
			CCrypt::Decrypt(
				packetBuffer + sizeof(DWORD)/*LENGTH*/,
				packetBuffer + sizeof(DWORD)/*LENGTH*/,
				PacketLength - sizeof(DWORD)/*LENGTH*/);

			DWORD PacketNumber = 0;
			DWORD Protocol = 0;

			// ��Ŷ ���̿� ��ȣ�� �˾ƿɴϴ�.
			CopyMemory(&PacketNumber, packetBuffer
				+ sizeof(DWORD)/*LENGTH*/
				, sizeof(DWORD));

			CopyMemory(&Protocol, packetBuffer
				+ sizeof(DWORD)/*LENGTH*/
				+ sizeof(DWORD)/*PACKETNUMBER*/
				, sizeof(DWORD));

			// �Ķ������ ���� �Է����ݴϴ�.
			_protocol = Protocol;
			_packetLength = PacketLength
				- sizeof(DWORD)/*LENGTH*/
				- sizeof(DWORD)/*PACKETNUMBER*/
				- sizeof(DWORD)/*PROTOCOL*/;

			CopyMemory(_packet, packetBuffer
				+ sizeof(DWORD)/*LENGTH*/
				+ sizeof(DWORD)/*PACKETNUMBER*/
				+ sizeof(DWORD)/*PROTOCOL*/
				, _packetLength);

			// �� ��Ŷ�� ó���ϰ� ���� ���̰� 0���� Ŭ ��� ���� �޸𸮸� ������ ����ݴϴ�.
			if (remainLength - PacketLength > 0)
				MoveMemory(packetBuffer, packetBuffer + PacketLength, remainLength - PacketLength);

			// �� ��Ŷ�� ó���� ��ŭ ���� ���̸� �ٿ��ݴϴ�.
			remainLength -= PacketLength;

			// ���� ���̰� 0�̰ų� 0���� ���� ��� �������ݴϴ�.
			if (remainLength <= 0)
			{
				remainLength = 0;
				ZeroMemory(packetBuffer, sizeof(packetBuffer));
			}

			// lastReadPacketInfoVectorForUdp���� �����͸� ���� ���� �ִ��� �˻��մϴ�.
			// ���� ���� ��� �� ������ ��Ŷ ��ȣ ���� ������Ʈ�մϴ�.
			for (DWORD i = 0; i < lastReadPacketInfoVectorForUdp.size(); i++)
			{
				READ_PACKET_INFO ReadPacketInfo = lastReadPacketInfoVectorForUdp[i];

				// ���� �ּ�, ���� ��Ʈ�� ������ �̹� ���� lastReadPacketInfoVectorForUdp�� �ִ��� �˻��մϴ�.
				if (!strcmp(ReadPacketInfo.RemoteAddress, _remoteAddress) && ReadPacketInfo.RemotePort == _remotePort)
				{
					// ���� ���� �������� ��Ŷ ��ȣ�� ���ݱ����� ��ȣ���� ū ��ȣ�� ��� ������Ʈ�� ���ݴϴ�.
					if (ReadPacketInfo.PacketNumber < PacketNumber)
					{
						lastReadPacketInfoVectorForUdp[i].PacketNumber = PacketNumber;

						return TRUE;
					}
					else
						return FALSE;
				}
			}

			// ���� ���ο� �ּҿ��� ���� �������� ��� ���� ���Ϳ� �߰����ݴϴ�.
			READ_PACKET_INFO ReadPacketInfo;
			ReadPacketInfo.PacketNumber = PacketNumber;
			ReadPacketInfo.RemotePort = _remotePort;
			strcpy(ReadPacketInfo.RemoteAddress, _remoteAddress);

			lastReadPacketInfoVectorForUdp.push_back(ReadPacketInfo);

			return TRUE;
		}

		return FALSE;
	}

	BOOL CPacketSession::ReadPacketForIocp(DWORD _readLength)
	{
		CThreadSync Sync;

		// CNetworkSession�� �Լ��� �̿��ؼ� paketBuffer�� ���� �����͸� �Է��մϴ�.
		if (!CNetworkSession::ReadForIOCP(packetBuffer + remainLength, _readLength))
			return FALSE;

		// packetBuffer�� ������ ���̸� ���������ݴϴ�.
		remainLength += _readLength;

		return TRUE;
	}

	// ReadPacketForEventSelect�� FALSE�� �������� ���� while���� ������.
	BOOL CPacketSession::ReadPacketForEventSelect(VOID)
	{
		CThreadSync Sync;

		DWORD ReadLength = 0;

		if (!CNetworkSession::ReadForEventSelect(packetBuffer + remainLength, ReadLength))
			return FALSE;

		remainLength += ReadLength;

		//return getPacket(protocol, packet, packetLength);
		return TRUE;
	}

	// ReadPacketForEventSelect�� FALSE�� �������� ���� while���� ������.
	BOOL CPacketSession::ReadFromPacketForEventSelect(LPSTR _remoteAddress, USHORT &_remotePort)
	{
		CThreadSync Sync;

		DWORD ReadLength = 0;

		if (!CNetworkSession::ReadFromForEventSelect(_remoteAddress, _remotePort, packetBuffer + remainLength, ReadLength))
			return FALSE;

		remainLength += ReadLength;

		//return getPacket(remoteAddress, remotePort, protocol, packet, packetLength);
		return TRUE;
	}
	
	BOOL CPacketSession::WritePacket(DWORD _protocol, const BYTE *_packet, DWORD _packetLength)
	{
		CThreadSync Sync;

		if (!_packet)
			return FALSE;

		// LENGTH(4) | PACKET_NUMBER(4) | PROTOCOL(4) | DATA(4084)
		// ��Ŷ�� ��ü ���� ���
		// ��Ŷ ��ü ���� = ��Ŷ ���� + ��Ŷ ���� ��ȣ + �������� + ������ ����
		DWORD PacketLength =
			sizeof(DWORD) /*LENGTH*/ +
			sizeof(DWORD) /*PACKETNUMBER*/ +
			sizeof(DWORD) /*PROTOCOL*/ +
			_packetLength;

		// ��Ŷ ���̰� �ִ� ���� ���̺��� ��� �����մϴ�.
		if (PacketLength >= MAX_BUFFER_LENGTH)
			return FALSE;

		// ��ų ��ȣ�� �ϳ� ������ŵ�ϴ�.
		currentPacketNumber++;

		// ���� �����͸� �̿��ؼ� ��Ŷ�� ����� ���� ����
		BYTE TempBuffer[MAX_BUFFER_LENGTH] = { 0, };

		// ��Ŷ�� ����ϴ�.
		// ��Ŷ �ε����� �ٿ� ���� ������ ���־�� �մϴ�.
		CopyMemory(TempBuffer, &PacketLength, sizeof(DWORD));

		// ��Ŷ ���̸� ���ݴϴ�.
		CopyMemory(TempBuffer
			+ sizeof(DWORD)/*LENGTH*/,
			&currentPacketNumber, sizeof(DWORD));
			
		// ��Ŷ ��ȣ�� ���ݴϴ�.
		CopyMemory(TempBuffer +
			sizeof(DWORD)/*LENGTH*/ +
			sizeof(DWORD)/*PACKETNUMBER*/,
			&_protocol, sizeof(DWORD));

		// ���������� ���ݴϴ�.
		CopyMemory(TempBuffer +
			sizeof(DWORD)/*LENGTH*/ +
			sizeof(DWORD)/*PACKETNUMBER*/ +
			sizeof(DWORD)/*PROTOCOL*/,
			_packet, PacketLength);

		// ������ �����͸� Encrypt �Լ��� �̿��ؼ� ��ȣȭ�մϴ�.
		CCrypt::Encrypt(TempBuffer + sizeof(DWORD), TempBuffer + sizeof(DWORD), PacketLength - sizeof(DWORD));
		
		// writeQueue�� �̿��ؼ� ��Ŷ�� ���� �Ϸ�Ǿ��� ������ �޸𸮸� ����Ӵϴ�.
		BYTE *WriteData = writeQueue.Push(this, TempBuffer, PacketLength);

		// Queue�� ������ �����͸� �־ �����ݴϴ�.
		return CNetworkSession::Write(WriteData, PacketLength);
	}

	BOOL CPacketSession::WriteComplete(VOID)
	{
		CThreadSync Sync;

		return writeQueue.Pop();
	}
}