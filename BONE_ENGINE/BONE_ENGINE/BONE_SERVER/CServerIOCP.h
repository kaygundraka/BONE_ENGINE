#pragma once
#include "PreHeader.h"
using namespace BONE_NETWORK;

DWORD WINAPI KeepThreadCallback(LPVOID parameter);

class CServerIOCP : public CIOCP{
public:
	CServerIOCP(VOID);
	virtual ~CServerIOCP(VOID);

protected:
	// CIOCP 상속 가상 함수들을 재선언합니다.
	virtual VOID OnIoRead(VOID *_object, DWORD _DataLength);
	virtual VOID OnIoWrote(VOID *_object, DWORD _DataLength);
	virtual VOID OnIoConnected(VOID *_object);
	virtual VOID OnIoDisconnected(VOID *_object);

private:
	// 서버의 Listen을 담당할 개체
	CNetworkSession *listen;
	CConnectedSessionManager connectedSessionManager;

	// KeepAlive Thread를 관리하는 핸들
	HANDLE keepThread;

	// KeepAlive Thread를 종료할 때 사용하는 이벤트
	HANDLE keepThreadDestroyEvent;

private:
	// 정보를 관리하는 MAP
	std::map<std::wstring, USER*> users;
	std::map<std::wstring, COMPUTER*> computers;
	std::map<std::wstring, PROGRAM*> programs;

	// 패킷을 처리할 함수 들
	VOID PROC_PT_REG_USER(CConnectedSession *_connectedSession, DWORD _protocol, BYTE *_packet, DWORD _packetLength);
	VOID PROC_PT_QUERY_USER(CConnectedSession *_connectedSession, DWORD _protocol, BYTE *_packet, DWORD _packetLength);

	VOID PROC_PT_REG_COMPUTER(CConnectedSession *_connectedSession, DWORD _protocol, BYTE *_packet, DWORD _packetLength);
	VOID PROC_PT_QUERY_COMPUTER(CConnectedSession *_connectedSession, DWORD _protocol, BYTE *_packet, DWORD _packetLength);
	
	VOID PROC_PT_REG_PROGRAM(CConnectedSession *_connectedSession, DWORD _protocol, BYTE *_packet, DWORD _packetLength);
	VOID PROC_PT_QUERY_PROGRAM(CConnectedSession *_connectedSession, DWORD _protocol, BYTE *_packet, DWORD _packetLength);

public:
	// 개체를 초기화하고 시작하기 위한 함수
	BOOL Begin(VOID);

	// 개체를 종료하기 위한 함수
	VOID End(VOID);

	// KeepAlive Thread Callback 함수
	VOID KeepThreadCallback(VOID);
};