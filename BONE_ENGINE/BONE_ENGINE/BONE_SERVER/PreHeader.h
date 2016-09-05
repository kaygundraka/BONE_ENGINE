#pragma once

#define MAX_USER 100
#define DEFAULT_PORT 1820

#include "..//BONE_NETWORK/BONE_NETWORK/Common.h"

typedef struct _USER {
	WCHAR szUserID[32];
	WCHAR szUserName[32];
	DWORD dwAge;
	BYTE cSex;
	WCHAR szAddress[32];
} USER;

typedef struct _COMPUTER {
	WCHAR szComputerName[32];
	WCHAR szIPAddress[32];
	BYTE cCPUType;
	DWORD dwRam;
	DWORD dwHDD;
} COMPUTER;

typedef struct _PROGRAM {
	WCHAR szProgramName[32];
	DWORD dwCost;
	WCHAR szComment[32];
} PROGRAM;

#include "..//BONE_NETWORK/BONE_NETWORK/CMultiThreadSync.h"
#include "..//BONE_NETWORK/BONE_NETWORK/CMemoryPool.h"
#include "..//BONE_NETWORK/BONE_NETWORK/CCircularQueue.h"
#include "..//BONE_NETWORK/BONE_NETWORK/CNetworkSession.h"
#include "..//BONE_NETWORK/BONE_NETWORK/CPacketSession.h"
#include "..//BONE_NETWORK/BONE_NETWORK/CIOCP.h"
#include "..//BONE_NETWORK/BONE_NETWORK/CStream.h"
#include "..//BONE_NETWORK/BONE_NETWORK/CMiniDump.h"
#include "..//BONE_NETWORK/BONE_NETWORK/CIniFile.h"

#include "CConnectedSession.h"
#include "CConnectedSessionManager.h"
#include "CServerIOCP.h"

#include "Protocol.h"
#include "PT_ReadPacket.h"
#include "PT_Structure.h"
#include "PT_WritePacket.h"

#include <map>