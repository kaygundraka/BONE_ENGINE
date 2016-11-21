#pragma once
#include "Common.h"
#include <MultiThreadSync.h>
#include "CircularQueue.h"

namespace BONE_NETWORK
{
	DWORD WINAPI ReliableUdpThreadCallback(LPVOID parameter);

	class CNetworkSession : public BONE_SYSTEM::CMultiThreadSync<CNetworkSession>
	{
	private:
		OVERLAPPED_EX acceptOverlapped;
		OVERLAPPED_EX readOverlapped;
		OVERLAPPED_EX writeOverlapped;

		BYTE readBuffer[MAX_BUFFER_LENGTH];
		SOCKADDR_IN UdpRemoteInfo;

		SOCKET sock;

		HANDLE reliableUdpThreadHandle;
		HANDLE reliableUdpThreadStartupEvent;
		HANDLE reliableUdpThreadDestroyEvent;
		HANDLE reliableUdpThreadWakeUpEvent;
		HANDLE reliableUdpWriteCompleteEvent;

		BOOL IsReliableUdpSending;

		CCircularQueue reliableWriteQueue;

	public:
		CNetworkSession();
		virtual ~CNetworkSession();
		
		BOOL Begin(VOID);
	
		BOOL TCPBind(VOID);
		BOOL UDPBind(USHORT _port);
		BOOL Listen(USHORT _port, INT _backLog);
		BOOL Connect(LPSTR _address, USHORT _port);
		BOOL Accept(SOCKET _listenSocket);

		VOID ReliableUdpThreadCallback(VOID);

		BOOL InitializeReadForIOCP(VOID);
		BOOL InitializeReadFromForIOCP(VOID);

		BOOL ReadForIOCP(BYTE *_data, DWORD &_dataLength);
		BOOL ReadFromForIOCP(LPSTR _remoteAddress, USHORT &_remotePort, BYTE *_data, DWORD &_dataLength);
		BOOL ReadForEventSelect(BYTE *_data, DWORD &_dataLength);
		BOOL ReadFromForEventSelect(LPSTR _remoteAddress, USHORT &_remotePort, BYTE *_data, DWORD &_dataLength);
		
		BOOL Write(BYTE *_data, DWORD dataLength);
		BOOL WriteTo(LPCSTR _remoteAddress, USHORT _remotePort, BYTE *_data, DWORD _dataLength);
		BOOL WriteTo2(LPCSTR _remoteAddress, USHORT _remotePort, BYTE *_data, DWORD _dataLength);

		SOCKET GetSocket(VOID);

		BOOL End(VOID);

	};
}