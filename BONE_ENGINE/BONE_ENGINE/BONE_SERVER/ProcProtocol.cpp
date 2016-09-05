#include "PreHeader.h"
#include "CServerIOCP.h"

#define READ_PACKET(PROTOCOL)\
	BYTE WriteBuffer[MAX_BUFFER_LENGTH] = {0, };\
	S_##PROTOCOL Data;\
	READ_##PROTOCOL(_packet, Data); 

VOID CServerIOCP::PROC_PT_REG_USER(CConnectedSession *_connectedSession, DWORD _protocol, BYTE *_packet, DWORD _packetLength)
{
	// ��ó�� �Լ��� ����ȭ
	READ_PACKET(PT_REG_USER);
	// ���� �ڵ�
	// BYTE WriteBuffer[MAX_BUFFER_LENGTH] = {0,};
	// S_PT_REG_USER Data;
	// READ_PT_REG_USER(_packet, Data);

	// USER ��ü�� 1�� �����մϴ�.
	USER *User = new USER();

	// �������ݿ��� �����͸� ������ ��ü�� �ű�ϴ�.
	_tcscpy(User->szUserID, Data.USER_ID);
	_tcscpy(User->szUserName, Data.USER_NAME);
	_tcscpy(User->szAddress, Data.ADDRESS);
	User->dwAge = Data.AGE;
	User->cSex = Data.SEX;

	// USER ��ü �� �����ϴ� MAP Users�� ������ �Է�
	users.insert(std::make_pair(Data.USER_ID, User));
}

VOID CServerIOCP::PROC_PT_QUERY_USER(CConnectedSession *_connectedSession, DWORD _protocol, BYTE *_packet, DWORD _packetLength)
{
	// ��ó�� �Լ��� ����ȭ
	READ_PACKET(PT_QUERY_USER);

	// USER_ID�� �̿��ؼ� �����͸� �˻��մϴ�.
	auto Iter = users.find(Data.USER_ID);

	// �˻� ����� ���� ���
	if (Iter != users.end())
	{
		_connectedSession->WritePacket(
			PT_QUERY_USER_RESULT,
			WriteBuffer,
			WRITE_PT_QUERY_USER_RESULT(WriteBuffer,
				Iter->second->szUserID,
				Iter->second->szUserName,
				Iter->second->dwAge,
				Iter->second->cSex,
				Iter->second->szAddress)
		);
	}
	else
	{
		// �˻� ����� ���� ��� �� ���� �����ݴϴ�.
		USER User;
		ZeroMemory(&User, sizeof(USER));

		// ��� ����ִ� �����͸� �����ݴϴ�.
		_connectedSession->WritePacket(
			PT_QUERY_USER_RESULT,
			WriteBuffer,
			WRITE_PT_QUERY_USER_RESULT(WriteBuffer,
				User.szUserID,
				User.szUserName,
				User.dwAge,
				User.cSex,
				User.szAddress)
		);
	}
}

VOID CServerIOCP::PROC_PT_REG_COMPUTER(CConnectedSession *_connectedSession, DWORD _protocol, BYTE *_packet, DWORD _packetLength)
{
	// ��ó�� �Լ��� ����ȭ
	READ_PACKET(PT_REG_COMPUTER);

	// COMPUTER ��ü�� �� �� �����մϴ�.
	COMPUTER *Computer = new COMPUTER();

	// �������ݿ��� �����͸� ������ ��ü�� �ű�ϴ�.
	// COMPUTER_NAME ����
	_tcscpy(Computer->szComputerName, Data.COMPUTER_NAME);
	_tcscpy(Computer->szIPAddress, Data.IP_ADDRESS);
	Computer->cCPUType = Data.CPU_TYPE;
	Computer->dwRam = Data.RAM;
	Computer->dwHDD = Data.HDD;

	computers.insert(std::make_pair(Data.COMPUTER_NAME, Computer));
}

VOID CServerIOCP::PROC_PT_QUERY_COMPUTER(CConnectedSession *_connectedSession, DWORD _protocol, BYTE *_packet, DWORD _packetLength)
{
	// ��ó�� �Լ��� ����ȭ
	READ_PACKET(PT_QUERY_COMPUTER);

	// COMPUTER_NAME�� �̿��ؼ� �����͸� �˻��մϴ�.
	auto Iter = computers.find(Data.COMPUTER_NAME);

	// �˻� ����� ���� ���
	if (Iter != computers.end())
	{
		_connectedSession->WritePacket(
			PT_QUERY_COMPUTER_RESULT,
			WriteBuffer,
			WRITE_PT_QUERY_COMPUTER_RESULT(
				WriteBuffer,
				Iter->second->szComputerName,
				Iter->second->szIPAddress,
				Iter->second->cCPUType,
				Iter->second->dwRam,
				Iter->second->dwHDD
			)
		);
	}
	else
	{
		// ��� ��� �ִ� �����͸� �����ݴϴ�.
		COMPUTER Computer;
		ZeroMemory(&Computer, sizeof(COMPUTER));

		_connectedSession->WritePacket(
			PT_QUERY_COMPUTER_RESULT,
			WriteBuffer,
			WRITE_PT_QUERY_COMPUTER_RESULT(
				WriteBuffer,
				Computer.szComputerName,
				Computer.szIPAddress,
				Computer.cCPUType,
				Computer.dwRam,
				Computer.dwHDD
			)
		);
	}
}

VOID CServerIOCP::PROC_PT_REG_PROGRAM(CConnectedSession *_connectedSession, DWORD _protocol, BYTE *_packet, DWORD _packetLength)
{
	// ��ó�� �Լ��� ����ȭ
	READ_PACKET(PT_REG_PROGRAM);

	// PROGAM ��ü�� 1�� �����մϴ�.
	PROGRAM *Program = new PROGRAM;

	// �������ݿ��� �����͸� ������ ��ü�� �ű�ϴ�.
	// PROGRAM_NAME ����
	_tcscpy(Program->szProgramName, Data.PROGRAM_NAME);
	_tcscpy(Program->szComment, Data.COMMENT);
	Program->dwCost = Data.COST;

	programs.insert(std::make_pair(Data.PROGRAM_NAME, Program));
}

VOID CServerIOCP::PROC_PT_QUERY_PROGRAM(CConnectedSession *_connectedSession, DWORD _protocol, BYTE *_packet, DWORD _packetLength)
{
	// ��ó�� �Լ��� ����ȭ
	READ_PACKET(PT_QUERY_PROGRAM);

	auto Iter = programs.find(Data.PROGRAM_NAME);

	if (Iter != programs.end())
	{
		_connectedSession->WritePacket(
			PT_QUERY_PROGRAM_RESULT,
			WriteBuffer,
			WRITE_PT_QUERY_PROGRAM_RESULT(
				WriteBuffer,
				Iter->second->szProgramName,
				Iter->second->dwCost,
				Iter->second->szComment
			)
		);
	}
	else
	{
		PROGRAM Program;
		ZeroMemory(&Program, sizeof(PROGRAM));

		_connectedSession->WritePacket(
			PT_QUERY_PROGRAM_RESULT,
			WriteBuffer,
			WRITE_PT_QUERY_PROGRAM_RESULT(
				WriteBuffer,
				Program.szProgramName,
				Program.dwCost,
				Program.szComment
			)
		);
	}
}