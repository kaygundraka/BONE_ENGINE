#include "Common.h"
#include "NetworkSession.h"

namespace BONE_NETWORK
{
	DWORD WINAPI ReliableUdpThreadCallback(LPVOID parameter)
	{
		CNetworkSession *Owner = (CNetworkSession*)parameter;
		Owner->ReliableUdpThreadCallback();

		return 0;
	}

	CNetworkSession::CNetworkSession()
	{
		ZeroMemory(&acceptOverlapped, sizeof(acceptOverlapped));
		ZeroMemory(&readOverlapped, sizeof(readOverlapped));
		ZeroMemory(&writeOverlapped, sizeof(writeOverlapped));

		ZeroMemory(readBuffer, sizeof(readBuffer));
		ZeroMemory(&UdpRemoteInfo, sizeof(UdpRemoteInfo));

		sock = NULL;

		reliableUdpThreadHandle = NULL;
		reliableUdpThreadStartupEvent = NULL;
		reliableUdpThreadDestroyEvent = NULL;
		reliableUdpThreadWakeUpEvent = NULL;
		reliableUdpWriteCompleteEvent = NULL;

		IsReliableUdpSending = FALSE;

		acceptOverlapped.IoType = IO_ACCPET;
		readOverlapped.IoType = IO_READ;
		writeOverlapped.IoType = IO_WRITE;

		acceptOverlapped.Object = this;
		readOverlapped.Object = this;
		writeOverlapped.Object = this;
	}

	CNetworkSession::~CNetworkSession()
	{

	}

	BOOL CNetworkSession::Begin(VOID)
	{
		CThreadSync Sync;

		if (sock)
			return FALSE;

		ZeroMemory(readBuffer, sizeof(readBuffer));
		ZeroMemory(&UdpRemoteInfo, sizeof(UdpRemoteInfo));

		sock = NULL;

		reliableUdpThreadHandle = NULL;
		reliableUdpThreadStartupEvent = NULL;
		reliableUdpThreadDestroyEvent = NULL;
		reliableUdpThreadWakeUpEvent = NULL;
		reliableUdpWriteCompleteEvent = NULL;

		IsReliableUdpSending = FALSE;

		return TRUE;
	}

	BOOL CNetworkSession::TCPBind(VOID)
	{
		CThreadSync Sync;

		if (sock)
			return FALSE;

		sock = WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, NULL, 0, WSA_FLAG_OVERLAPPED);
		
		if (sock == INVALID_SOCKET)
			return FALSE;

		BOOL NoDelay = TRUE;
		setsockopt(sock, IPPROTO_TCP, TCP_NODELAY, (const char FAR *)&NoDelay, sizeof(NoDelay));

