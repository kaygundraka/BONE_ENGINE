#pragma once
#include "PreHeader.h"
#include "CConnectedSession.h"
using namespace BONE_NETWORK;

class CConnectedSessionManager : public CMultiThreadSync<CConnectedSessionManager>{
public:
	CConnectedSessionManager(VOID);
	virtual ~CConnectedSessionManager(VOID);

private:
	// 생성된 ConnectedSession 개체의 포인터를 관리하는 Vector
	std::vector<CConnectedSession*> connectedSessions;

public:
	// 시작하는 함수
	BOOL Begin(SOCKET _listenSocket);

	// 종료하는 함수
	VOID End(VOID);

	// 접속된 모든 ConnectedSession에 패킷을 전송하는 함수
	VOID WriteAll(DWORD _protocol, BYTE *_data, DWORD _length);
};