#pragma once
#include "PreHeader.h"
using namespace BONE_NETWORK;

// Ŭ���̾�Ʈ ����� ����� CTestClientSession ��ü
class CTestClientSession : public CClientSession {
protected:
	// Ŭ���̾�Ʈ�� ���� �����Ǿ��� �� ȣ��Ǵ� �����Լ�
	virtual VOID OnIoConnected(VOID)
	{
		_tprintf(_T("OnIoConnected\n"));
	}

	// Ŭ���̾�Ʈ�� ���� ����Ǿ����� ȣ��Ǵ� �����Լ�
	virtual VOID OnIoDisconnected(VOID)
	{
		_tprintf(_T("OnIoDisconnected\n"));
	}
};