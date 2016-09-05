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
	// ����ȭ ��ü
	CThreadSync Sync;

	// �Ķ���ͷ� Listen ��ü�� ������ �޾ƾ� �մϴ�.
	// ���� NULL �� ���, �����մϴ�.
	if (!_listenSocket)
		return FALSE;

	// �ִ� ������ ��ŭ�� ��ü�� �̸� �����մϴ�.
	for (DWORD i = 0; i < MAX_USER; i++)
	{
		CConnectedSession *ConnectedSession = new CConnectedSession();
		// ������ ��ü�� ��ü�� �����ϴ� connectedSessions ���Ϳ� �Է��մϴ�.
		connectedSessions.push_back(ConnectedSession);
	}

	// ������ ��ü���� �ʱ�ȭ�ϰ� Accept ���·� ������ݴϴ�.
	for (DWORD i = 0; i < MAX_USER; i++)
	{
		// ��ü �ʱ�ȭ �� ������ ��� �����ŵ�ϴ�.
		if (!connectedSessions[i]->Begin())
		{
			CConnectedSessionManager::End();
			return FALSE;
		}

		// ��ü�� Accept ���·� �����ϴ� ������ ������ �����ŵ�ϴ�.
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
	// ����ȭ ��ü
	CThreadSync Sync;

	// �ִ� �����ڸ�ŭ�� ��ü�� �����մϴ�.
	for (DWORD i = 0; i < connectedSessions.size(); i++)
	{
		// ��ü ����
		connectedSessions[i]->End();

		delete connectedSessions[i];
	}

	// �����ϴ� ������ ������ ��� �����ݴϴ�.
	connectedSessions.clear();
}

// ���ӵ� ��� Session�� �����͸� �����ϴ� �Լ�
VOID CConnectedSessionManager::WriteAll(DWORD _protocol, BYTE *_data, DWORD _length)
{
	// ����ȭ ��ü
	CThreadSync Sync;

	// ������ ��� ��ü�� WritePacket�� ���ݴϴ�.
	for (DWORD i = 0; i < connectedSessions.size(); i++)
	{
		// ���ӵǾ� �ִ� ��ü�� ��츸 WritePacket�� ���ݴϴ�.
		if (connectedSessions[i]->GetConnected())
			connectedSessions[i]->WritePacket(_protocol, _data, _length);
	}
}