		return TRUE;
	}

	BOOL CNetworkSession::UDPBind(USHORT _port)
	{
		CThreadSync Sync;

		if (sock)
			return FALSE;

		SOCKADDR_IN RemoteAddressInfo;


		RemoteAddressInfo.sin_family = AF_INET;
		RemoteAddressInfo.sin_port = htons(_port);
		RemoteAddressInfo.sin_addr.S_un.S_addr = htonl(INADDR_ANY);

		sock = WSASocket(AF_INET, SOCK_DGRAM, IPPROTO_UDP, NULL, 0, WSA_FLAG_OVERLAPPED);

		if (sock == INVALID_SOCKET)
			return FALSE;

		if (bind(sock, (sockaddr*)&RemoteAddressInfo, sizeof(SOCKADDR_IN)) == SOCKET_ERROR)
		{
			End();

			return FALSE;
		}

		reliableUdpThreadDestroyEvent = CreateEvent(0, FALSE, FALSE, 0);
		if (reliableUdpThreadDestroyEvent == NULL)
		{
			End();

			return FALSE;
		}

		reliableUdpThreadStartupEvent = CreateEvent(0, FALSE, FALSE, 0);
		if (reliableUdpThreadStartupEvent == NULL)
		{
			End();

			return FALSE;
		}

		reliableUdpThreadWakeUpEvent = CreateEvent(0, FALSE, FALSE, 0);
		if (reliableUdpThreadWakeUpEvent == NULL)
		{
			End();

			return FALSE;
		}

		reliableUdpWriteCompleteEvent = CreateEvent(0, FALSE, FALSE, 0);
		if (reliableUdpWriteCompleteEvent == NULL)
		{
			End();

			return FALSE;
		}

		if (reliableWriteQueue.Begin())
		{
			End();
			
			return FALSE;
		}

		DWORD ReliableUdpThreadID = 0;
		reliableUdpThreadHandle = CreateThread(NULL, 0, BONE_NETWORK::ReliableUdpThreadCallback, this, 0, &ReliableUdpThreadID);

		WaitForSingleObject(reliableUdpThreadStartupEvent, INFINITE);

		return TRUE;
	}

	VOID CNetworkSession::ReliableUdpThreadCallback(VOID)
	{
		DWORD EventID = 0;

		HANDLE ThreadEvents[2] = { 
			reliableUdpThreadDestroyEvent, 
			reliableUdpThreadWakeUpEvent 
		};

		CHAR RemoteAddress[32] = { 0, };
		USHORT RemotePort = 0;
		BYTE Data[MAX_BUFFER_LENGTH] = { 0, };
		DWORD DataLength = 0;
		VOID *Object = NULL;

		while(TRUE)
		{
			SetEvent(reliableUdpThreadStartupEvent);

			EventID = WaitForMultipleObjects(2, ThreadEvents, FALSE, INFINITE);
			switch (EventID)
			{
			case WAIT_OBJECT_0:
				return;

			case WAIT_OBJECT_0 + 1:
NEXT_DATA:
				if (reliableWriteQueue.Pop(&Object, Data, DataLength, RemoteAddress, RemotePort))
				{
RETRY:
					if (!WriteTo2(RemoteAddress, RemotePort, Data, DataLength))
						return;

					DWORD Result = WaitForSingleObject(reliableUdpWriteCompleteEvent, 10);

					if (Result == WAIT_OBJECT_0)
						goto NEXT_DATA;
					else
						goto RETRY;
				}
				else
				{
					IsReliableUdpSending = FALSE;
				}

				break;
			}
		}
	}

	BOOL CNetworkSession::WriteTo(LPCSTR _remoteAddress, USHORT _remotePort, BYTE *_data, DWORD _dataLength)
	{
		CThreadSync Sync;

		if (!sock)
			return FALSE;

		if (!_remoteAddress || _remotePort <= 0 || !_data || _dataLength <= 0)
			return FALSE;

		if (!reliableWriteQueue.Push(this, _data, _dataLength, _remoteAddress, _remotePort))
			return FALSE;

		if (!IsReliableUdpSending)
		{
			IsReliableUdpSending = TRUE;
			SetEvent(reliableUdpThreadWakeUpEvent);
		}

		return TRUE;
	}
	
	BOOL CNetworkSession::WriteTo2(LPCSTR _remoteAddress, USHORT _remotePort, BYTE *_data, DWORD _dataLength)
	{
		CThreadSync Sync;

		if (!sock)
			return FALSE;

		if (!_remoteAddress || _remotePort <= 0 || !_data || _dataLength <= 0)
			return FALSE;

		WSABUF WsaBuf;
		DWORD WriteBytes = 0;
		DWORD WriteFlag = 0;

		SOCKADDR_IN RemoteAddressInfo;
		INT RemoteAddressInfoSize = sizeof(RemoteAddressInfo);

		WsaBuf.buf = (CHAR*)_data;
		WsaBuf.len = _dataLength;

		RemoteAddressInfo.sin_family = AF_INET;
		RemoteAddressInfo.sin_port = htons(_remotePort);
		RemoteAddressInfo.sin_addr.S_un.S_addr = inet_addr(_remoteAddress);

		INT ReturnValue = WSASendTo(sock,
			&WsaBuf,
			1,
			&WriteBytes,
			WriteFlag,
			(sockaddr*)&RemoteAddressInfo,
			RemoteAddressInfoSize,
			&writeOverlapped.Overlapped,
			NULL);

		if (ReturnValue == SOCKET_ERROR && WSAGetLastError() != WSA_IO_PENDING
			&& WSAGetLastError() != WSAEWOULDBLOCK)
		{
			End();

			return FALSE;
		}

		return TRUE;

		if (!reliableWriteQueue.Push(this, _data, _dataLength, _remoteAddress, _remotePort))
			return FALSE;

		if (!IsReliableUdpSending)
		{
			IsReliableUdpSending = TRUE;
			SetEvent(reliableUdpThreadWakeUpEvent);
		}

		return TRUE;
	}

	BOOL CNetworkSession::Listen(USHORT _port, INT _backLog)
	{
		CThreadSync Sync;

		if (_port <= 0 || _backLog <= 0)
			return FALSE;

		if (!sock)
			return FALSE;

		SOCKADDR_IN ListenSocketInfo;
		ListenSocketInfo.sin_family = AF_INET;
		ListenSocketInfo.sin_port = htons(_port);
		ListenSocketInfo.sin_addr.S_un.S_addr = htonl(INADDR_ANY);

		if (bind(sock, (struct sockaddr*) &ListenSocketInfo, sizeof(SOCKADDR_IN)) == SOCKET_ERROR)
		{
			End();

			return FALSE;
		}

		if (listen(sock, _backLog) == SOCKET_ERROR)
		{
			End();

			return FALSE;
		}

		LINGER Linger;
		Linger.l_onoff = 1;
		Linger.l_linger = 0;

		if (setsockopt(sock, SOL_SOCKET, SO_LINGER, (char*)&Linger, sizeof(LINGER)) == SOCKET_ERROR)
		{
			End();

			return FALSE;
		}

		return TRUE;
	}

	BOOL CNetworkSession::Connect(LPSTR _address, USHORT _port)
	{
		CThreadSync Sync;

		if (!_address || !_port)
			return FALSE;

		if (!sock)
			return FALSE;

		SOCKADDR_IN RemoteAddressInfo;

		RemoteAddressInfo.sin_family = AF_INET;
		RemoteAddressInfo.sin_port = htons(_port);
		RemoteAddressInfo.sin_addr.S_un.S_addr = inet_addr(_address);

		if (WSAConnect(sock, (LPSOCKADDR)&RemoteAddressInfo, sizeof(SOCKADDR_IN), NULL, NULL, NULL, NULL) == SOCKET_ERROR)
		{
			if (WSAGetLastError() != WSAEWOULDBLOCK)
			{
				End();

				return FALSE;
			}
		}

		return TRUE;
	}

	BOOL CNetworkSession::Accept(SOCKET _listenSocket)
	{
		CThreadSync Sync;

		if (!_listenSocket)
			return FALSE;

		if (sock)
			return FALSE;

		sock = WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, NULL, 0, WSA_FLAG_OVERLAPPED);

		if (sock == INVALID_SOCKET)
		{
			End();

			return FALSE;
		}

		BOOL NoDelay = TRUE;
		setsockopt(sock, IPPROTO_TCP, TCP_NODELAY, (const char FAR *)&NoDelay, sizeof(NoDelay));

		if (!AcceptEx(_listenSocket,
			sock,
			readBuffer,
			0,
			sizeof(sockaddr_in) + 16,
			sizeof(sockaddr_in) + 16,
			NULL,
			&acceptOverlapped.Overlapped))
		{
			if (WSAGetLastError() != ERROR_IO_PENDING &&
				WSAGetLastError() != WSAEWOULDBLOCK)
			{
				End();

				return FALSE;
			}
		}

		return TRUE;
	}

	BOOL CNetworkSession::InitializeReadForIOCP(VOID)
	{
		CThreadSync Sync;

		if (!sock)
			return FALSE;

		WSABUF WsaBuf;
		DWORD ReadByte = 0;
		DWORD ReadFlag = 0;

		WsaBuf.buf = (CHAR*)readBuffer;
		WsaBuf.len = MAX_BUFFER_LENGTH;
		
		INT ReturnValue = WSARecv(
			sock, &WsaBuf,
			1, 
			&ReadByte, 
			&ReadFlag, 
			&readOverlapped.Overlapped,
			NULL);

		if (ReturnValue == SOCKET_ERROR &&
			WSAGetLastError() != WSA_IO_PENDING &&
			WSAGetLastError() != WSAEWOULDBLOCK)
		{
			End();

			return FALSE;
		}

		return TRUE;
	}

	BOOL CNetworkSession::InitializeReadFromForIOCP(VOID)
	{
		CThreadSync Sync;

		if (!sock)
			return FALSE;

		WSABUF WsaBuf;
		DWORD ReadByte = 0;
		DWORD ReadFlag = 0;
		INT RemoteAddressInfoSize = sizeof(UdpRemoteInfo);

		WsaBuf.buf = (CHAR*)readBuffer;
		WsaBuf.len = MAX_BUFFER_LENGTH;

		INT ReturnValue = WSARecvFrom(
			sock, 
			&WsaBuf,
			1,
			&ReadByte,
			&ReadFlag,
			(sockaddr*)&UdpRemoteInfo,
			&RemoteAddressInfoSize,
			&readOverlapped.Overlapped,
			NULL);

		if (ReturnValue == SOCKET_ERROR &&
			WSAGetLastError() != WSA_IO_PENDING &&
			WSAGetLastError() != WSAEWOULDBLOCK)
		{
			End();

			return FALSE;
		}

		return TRUE;
	}

	BOOL CNetworkSession::ReadForIOCP(BYTE *_data, DWORD &_dataLength)
	{
		CThreadSync Sync;

		if (!sock)
			return FALSE;

		if (!_data || _dataLength <= 0)
			return FALSE;

		CopyMemory(_data, readBuffer, _dataLength);

		return TRUE;
	}

	BOOL CNetworkSession::ReadFromForIOCP(LPSTR _remoteAddress, USHORT &_remotePort, BYTE *_data, DWORD &_dataLength)
	{
		CThreadSync Sync;

		if (!sock)
			return FALSE;

		if (!_data || _dataLength <= 0)
			return FALSE;

		CopyMemory(_data, readBuffer, _dataLength);

		strcpy(_remoteAddress, inet_ntoa(UdpRemoteInfo.sin_addr));
		_remotePort = ntohs(UdpRemoteInfo.sin_port);

		USHORT Ack = 0;
		CopyMemory(&Ack, readBuffer, sizeof(USHORT));

		if (Ack == 9999)
		{
			SetEvent(reliableUdpWriteCompleteEvent);

			return FALSE;
		}
		else
		{
			Ack = 9999;
			WriteTo2(_remoteAddress, _remotePort, (BYTE*)&Ack, sizeof(USHORT));
		}

		return TRUE;
	}
	
	BOOL CNetworkSession::ReadForEventSelect(BYTE *_data, DWORD &_dataLength)
	{
		CThreadSync Sync;

		if (!sock)
			return FALSE;

		if (!_data)
			return FALSE;

		WSABUF WsaBuf;
		DWORD ReadBytes = 0;
		DWORD ReadFlag = 0;

		WsaBuf.buf = (CHAR*)readBuffer;
		WsaBuf.len = MAX_BUFFER_LENGTH;

		INT ReturnValue = WSARecv(sock,
			&WsaBuf,
			1,
			&ReadBytes,
			&ReadFlag,
			&readOverlapped.Overlapped,
			NULL);

		if (ReturnValue == SOCKET_ERROR && WSAGetLastError() != WSA_IO_PENDING
			&& WSAGetLastError() != WSAEWOULDBLOCK)
		{
			End();

			return FALSE;
		}

		CopyMemory(_data, readBuffer, ReadBytes);

		_dataLength = ReadBytes;

		return TRUE;
	}

	BOOL CNetworkSession::ReadFromForEventSelect(LPSTR _remoteAddress, USHORT &_remotePort, BYTE *_data, DWORD &_dataLength)
	{
		CThreadSync Sync;

		if (!sock)
			return FALSE;

		if (!_data)
			return FALSE;

		WSABUF WsaBuf;
		DWORD ReadBytes = 0;
		DWORD ReadFlag = 0;
		INT RemoteAddressInfo = sizeof(UdpRemoteInfo);

		WsaBuf.buf = (CHAR*)readBuffer;
		WsaBuf.len = MAX_BUFFER_LENGTH;

		INT ReturnValue = WSARecvFrom(sock,
			&WsaBuf,
			1,
			&ReadBytes,
			&ReadFlag,
			(SOCKADDR*)&UdpRemoteInfo,
			&RemoteAddressInfo,
			&readOverlapped.Overlapped,
			NULL);

		if (ReturnValue == SOCKET_ERROR && WSAGetLastError() == WSA_IO_PENDING && WSAGetLastError() == WSAEWOULDBLOCK)
		{
			End();

			return FALSE;
		}

		CopyMemory(_data, readBuffer, ReadBytes);
		_dataLength = ReadBytes;

		strcpy(_remoteAddress, inet_ntoa(UdpRemoteInfo.sin_addr));
		_remotePort = ntohs(UdpRemoteInfo.sin_port);

		USHORT Ack = 0;
		CopyMemory(&Ack, readBuffer, sizeof(USHORT));

		if (Ack == 9999)
		{
			SetEvent(reliableUdpWriteCompleteEvent);

			return FALSE;
		}
		else
		{
			Ack = 9999;
			WriteTo2(_remoteAddress, _remotePort, (BYTE*)&Ack, sizeof(USHORT));
		}

		return TRUE;
	}

	BOOL CNetworkSession::Write(BYTE *_data, DWORD dataLength)
	{
		CThreadSync Sync;

		if (!sock)
			return FALSE;

		if (!_data || dataLength <= 0)
			return FALSE;

		WSABUF WsaBuf;
		DWORD WriteBytes = 0;
		DWORD WriteFlag = 0;
		WsaBuf.buf = (CHAR*)_data;
		WsaBuf.len = dataLength;

		INT ReturnValue = WSASend(sock,
			&WsaBuf,
			1,
			&WriteBytes,
			WriteFlag,
			&writeOverlapped.Overlapped,
			NULL);

		if (ReturnValue == SOCKET_ERROR && WSAGetLastError() != WSA_IO_PENDING
			&& WSAGetLastError() != WSAEWOULDBLOCK)
		{
			End();

			return FALSE;
		}

		return TRUE;
	}

	SOCKET CNetworkSession::GetSocket(VOID)
	{
		CThreadSync Sync;

		return sock;
	}

	BOOL CNetworkSession::End(VOID)
	{
		CThreadSync Sync;

		if (sock)
			return FALSE;

		shutdown(sock, SD_BOTH);
		closesocket(sock);

		sock = NULL;

		if (reliableUdpThreadHandle)
		{
			SetEvent(reliableUdpThreadDestroyEvent);

			WaitForSingleObject(reliableUdpThreadHandle, INFINITE);
			CloseHandle(reliableUdpThreadHandle);
		}

		if (reliableUdpThreadStartupEvent)
			CloseHandle(reliableUdpThreadStartupEvent);

		if (reliableUdpThreadDestroyEvent)
			CloseHandle(reliableUdpThreadDestroyEvent);

		if (reliableUdpThreadWakeUpEvent)
			CloseHandle(reliableUdpThreadWakeUpEvent);

		if (reliableUdpWriteCompleteEvent)
			CloseHandle(reliableUdpWriteCompleteEvent);

		return TRUE;
	}
}