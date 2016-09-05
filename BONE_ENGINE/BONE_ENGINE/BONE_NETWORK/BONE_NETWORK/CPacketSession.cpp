#include "Common.h"
#include "CPacketSession.h"
#include "CCrypt.h"

namespace BONE_NETWORK
{
	CPacketSession::CPacketSession(VOID)
	{
		// CNetworkSession의 readBuffer에 데이터를 가져올 버퍼
		ZeroMemory(packetBuffer, sizeof(packetBuffer));
		
		// packetBuffer에 남은 길이, 즉 패킷을 처리하고 
		// nagle 알고리즘에 의해 완성되지 않은 패킷이 남은 길이입니다.
		remainLength = 0;

		// 현재의 상태에서 보낼 때 사용할 패킷 번호
		currentPacketNumber = 0;

		// 마지막으로 받은 패킷 번호, 이것으로 기존에 보낸 패킷 번호인지를 검증합니다.
		lastReadPacketNumber = 0;
	}

	CPacketSession::~CPacketSession(VOID)
	{

	}

	BOOL CPacketSession::Begin(VOID)
	{
		CThreadSync Sync;

		// 패킷 분석 전에 CNetworkSession에서 가져온 데이터를 담아둘 버퍼를 초기화합니다.
		ZeroMemory(packetBuffer, sizeof(packetBuffer));

		remainLength = 0;
		currentPacketNumber = 0;
		lastReadPacketNumber = 0;

		// 보내는 데이터를 저장할 큐를 초기화합니다.
		if (!writeQueue.Begin())
			return FALSE;

		// 상속받은 개채인 CNetworkSession을 초기화 합니다.
		return CNetworkSession::Begin();
	}

	BOOL CPacketSession::End(VOID)
	{
		CThreadSync Sync;

		// UDP 받은 패킿 정보 저장 벡터
		lastReadPacketInfoVectorForUdp.clear();

		// 큐 종료
		if (!writeQueue.End())
			return FALSE;

		// CNetworkSession 개체 종료
		return CNetworkSession::End();
	}

