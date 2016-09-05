#pragma once
#include "PreHeader.h"
#include "CConnectedSession.h"
using namespace BONE_NETWORK;

class CConnectedSessionManager : public CMultiThreadSync<CConnectedSessionManager>{
public:
	CConnectedSessionManager(VOID);
	virtual ~CConnectedSessionManager(VOID);

private:
	// ������ ConnectedSession ��ü�� �����͸� �����ϴ� Vector
	std::vector<CConnectedSession*> connectedSessions;

public:
	// �����ϴ� �Լ�
	BOOL Begin(SOCKET _listenSocket);

	// �����ϴ� �Լ�
	VOID End(VOID);

	// ���ӵ� ��� ConnectedSession�� ��Ŷ�� �����ϴ� �Լ�
	VOID WriteAll(DWORD _protocol, BYTE *_data, DWORD _length);
};