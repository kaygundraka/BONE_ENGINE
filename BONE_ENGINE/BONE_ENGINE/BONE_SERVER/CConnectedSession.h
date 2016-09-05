#pragma once
#include "PreHeader.h"
using namespace BONE_NETWORK;

class CConnectedSession : public CPacketSession{
public :
	CConnectedSession(VOID);
	virtual ~CConnectedSession(VOID);

private:
	// ���� ����Ǿ����� �Ǻ��ϴ� ����
	BOOL connected;

public:
	// ��ü�� �����ϰ� �ٽ� Accept ���·� ������ִ� �Լ�
	// CServerIocp���� OnIoDisconnected�� �Ǿ��� �� ��ü�� �����ϰ� �ٽ� �ʱ�ȭ �� �� ����ϰ� �˴ϴ�.
	BOOL Restart(SOCKET _listenSocket);

	// connected�� ���� �����ϰ� �������� ����
	VOID SetConnected(BOOL _connected) { CThreadSync Sync; connected = _connected; }
	BOOL GetConnected(VOID) { CThreadSync Sync; return connected; }
};