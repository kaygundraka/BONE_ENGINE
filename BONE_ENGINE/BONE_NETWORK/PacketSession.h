#pragma once
#include "Common.h"
#include "NetworkSession.h"

namespace BONE_NETWORK
{
	class CPacketSession : public CNetworkSession {
	public:
		CPacketSession(VOID);
		virtual ~CPacketSession(VOID);

	private:
		BYTE packetBuffer[MAX_BUFFER_LENGTH];
		
		INT remainLength;
		INT currentPacketNumber;
		INT lastReadPacketNumber;

		CCircularQueue writeQueue;

		std::vector<READ_PACKET_INFO> lastReadPacketInfoVectorForUdp;

	public:
		BOOL Begin(VOID);
		BOOL End(VOID);

		BOOL GetPacket(DWORD &_protocol, BYTE *_packet, DWORD &_packetLength);
		BOOL GetPacket(LPSTR _remoteAddress, USHORT _remotePort, DWORD &_protocol, BYTE *_packet, DWORD &_packetLength);
		
		BOOL ReadPacketForIocp(DWORD _readLength);
		BOOL ReadPacketForEventSelect(VOID);
		BOOL ReadFromPacketForEventSelect(LPSTR remoteAddress, USHORT &remotePort);
		
		BOOL WritePacket(DWORD _protocol, const BYTE *_packet, DWORD _packetLength);
		BOOL WriteComplete(VOID);
	};
}