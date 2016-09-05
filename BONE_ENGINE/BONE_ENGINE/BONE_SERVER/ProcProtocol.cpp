#include "PreHeader.h"
#include "CServerIOCP.h"

#define READ_PACKET(PROTOCOL)\
	BYTE WriteBuffer[MAX_BUFFER_LENGTH] = {0, };\
	S_##PROTOCOL Data;\
	READ_##PROTOCOL(_packet, Data); 

VOID CServerIOCP::PROC_PT_REG_USER(CConnectedSession *_connectedSession, DWORD _protocol, BYTE *_packet, DWORD _packetLength)
{
	// 전처리 함수로 간략화
	READ_PACKET(PT_REG_USER);
	// 실제 코드
	// BYTE WriteBuffer[MAX_BUFFER_LENGTH] = {0,};
	// S_PT_REG_USER Data;
	// READ_PT_REG_USER(_packet, Data);

	// USER 객체를 1개 생성합니다.
	USER *User = new USER();

	// 프로토콜에서 데이터를 생성한 개체로 옮깁니다.
	_tcscpy(User->szUserID, Data.USER_ID);
	_tcscpy(User->szUserName, Data.USER_NAME);
	_tcscpy(User->szAddress, Data.ADDRESS);
	User->dwAge = Data.AGE;
	User->cSex = Data.SEX;

	// USER 객체 를 관리하는 MAP Users에 데이터 입력
	users.insert(std::make_pair(Data.USER_ID, User));
}

VOID CServerIOCP::PROC_PT_QUERY_USER(CConnectedSession *_connectedSession, DWORD _protocol, BYTE *_packet, DWORD _packetLength)
{
	// 전처리 함수로 간략화
	READ_PACKET(PT_QUERY_USER);

	// USER_ID를 이용해서 데이터를 검색합니다.
	auto Iter = users.find(Data.USER_ID);

	// 검색 결과가 있을 경우
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
		// 검색 결과가 없을 경우 빈 값을 보내줍니다.
		USER User;
		ZeroMemory(&User, sizeof(USER));

		// 모두 비어있는 데이터를 보내줍니다.
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
	// 전처리 함수로 간략화
	READ_PACKET(PT_REG_COMPUTER);

	// COMPUTER 객체를 한 개 생성합니다.
	COMPUTER *Computer = new COMPUTER();

	// 프로토콜에서 데이터를 생성한 개체로 옮깁니다.
	// COMPUTER_NAME 복사
	_tcscpy(Computer->szComputerName, Data.COMPUTER_NAME);
	_tcscpy(Computer->szIPAddress, Data.IP_ADDRESS);
	Computer->cCPUType = Data.CPU_TYPE;
	Computer->dwRam = Data.RAM;
	Computer->dwHDD = Data.HDD;

	computers.insert(std::make_pair(Data.COMPUTER_NAME, Computer));
}

VOID CServerIOCP::PROC_PT_QUERY_COMPUTER(CConnectedSession *_connectedSession, DWORD _protocol, BYTE *_packet, DWORD _packetLength)
{
	// 전처리 함수로 간략화
	READ_PACKET(PT_QUERY_COMPUTER);

	// COMPUTER_NAME을 이용해서 데이터를 검색합니다.
	auto Iter = computers.find(Data.COMPUTER_NAME);

	// 검색 결과가 있을 경우
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
		// 모두 비어 있는 데이터를 보내줍니다.
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
	// 전처리 함수로 간략화
	READ_PACKET(PT_REG_PROGRAM);

	// PROGAM 객체를 1개 생성합니다.
	PROGRAM *Program = new PROGRAM;

	// 프로토콜에서 데이터를 생성한 개체로 옮깁니다.
	// PROGRAM_NAME 복사
	_tcscpy(Program->szProgramName, Data.PROGRAM_NAME);
	_tcscpy(Program->szComment, Data.COMMENT);
	Program->dwCost = Data.COST;

	programs.insert(std::make_pair(Data.PROGRAM_NAME, Program));
}

VOID CServerIOCP::PROC_PT_QUERY_PROGRAM(CConnectedSession *_connectedSession, DWORD _protocol, BYTE *_packet, DWORD _packetLength)
{
	// 전처리 함수로 간략화
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