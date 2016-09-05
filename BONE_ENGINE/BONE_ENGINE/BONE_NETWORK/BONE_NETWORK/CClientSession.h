#pragma once
#include "Common.h"
#include "CEventSelect.h"
#include "CPacketSession.h"

namespace BONE_NETWORK
{
	class CClientSession : public CEventSelect{
	protected:
		// 통신을 담당하는 Session 개체 입니다.
		CPacketSession session;

		// 받은 패킷을 저장하고 있는 큐 입니다.
		CCircularQueue readPacketQueue;

		// 현재 UDP로 사용 중인지 확인하는 변수입니다.
		BOOL IsUDP;

	public:
		BOOL BeginTCP(LPSTR _remoteAddress, USHORT _remotePort);
		BOOL BeginUDP(USHORT _remotePort);

		BOOL End(VOID);
		BOOL WritePacket(DWORD _protocol, const BYTE *_packet, DWORD _packetLength);
		BOOL ReadPacket(DWORD &_protocol, BYTE *_packet, DWORD &_packetLength);

		virtual VOID OnIoRead(VOID);
	};
}