#include "PreHeader.h"
#include "CTestClientSession.h"

#define READ_PACKET(PROTOCOL)\
	BYTE WriteBuffer[MAX_BUFFER_LENGTH] = {0, };\
	S_##PROTOCOL Data;\
	READ_##PROTOCOL(Packet, Data); 

int _tmain(int argc, _TCHAR* argv[])
{
	// Winsock�� ����ϱ� ���� DLL �ε�
	WSADATA WsaData;
	WSAStartup(MAKEWORD(2, 2), &WsaData);

	// ��� ��ü�� �����մϴ�.
	CTestClientSession *ClientSession = new CTestClientSession();

	// TCP�� ����� ���� �����ϰ� ������ IP, PORT�� �����մϴ�.
	// �������� ��� getchar�� ���� ��� ���� ���°� �˴ϴ�.
	if (ClientSession->BeginTCP("127.0.0.1", DEFAULT_PORT))
	{
		while (TRUE)
		{
			// ��Ŷ�� ���� �� ����ϴ� ����
			BYTE WriteBuffer[MAX_BUFFER_LENGTH] = { 0, };
			// ��Ŷ�� ���� �� ����ϴ� ��������, ����, ��Ŷ ����
			DWORD Protocol = 0, PacketLength = 0;
			BYTE Packet[MAX_BUFFER_LENGTH] = { 0, };

			// ȭ���� �� �� �����ݴϴ�.
			system("cls");

			// Ű����� �Է��� ������ �����ϴ� ����
			// ������ ��� Ű����� �Է��� ������ �̰��� ����˴ϴ�.
			TCHAR szCommand[32] = { 0, };
			// USER, COMPUTER, PROGRAM�� ������ �޽��ϴ�.
			_tprintf(_T("Enter command (USER/COMPUTER/PROGRAM) : "));
			ZeroMemory(szCommand, sizeof(szCommand));
			_getts_s(szCommand);

			// Ű����� �Է¹��� ������ �ҹ��ڷ� �ٲپ� ���մϴ�.
			if (!_tcscmp(_tcslwr(szCommand), _T("user")))
			{
				_tprintf(_T("=== USER ===\n"));
				// REG ���� QUERY���� �����մϴ�.
				_tprintf(_T("Enter Command (REG/QUERY) : "));
				ZeroMemory(szCommand, sizeof(szCommand));
				_getts_s(szCommand);

				// REG�� ���
				if (!_tcscmp(_tcslwr(szCommand), _T("reg")))
				{
					WCHAR szUserID[32] = { 0, };
					WCHAR szUserName[32] = { 0, };
					DWORD dwAge = 0;
					BYTE cSex = 0;
					WCHAR szAddress[64] = { 0, };

					// USER_ID�� �Է� �޽��ϴ�.
					_tprintf(_T("Enter USER_ID : "));
					ZeroMemory(szCommand, sizeof(szCommand));
					_getts_s(szCommand);
					_tcscpy(szUserID, szCommand);

					// USER_NAME�� �Է� �޽��ϴ�.
					_tprintf(_T("Enter USER_NAME : "));
					ZeroMemory(szCommand, sizeof(szCommand));
					_getts_s(szCommand);
					_tcscpy(szUserName, szCommand);

					// AGE�� �Է� �޽��ϴ�.
					_tprintf(_T("Enter AGE : "));
					ZeroMemory(szCommand, sizeof(szCommand));
					_getts_s(szCommand);
					dwAge = _ttoi(szCommand);

					// SEX�� �Է� �޽��ϴ�.
					_tprintf(_T("Enter SEX : "));
					ZeroMemory(szCommand, sizeof(szCommand));
					_getts_s(szCommand);
					cSex = _ttoi(szCommand);

					// ADDRESS�� �Է� �޽��ϴ�.
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
					// Query�� ���
					WCHAR szUserID[32] = { 0, };

					// �˻��� USER_ID�� �Է� �޽��ϴ�.
					_tprintf(_T("Enter USER_ID : "));
					ZeroMemory(szCommand, sizeof(szCommand));
					_getts_s(szCommand);
					_tcscpy(szUserID, szCommand);

					// �˻� ��Ŷ�� �����մϴ�.
					ClientSession->WritePacket(
						PT_QUERY_USER,
						WriteBuffer,
						WRITE_PT_QUERY_USER(
							WriteBuffer,
							szUserID
						)
					);

					// ������ ��Ŷ�� ����� �� �� ���� ��� ��Ŷ�� �н��ϴ�.
					// ��� �д� ������ Server�� �߰��ߴ� KeepAlive ��Ŷ �����Դϴ�.
					while (TRUE)
					{
						if (!ClientSession->ReadPacket(Protocol, Packet, PacketLength))
							if (Protocol == PT_QUERY_USER_RESULT) break;
					}

					// ��Ŷ�� �н��ϴ�.
					READ_PACKET(PT_QUERY_USER_RESULT);

					// ���� ��Ŷ�� ����մϴ�.
					_tprintf(_T("QUERY : USER_ID(%s), USER_NAME(%s), USER_AGE(%d).USER_SEX(%d), USER_ADDRESS(%s)"),
						Data.USER_ID, Data.USER_NAME, Data.AGE, Data.SEX, Data.ADDRESS);

					_tprintf(_T("Press any Key... "));
					getchar();
				}
			}
		}
	}

	// �ƹ�Ű�� ������ ��� ����������� ó���˴ϴ�.
	ClientSession->End();

	delete ClientSession;

	// Winsock�� Unload�մϴ�.
	WSACleanup();

	return 0;
}