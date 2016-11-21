#pragma once
#include <iostream>
#include <tchar.h>

#include <winsock2.h>
#include <ws2tcpip.h>
#include <mswsock.h>
#include <windows.h>
#include <dbghelp.h>
#include <assert.h>
#include <time.h>

#include <vector>
#include <queue>
#include <list>
#include <map>
#include <string>
#include <utility>

#include <stdlib.h>
#include <stdio.h>

#pragma warning(disable:4996)
#pragma comment(lib,"ws2_32.lib")
#pragma comment(lib, "mswsock.lib")

#define TOTALCOUNT 1

#define MAX_BUFFER_LENGTH	4096
#define MAX_SESSION 10

#define MAX_QUEUE_LENGTH	500

using namespace std;

namespace BONE_NETWORK 
{
	enum IO_TYPE {
		IO_READ, IO_WRITE, IO_ACCPET,
	};

	typedef struct _READ_PACKET_INFO {
		CHAR RemoteAddress[4];
		USHORT RemotePort;
		DWORD PacketNumber;
	} READ_PACKET_INFO;

	typedef struct _OVERLAPPED_EX {
		OVERLAPPED Overlapped;
		IO_TYPE IoType;
		VOID *Object;
	} OVERLAPPED_EX;
}