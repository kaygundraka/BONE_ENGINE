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
	// ����ȭ ��ü
	CThreadSync Sync;

	// ��ü�� �������ݴϴ�.
	End();

	// ��ü�� �ٽ� �����ϰ� Accept ���·� ������ݴϴ�.
	// Accept�� �ϱ� ���ؼ��� �ݵ�� hListenSocket ���� �ʿ��մϴ�.
	// �� ���� CServerIocp�� ����� listen�� GetSocket���� �޾ƿ� �� �ֽ��ϴ�.
	return Begin() && Accept(_listenSocket);
}