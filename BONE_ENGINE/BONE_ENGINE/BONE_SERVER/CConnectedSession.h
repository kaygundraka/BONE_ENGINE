#pragma once
#include "PreHeader.h"
using namespace BONE_NETWORK;

class CConnectedSession : public CPacketSession{
public :
	CConnectedSession(VOID);
	virtual ~CConnectedSession(VOID);

private:
	// 접속 종료되었는지 판별하는 변수
	BOOL connected;

public:
	// 개체를 종료하고 다시 Accept 상태로 만들어주는 함수
	// CServerIocp에서 OnIoDisconnected가 되었을 때 개체를 종료하고 다시 초기화 할 때 사용하게 됩니다.
	BOOL Restart(SOCKET _listenSocket);

	// connected의 값을 설정하고 가져오는 변수
	VOID SetConnected(BOOL _connected) { CThreadSync Sync; connected = _connected; }
	BOOL GetConnected(VOID) { CThreadSync Sync; return connected; }
};