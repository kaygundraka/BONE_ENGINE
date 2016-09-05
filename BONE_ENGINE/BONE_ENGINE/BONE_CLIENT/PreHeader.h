#pragma once

#define DEFAULT_PORT 1820

#include "..//BONE_NETWORK/BONE_NETWORK/Common.h"

using namespace BONE_NETWORK;

#include "..//BONE_NETWORK/BONE_NETWORK/CMultiThreadSync.h"
#include "..//BONE_NETWORK/BONE_NETWORK/CMemoryPool.h"
#include "..//BONE_NETWORK/BONE_NETWORK/CCircularQueue.h"
#include "..//BONE_NETWORK/BONE_NETWORK/CNetworkSession.h"
#include "..//BONE_NETWORK/BONE_NETWORK/CPacketSession.h"
#include "..//BONE_NETWORK/BONE_NETWORK/CIOCP.h"
#include "..//BONE_NETWORK/BONE_NETWORK/CStream.h"
#include "..//BONE_NETWORK/BONE_NETWORK/CMiniDump.h"
#include "..//BONE_NETWORK/BONE_NETWORK/CIniFile.h"
#include "..//BONE_NETWORK/BONE_NETWORK/CClientSession.h"

#include "Protocol.h"
#include "PT_ReadPacket.h"
#include "PT_Structure.h"
#include "PT_WritePacket.h"