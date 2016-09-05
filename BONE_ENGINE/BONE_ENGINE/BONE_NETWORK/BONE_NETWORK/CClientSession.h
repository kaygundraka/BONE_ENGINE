#pragma once
#include "Common.h"
#include "CEventSelect.h"
#include "CPacketSession.h"

namespace BONE_NETWORK
{
	class CClientSession : public CEventSelect{
	protected:
		// ����� ����ϴ� Session ��ü �Դϴ�.
		CPacketSession session;

		// ���� ��Ŷ�� �����ϰ� �ִ� ť �Դϴ�.
		CCircularQueue readPacketQueue;

		// ���� UDP�� ��� ������ Ȯ���ϴ� �����Դϴ�.
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