	BOOL CPacketSession::GetPacket(DWORD &_protocol, BYTE *_packet, DWORD &_packetLength)
	{
		CThreadSync Sync;

		if (!_packet)
			return FALSE;

		// remainLength는 packetBuffer에 데이터 길이를 나타내는데,
		// 4바이트 이상을 받지 않으면 처리가 되지 않습니다.
		if (remainLength < sizeof(DWORD))
			return FALSE;

		INT PacketLength = 0;

		// 패킷 앞에 4바이트를 읽어 패킷 길이를 알아옵니다.
		CopyMemory(&PacketLength, packetBuffer, sizeof(INT));

		// 패킷 길이가 최대 버퍼 길이를 넘거나 0보다 작은 경우 
		// 잘못된 패킷을 판단하고 버퍼를 지웁니다.
		if (PacketLength > MAX_BUFFER_LENGTH || PacketLength <= 0)
		{
			remainLength = 0;
			return FALSE;
		}

		// 읽은 패킷 길이가 버퍼에 있는 길이보다 작거나,
		// 즉 온전한 한 개의 패킷이 있거나 그 이상이 있을 때
		if (PacketLength <= remainLength)
		{
			// 패킷 길이 이하의 부분의 암호화를 풀어줍니다.
			CCrypt::Decrypt(
				packetBuffer + sizeof(DWORD)/*LENGTH*/,
				packetBuffer + sizeof(DWORD)/*LENGTH*/,
				PacketLength - sizeof(DWORD)/*LENGTH*/);

			DWORD PacketNumber = 0;
			DWORD Protocol = 0;
			
			// 패킷 길이와 번호를 알아옵니다.
			CopyMemory(&PacketNumber, packetBuffer
				+ sizeof(DWORD)/*LENGTH*/
				, sizeof(DWORD));

			CopyMemory(&Protocol, packetBuffer
				+ sizeof(DWORD)/*LENGTH*/
				+ sizeof(DWORD)/*PACKETNUMBER*/
				, sizeof(DWORD));

			// 파라미터의 값을 입력해줍니다.
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

			// 한 패킷을 처리하고 남은 길이가 0보다 클 경우 뒤의 메모리를 앞으로 당겨줍니다.
			if (remainLength - PacketLength > 0)
				MoveMemory(packetBuffer, packetBuffer + PacketLength, remainLength - PacketLength);

			// 한 패킷을 처리한 만큼 남은 길이를 줄여줍니다.
			remainLength -= PacketLength;

			// 남은 길이가 0이거나 0보다 작은 경우 리셋해줍니다.
			if (remainLength <= 0)
			{
				remainLength = 0;
				ZeroMemory(packetBuffer, sizeof(packetBuffer));
			}

			// 패킷 번호가 같은 것이 왔을 경우 버퍼에서 제외시키고 무시합니다.
			if (PacketNumber <= lastReadPacketNumber)
				return FALSE;
			else
			{
				// 새로운 패킷 번호일 경우, 마지막 받은 패킷을 저장하는 변수를 업데이트합니다.
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

		// remainLength는 packetBuffer에 데이터 길이를 나타내는데,
		// 4바이트 이상을 받지 않으면 처리가 되지 않습니다.
		if (remainLength < sizeof(DWORD))
			return FALSE;

		INT PacketLength = 0;

		// 패킷 앞에 4바이트를 읽어 패킷 길이를 알아옵니다.
		CopyMemory(&PacketLength, packetBuffer, sizeof(INT));

		// 패킷 길이가 최대 버퍼 길이를 넘거나 0보다 작은 경우 
		// 잘못된 패킷을 판단하고 버퍼를 지웁니다.
		if (PacketLength > MAX_BUFFER_LENGTH || PacketLength <= 0)
		{
			remainLength = 0;
			return FALSE;
		}

		// 읽은 패킷 길이가 버퍼에 있는 길이보다 작거나,
		// 즉 온전한 한 개의 패킷이 있거나 그 이상이 있을 때
		if (PacketLength <= remainLength)
		{
			// 패킷 길이 이하의 부분의 암호화를 풀어줍니다.
			CCrypt::Decrypt(
				packetBuffer + sizeof(DWORD)/*LENGTH*/,
				packetBuffer + sizeof(DWORD)/*LENGTH*/,
				PacketLength - sizeof(DWORD)/*LENGTH*/);

			DWORD PacketNumber = 0;
			DWORD Protocol = 0;

			// 패킷 길이와 번호를 알아옵니다.
			CopyMemory(&PacketNumber, packetBuffer
				+ sizeof(DWORD)/*LENGTH*/
				, sizeof(DWORD));

			CopyMemory(&Protocol, packetBuffer
				+ sizeof(DWORD)/*LENGTH*/
				+ sizeof(DWORD)/*PACKETNUMBER*/
				, sizeof(DWORD));

			// 파라미터의 값을 입력해줍니다.
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

			// 한 패킷을 처리하고 남은 길이가 0보다 클 경우 뒤의 메모리를 앞으로 당겨줍니다.
			if (remainLength - PacketLength > 0)
				MoveMemory(packetBuffer, packetBuffer + PacketLength, remainLength - PacketLength);

			// 한 패킷을 처리한 만큼 남은 길이를 줄여줍니다.
			remainLength -= PacketLength;

			// 남은 길이가 0이거나 0보다 작은 경우 리셋해줍니다.
			if (remainLength <= 0)
			{
				remainLength = 0;
				ZeroMemory(packetBuffer, sizeof(packetBuffer));
			}

			// lastReadPacketInfoVectorForUdp에서 데이터를 받은 적이 있는지 검사합니다.
			// 만약 있을 경우 그 정보에 패킷 번호 만을 업데이트합니다.
			for (DWORD i = 0; i < lastReadPacketInfoVectorForUdp.size(); i++)
			{
				READ_PACKET_INFO ReadPacketInfo = lastReadPacketInfoVectorForUdp[i];

				// 같은 주소, 같은 포트의 정보가 이미 벡터 lastReadPacketInfoVectorForUdp에 있는지 검사합니다.
				if (!strcmp(ReadPacketInfo.RemoteAddress, _remoteAddress) && ReadPacketInfo.RemotePort == _remotePort)
				{
					// 만약 받은 데이터의 패킷 번호가 지금까지의 번호보다 큰 번호일 경우 업데이트를 해줍니다.
					if (ReadPacketInfo.PacketNumber < PacketNumber)
					{
						lastReadPacketInfoVectorForUdp[i].PacketNumber = PacketNumber;

						return TRUE;
					}
					else
						return FALSE;
				}
			}

			// 만약 새로운 주소에서 받은 데이터일 경우 새로 벡터에 추가해줍니다.
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

		// CNetworkSession의 함수를 이용해서 paketBuffer에 받은 데이터를 입력합니다.
		if (!CNetworkSession::ReadForIOCP(packetBuffer + remainLength, _readLength))
			return FALSE;

		// packetBuffer에 데이터 길이를 증가시켜줍니다.
		remainLength += _readLength;

		return TRUE;
	}

	// ReadPacketForEventSelect는 FALSE가 떨어질때 까지 while문을 돌린다.
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

	// ReadPacketForEventSelect는 FALSE가 떨어질때 까지 while문을 돌린다.
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
		// 패킷의 전체 길이 계산
		// 패킷 전체 길이 = 패킷 길이 + 패킷 고유 번호 + 프로토콜 + 데이터 길이
		DWORD PacketLength =
			sizeof(DWORD) /*LENGTH*/ +
			sizeof(DWORD) /*PACKETNUMBER*/ +
			sizeof(DWORD) /*PROTOCOL*/ +
			_packetLength;

		// 패킷 길이가 최대 버퍼 길이보다 길면 실패합니다.
		if (PacketLength >= MAX_BUFFER_LENGTH)
			return FALSE;

		// 패킬 번호를 하나 증가시킵니다.
		currentPacketNumber++;

		// 들어온 데이터를 이용해서 패킷을 만들기 위한 버퍼
		BYTE TempBuffer[MAX_BUFFER_LENGTH] = { 0, };

		// 패킷을 만듭니다.
		// 패킷 인덱스를 붙여 순서 보정을 해주어야 합니다.
		CopyMemory(TempBuffer, &PacketLength, sizeof(DWORD));

		// 패킷 길이를 써줍니다.
		CopyMemory(TempBuffer
			+ sizeof(DWORD)/*LENGTH*/,
			&currentPacketNumber, sizeof(DWORD));
			
		// 패킷 번호를 써줍니다.
		CopyMemory(TempBuffer +
			sizeof(DWORD)/*LENGTH*/ +
			sizeof(DWORD)/*PACKETNUMBER*/,
			&_protocol, sizeof(DWORD));

		// 프로토콜을 써줍니다.
		CopyMemory(TempBuffer +
			sizeof(DWORD)/*LENGTH*/ +
			sizeof(DWORD)/*PACKETNUMBER*/ +
			sizeof(DWORD)/*PROTOCOL*/,
			_packet, PacketLength);

		// 보내는 데이터를 Encrypt 함수를 이용해서 암호화합니다.
		CCrypt::Encrypt(TempBuffer + sizeof(DWORD), TempBuffer + sizeof(DWORD), PacketLength - sizeof(DWORD));
		
		// writeQueue를 이용해서 패킷이 전송 완료되었을 때까지 메모리를 살려둡니다.
		BYTE *WriteData = writeQueue.Push(this, TempBuffer, PacketLength);

		// Queue의 데이터 포인터를 넣어서 보내줍니다.
		return CNetworkSession::Write(WriteData, PacketLength);
	}

	BOOL CPacketSession::WriteComplete(VOID)
	{
		CThreadSync Sync;

		return writeQueue.Pop();
	}
}