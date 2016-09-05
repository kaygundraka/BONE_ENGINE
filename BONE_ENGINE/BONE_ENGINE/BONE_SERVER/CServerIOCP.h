#pragma once
#include "PreHeader.h"
using namespace BONE_NETWORK;

DWORD WINAPI KeepThreadCallback(LPVOID parameter);

class CServerIOCP : public CIOCP{
public:
	CServerIOCP(VOID);
	virtual ~CServerIOCP(VOID);

protected:
	// CIOCP ��� ���� �Լ����� �缱���մϴ�.
	virtual VOID OnIoRead(VOID *_object, DWORD _DataLength);
	virtual VOID OnIoWrote(VOID *_object, DWORD _DataLength);
	virtual VOID OnIoConnected(VOID *_object);
	virtual VOID OnIoDisconnected(VOID *_object);

private:
	// ������ Listen�� ����� ��ü
	CNetworkSession *listen;
	CConnectedSessionManager connectedSessionManager;

	// KeepAlive Thread�� �����ϴ� �ڵ�
	HANDLE keepThread;

	// KeepAlive Thread�� ������ �� ����ϴ� �̺�Ʈ
	HANDLE keepThreadDestroyEvent;

private:
	// ������ �����ϴ� MAP
	std::map<std::wstring, USER*> users;
	std::map<std::wstring, COMPUTER*> computers;
	std::map<std::wstring, PROGRAM*> programs;

	// ��Ŷ�� ó���� �Լ� ��
	VOID PROC_PT_REG_USER(CConnectedSession *_connectedSession, DWORD _protocol, BYTE *_packet, DWORD _packetLength);
	VOID PROC_PT_QUERY_USER(CConnectedSession *_connectedSession, DWORD _protocol, BYTE *_packet, DWORD _packetLength);

	VOID PROC_PT_REG_COMPUTER(CConnectedSession *_connectedSession, DWORD _protocol, BYTE *_packet, DWORD _packetLength);
	VOID PROC_PT_QUERY_COMPUTER(CConnectedSession *_connectedSession, DWORD _protocol, BYTE *_packet, DWORD _packetLength);
	
	VOID PROC_PT_REG_PROGRAM(CConnectedSession *_connectedSession, DWORD _protocol, BYTE *_packet, DWORD _packetLength);
	VOID PROC_PT_QUERY_PROGRAM(CConnectedSession *_connectedSession, DWORD _protocol, BYTE *_packet, DWORD _packetLength);

public:
	// ��ü�� �ʱ�ȭ�ϰ� �����ϱ� ���� �Լ�
	BOOL Begin(VOID);

	// ��ü�� �����ϱ� ���� �Լ�
	VOID End(VOID);

	// KeepAlive Thread Callback �Լ�
	VOID KeepThreadCallback(VOID);
};