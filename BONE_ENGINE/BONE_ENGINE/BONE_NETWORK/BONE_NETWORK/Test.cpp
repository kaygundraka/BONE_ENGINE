#include "Common.h"
#include "CIOCP.h"
#include "CPacketSession.h"

namespace BONE_NETWORK
{
	class CTestSession : public CPacketSession {
	public:
		BOOL Initialze(SOCKET _listenSocket)
		{
			if (!Begin())
				return FALSE;

			if (!Accept(_listenSocket))
				return FALSE;

			return TRUE;
		}

		BOOL OnIoConnected(VOID)
		{
			// IOCP �ʱ� �ޱ⸦ ���ݴϴ�.
			if (!CPacketSession::InitializeReadForIOCP())
				return FALSE;

			// �׽�Ʈ�� �ڵ�� 10����Ʈ �����͸� �����մϴ�.
			BYTE WriteData[10] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9 };

			// �������� ��ȣ 0���� 10����Ʈ �����͸� �����մϴ�.
			return WritePacket(0, WriteData, 10);
		}

		// Ŭ���̾�Ʈ ���� ���� �� ó���ϴ� �Լ�
		BOOL OnIoDisConnected(SOCKET _listenSocket)
		{
			// �ʱ�ȭ�� ���ݴϴ�.
			return Initialze(_listenSocket);
		}
	};

	class CTestIocp : public CIOCP {
	private:
		// CPacketSession �� ��� ���� CTestSession�Դϴ�.
		// ���⼭�� �ܼ��� TestSession���� �̸��� �ٿ����� ���߿� ���� ������ �ٸ� ������ ������ ������
		// �ش� ������ �´� Ŭ���� �̸��� �����ָ� �˴ϴ�.
		CTestSession *listenSession; // ������ Listen�� ����� ��ü�Դϴ�.
		CTestSession *testSessions[MAX_SESSION];
		// ������ ��ü���� ������ �迭�Դϴ�. MAX_SESSION�� 10���� ���ǵǾ� �ֽ��ϴ�.

	public:
		CTestIocp() {}

		BOOL Begin(VOID)
		{
			listenSession = new CTestSession();

			if (!CIOCP::Begin())
				return FALSE;
			
			if (!listenSession->Begin())
				return FALSE;

			if (!listenSession->TCPBind())
				return FALSE;

			if (!listenSession->Listen(7900, MAX_SESSION))
				return FALSE;

			// ���� ������ �����δ� ���� �����ϴ�.
			// ������ ������ ��� Listen�� ����ϴ� ��ü�� ó�� ������ �� TCP�� ���
			// Bind->Listen�� ���ִ� �����Դϴ�.

			// Listen�� �� ��ü�� IOCP�� ������ݴϴ�.
			// reinterpret_cast�� ���ؼ� �� ��ȯ�� ���ݴϴ�.
			if (!RegisterSocketToIocp(listenSession->GetSocket(), reinterpret_cast<ULONG_PTR>(listenSession)))
				return FALSE;

			// ������ Ŭ���̾�Ʈ�� �����ϴ� ��ü�� �ʱ�ȭ�մϴ�.
			for (USHORT i = 0; i < MAX_SESSION; i++)
			{
				// ������ Ŭ���̾�Ʈ ��ü�� ����
				testSessions[i] = new CTestSession();

				// �ʱ�ȭ�մϴ�. ���δ� ���� �ڵ忡�� ���캸�ڽ��ϴ�.
				if (!testSessions[i]->Initialze(listenSession->GetSocket()))
				{
					// ���� �������� ��� ���ݱ��� ������ ��ü�� �����մϴ�.
					for (USHORT j = 0; j < i; j++)
					{
						testSessions[j]->End();
						delete testSessions[j];
					}

					return FALSE;
				}
			}

			return TRUE;
		}

		BOOL End(VOID)
		{
			// Listen ��ü�� �����մϴ�.
			listenSession->End();

			// ������ Ŭ���̾�Ʈ�� �����ϴ� ��ü���� ����, �����մϴ�.
			for (USHORT i = 0; i < MAX_SESSION; i++)
			{
				testSessions[i]->End();

				delete testSessions[i];
			}

			// listen ��ü�� �����մϴ�.
			delete listenSession;

			// �θ� Ŭ������ CIocp�� End�� ȣ���մϴ�.
			// CIOCP�� End�� ȣ�������ν� ����� ��� �ڵ��� ���µ˴ϴ�.
			return CIOCP::End();
		}

		// AcceptEx�� ���ؼ� Ŭ���̾�Ʈ�� ���������� ȣ��
		// ���� �Լ� �Ķ���ͷ� ���ӿ� ���� ��ü�� �Ѿ���� �˴ϴ�.
		// ������ VOID*������ �Ǿ� ������ �� ��ȯ�� �ϸ� CTestSession���� ����� �� �ֽ��ϴ�.
		virtual VOID OnIoConnected(VOID *_object)
		{
			_tprintf(_T("OnIoConnected 0x%x\n"), _object);

			// ���� �Լ� �Ķ���ͷ� ������ Ŭ���̾�Ʈ�� ������ ��ü �����Ͱ� �Ѿ�ɴϴ�.
			// �̰��� �� ��ȯ�ؼ� ����մϴ�.
			// reinterpret_cast�� ����ϴ� ������ �� �� �� ��ȯ ������ ����Ű�� ���ؼ� �Դϴ�.
			CTestSession *Session = reinterpret_cast<CTestSession*>(_object);

			// ������ ����� ��ü�� ������ IOCP�� ����մϴ�.
			// ������ ������ ��ü�� ��� �ൿ�� IOCP�� ��ȣ�� ���� �˴ϴ�.
			// IOCP�� ����ϸ鼭 ���� ����� Ű�� ��ü�� �����ͷ� �մϴ�.
			if (!RegisterSocketToIocp(Session->GetSocket(), reinterpret_cast<ULONG_PTR>(Session)))
				return;

			// ��ü�� ���ӿ� �������� �� ó���� �Լ��� ȣ���մϴ�.
			if (!Session->OnIoConnected())
				return;
		}

		// Ŭ���̾�Ʈ ���� ���� �� �ҷ����� ���� �Լ�
		// ���� ���� ����Ǵ� ��ü�� �Ķ���Ͱ� �Ѿ�ϴ�.
		virtual VOID OnIoDisconnected(VOID *_object)
		{
			_tprintf(_T("OnIoDisconnected 0x%x\n"), _object);

			// ������ �����ϴ� ��ü�� �����Ͱ� �Ѿ�ɴϴ�.
			CTestSession *Session = reinterpret_cast<CTestSession*>(_object);

			// ���� ���� �� ó���� �Լ��� ȣ���մϴ�.
			if (!Session->OnIoDisConnected(listenSession->GetSocket()))
				return;
		}

		// ���� �Ϸ� IO�� �߻�
		// ������ �Ϸ�� ��ü�� �Ķ���Ͱ� �Ѿ�ϴ�.
		virtual VOID OnIoWrote(VOID *_object, DWORD _dataLength)
		{
			_tprintf(_T("OnIoConnected 0x%x\n"), _object);

			// ��ü�� ����ϱ� ���ؼ� �� ��ȯ�� ���ݴϴ�.
			CTestSession *Session = reinterpret_cast<CTestSession*>(_object);

			// ���� �Ϸᰡ �� ������ ����ξ��� �����͸� �����մϴ�.
			Session->WriteComplete();
		}

		// Ŭ���̾�Ʈ���� �����Ͱ� ���� �� ȣ��Ǿ� ó���ϴ� �Լ� �Դϴ�.
		// OnIoRead������ ��ü �Ķ���� �̿ܿ��� ���� �������� ���̰� �Ķ���ͷ� �Ѿ�� �˴ϴ�.
		virtual VOID OnIoRead(VOID *_object, DWORD _dataLength)
		{
			// ���� �������� ���̸� �����ϴ� ����
			DWORD DataLength = 0;

			// �ش� �������� ������ ����Ǵ� �������� ����
			DWORD Protocol = 0;

			// �ϳ��� ��Ŷ�� �����ϴ� ���� MAX_BUFFER_LENGTH�� 4096���� ���ǵǾ� �ֽ��ϴ�.
			BYTE Packet[MAX_BUFFER_LENGTH] = { 0, };

			// ���� ��Ŷ�� ����
			DWORD PacketLength = 0;

			// �����͸� ���� ��ü �����͸� �� ��ȯ�Ͽ� Ȯ���մϴ�.
			CTestSession *Session = reinterpret_cast<CTestSession*>(_object);

			// CNetworkSession���� CPacketSession���� �����͸� �ű�ϴ�.
			// DataLength�� ���۷����� �̿��ؼ� ���� ������ ���̰� ���� �˴ϴ�.
			if (!Session->ReadPacketForIocp(DataLength))
				return;

			// ���� ��Ŷ�� �м��Ͽ� ���������� Ȯ���Ͽ� ó���մϴ�.
			// CPacketSession �ȿ� �ִ� ���ۿ��� �ϳ��� ��Ŷ�� ������ �ɴϴ�.
			// GetPacket �Լ��� ȣ���ϸ� Protocol, Packet, PacketLength�� �Ѿ���� �˴ϴ�.
			// �� �κ��� While�� ������ ������ Nagle�� ���ؼ� ��Ŷ�� ���ڶ�� ���ų�
			// 2�� �̻��� ��Ŷ�� ���ļ� ���� �� ���������� äŶ�� ó���ϱ� �����Դϴ�.
			while (Session->GetPacket(Protocol, Packet, PacketLength))
			{
				// ��������ó����
			}
		}
	};
}

using namespace BONE_NETWORK;

int _tmain(int argc, _TCHAR* argv[])
{
	// Winsock DLL�� �ε��մϴ�
	WSADATA WsaData;
	WSAStartup(MAKEWORD(2, 2), &WsaData);

	CTestIocp Iocp;

	Iocp.Begin();

	getchar();

	Iocp.End();

	WSACleanup();

	return 0;
}