#include "PreHeader.h"
#include "CConnectedSessionManager.h"

CConnectedSessionManager::CConnectedSessionManager(VOID)
{

}

CConnectedSessionManager::~CConnectedSessionManager(VOID)
{

}

BOOL CConnectedSessionManager::Begin(SOCKET _listenSocket)
{
	// 동기화 개체
	CThreadSync Sync;

	// 파라미터로 Listen 개체의 소켓을 받아야 합니다.
	// 값이 NULL 일 경우, 실패합니다.
	if (!_listenSocket)
		return FALSE;

	// 최대 접속자 만큼의 개체를 미리 생성합니다.
	for (DWORD i = 0; i < MAX_USER; i++)
	{
		CConnectedSession *ConnectedSession = new CConnectedSession();
		// 생성된 개체를 개체를 관리하는 connectedSessions 벡터에 입력합니다.
		connectedSessions.push_back(ConnectedSession);
	}

	// 생성한 개체들을 초기화하고 Accept 상태로 만들어줍니다.
	for (DWORD i = 0; i < MAX_USER; i++)
	{
		// 개체 초기화 중 실패할 경우 종료시킵니다.
		if (!connectedSessions[i]->Begin())
		{
			CConnectedSessionManager::End();
			return FALSE;
		}

		// 개체를 Accept 상태로 변경하다 문제가 있으면 종료시킵니다.
		if (!connectedSessions[i]->Accept(_listenSocket))
		{
			CConnectedSessionManager::End();
			return FALSE;
		}
	}

	return TRUE;
}

VOID CConnectedSessionManager::End(VOID)
{
	// 동기화 개체
	CThreadSync Sync;

	// 최대 접속자만큼의 개체를 삭제합니다.
	for (DWORD i = 0; i < connectedSessions.size(); i++)
	{
		// 개체 종료
		connectedSessions[i]->End();

		delete connectedSessions[i];
	}

	// 관리하는 벡터의 내용을 모두 지워줍니다.
	connectedSessions.clear();
}

// 접속된 모든 Session에 데이터를 전송하는 함수
VOID CConnectedSessionManager::WriteAll(DWORD _protocol, BYTE *_data, DWORD _length)
{
	// 동기화 개체
	CThreadSync Sync;

	// 접속한 모든 개체에 WritePacket을 해줍니다.
	for (DWORD i = 0; i < connectedSessions.size(); i++)
	{
		// 접속되어 있는 개체일 경우만 WritePacket을 해줍니다.
		if (connectedSessions[i]->GetConnected())
			connectedSessions[i]->WritePacket(_protocol, _data, _length);
	}
}