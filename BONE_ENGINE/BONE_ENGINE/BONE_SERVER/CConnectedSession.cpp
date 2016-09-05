#include "PreHeader.h"
#include "CConnectedSession.h"

CConnectedSession::CConnectedSession(VOID)
{
	connected = FALSE;
}

CConnectedSession::~CConnectedSession(VOID)
{

}

BOOL CConnectedSession::Restart(SOCKET _listenSocket)
{
	// 동기화 개체
	CThreadSync Sync;

	// 개체를 종료해줍니다.
	End();

	// 개체를 다시 시작하고 Accept 상태로 만들어줍니다.
	// Accept를 하기 위해서는 반드시 hListenSocket 값이 필요합니다.
	// 이 값은 CServerIocp의 멤버인 listen의 GetSocket으로 받아올 수 있습니다.
	return Begin() && Accept(_listenSocket);
}