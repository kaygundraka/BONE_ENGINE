#pragma once
#include "PreHeader.h"
using namespace BONE_NETWORK;

// 클라이언트 통신을 담당할 CTestClientSession 개체
class CTestClientSession : public CClientSession {
protected:
	// 클라이언트가 접속 성공되었을 때 호출되는 가상함수
	virtual VOID OnIoConnected(VOID)
	{
		_tprintf(_T("OnIoConnected\n"));
	}

	// 클라이언트가 접속 종료되었을때 호출되는 가상함수
	virtual VOID OnIoDisconnected(VOID)
	{
		_tprintf(_T("OnIoDisconnected\n"));
	}
};