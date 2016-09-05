#include "PreHeader.h"
#include "CTestClientSession.h"

#define READ_PACKET(PROTOCOL)\
	BYTE WriteBuffer[MAX_BUFFER_LENGTH] = {0, };\
	S_##PROTOCOL Data;\
	READ_##PROTOCOL(Packet, Data); 

int _tmain(int argc, _TCHAR* argv[])
{
	// Winsock을 사용하기 위한 DLL 로드
	WSADATA WsaData;
	WSAStartup(MAKEWORD(2, 2), &WsaData);

	// 통신 개체를 생성합니다.
	CTestClientSession *ClientSession = new CTestClientSession();

	// TCP로 사용할 것을 선언하고 저속할 IP, PORT를 설정합니다.
	// 성공했을 경우 getchar에 의해 잠시 멈춤 상태가 됩니다.
	if (ClientSession->BeginTCP("127.0.0.1", DEFAULT_PORT))
	{
		while (TRUE)
		{
			// 패킷을 보낼 때 사용하는 버퍼
			BYTE WriteBuffer[MAX_BUFFER_LENGTH] = { 0, };
			// 패킷을 받을 때 사용하는 프로토콜, 길이, 패킷 변수
			DWORD Protocol = 0, PacketLength = 0;
			BYTE Packet[MAX_BUFFER_LENGTH] = { 0, };

			// 화면을 한 번 지워줍니다.
			system("cls");

			// 키보드로 입력한 내용을 저장하는 변수
			// 앞으로 모든 키보드로 입력한 내용이 이곳에 저장됩니다.
			TCHAR szCommand[32] = { 0, };
			// USER, COMPUTER, PROGRAM의 선택을 받습니다.
			_tprintf(_T("Enter command (USER/COMPUTER/PROGRAM) : "));
			ZeroMemory(szCommand, sizeof(szCommand));
			_getts_s(szCommand);

			// 키보드로 입력받은 내용을 소문자로 바꾸어 비교합니다.
			if (!_tcscmp(_tcslwr(szCommand), _T("user")))
			{
				_tprintf(_T("=== USER ===\n"));
				// REG 인지 QUERY인지 선택합니다.
				_tprintf(_T("Enter Command (REG/QUERY) : "));
				ZeroMemory(szCommand, sizeof(szCommand));
				_getts_s(szCommand);

				// REG일 경우
				if (!_tcscmp(_tcslwr(szCommand), _T("reg")))
				{
					WCHAR szUserID[32] = { 0, };
					WCHAR szUserName[32] = { 0, };
					DWORD dwAge = 0;
					BYTE cSex = 0;
					WCHAR szAddress[64] = { 0, };

					// USER_ID를 입력 받습니다.
					_tprintf(_T("Enter USER_ID : "));
					ZeroMemory(szCommand, sizeof(szCommand));
					_getts_s(szCommand);
					_tcscpy(szUserID, szCommand);

					// USER_NAME를 입력 받습니다.
					_tprintf(_T("Enter USER_NAME : "));
					ZeroMemory(szCommand, sizeof(szCommand));
					_getts_s(szCommand);
					_tcscpy(szUserName, szCommand);

					// AGE를 입력 받습니다.
					_tprintf(_T("Enter AGE : "));
					ZeroMemory(szCommand, sizeof(szCommand));
					_getts_s(szCommand);
					dwAge = _ttoi(szCommand);

					// SEX를 입력 받습니다.
					_tprintf(_T("Enter SEX : "));
					ZeroMemory(szCommand, sizeof(szCommand));
					_getts_s(szCommand);
					cSex = _ttoi(szCommand);

					// ADDRESS를 입력 받습니다.
					_tprintf(_T("Enter ADDRESS : "));
					ZeroMemory(szCommand, sizeof(szCommand));
					_getts_s(szCommand);
					_tcscpy(szAddress, szCommand);

					ClientSession->WritePacket(
						PT_REG_USER,
						WriteBuffer,
						WRITE_PT_REG_USER(
							WriteBuffer,
							szUserID,
							szUserName,
							dwAge,
							cSex,
							szAddress)
					);

					_tprintf(_T("Press any Key... "));
					getchar();
				}
				else
				{
					// Query일 경우
					WCHAR szUserID[32] = { 0, };

					// 검색할 USER_ID를 입력 받습니다.
					_tprintf(_T("Enter USER_ID : "));
					ZeroMemory(szCommand, sizeof(szCommand));
					_getts_s(szCommand);
					_tcscpy(szUserID, szCommand);

					// 검색 패킷을 전송합니다.
					ClientSession->WritePacket(
						PT_QUERY_USER,
						WriteBuffer,
						WRITE_PT_QUERY_USER(
							WriteBuffer,
							szUserID
						)
					);

					// 전송할 패킷의 결과가 올 때 까지 계속 패킷을 읽습니다.
					// 계속 읽는 이유는 Server에 추가했던 KeepAlive 패킷 때문입니다.
					while (TRUE)
					{
						if (!ClientSession->ReadPacket(Protocol, Packet, PacketLength))
							if (Protocol == PT_QUERY_USER_RESULT) break;
					}

					// 패킷을 읽습니다.
					READ_PACKET(PT_QUERY_USER_RESULT);

					// 읽은 패킷을 출력합니다.
					_tprintf(_T("QUERY : USER_ID(%s), USER_NAME(%s), USER_AGE(%d).USER_SEX(%d), USER_ADDRESS(%s)"),
						Data.USER_ID, Data.USER_NAME, Data.AGE, Data.SEX, Data.ADDRESS);

					_tprintf(_T("Press any Key... "));
					getchar();
				}
			}
		}
	}

	// 아무키나 눌렀을 경우 종료시퀸스가 처리됩니다.
	ClientSession->End();

	delete ClientSession;

	// Winsock을 Unload합니다.
	WSACleanup();

	return 0;
}