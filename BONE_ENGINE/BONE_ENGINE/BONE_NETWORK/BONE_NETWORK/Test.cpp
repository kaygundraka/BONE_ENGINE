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
			// IOCP 초기 받기를 해줍니다.
			if (!CPacketSession::InitializeReadForIOCP())
				return FALSE;

			// 테스트용 코드로 10바이트 데이터를 전송합니다.
			BYTE WriteData[10] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9 };

			// 프로토콜 번호 0번인 10바이트 데이터를 전송합니다.
			return WritePacket(0, WriteData, 10);
		}

		// 클라이언트 접속 종료 후 처리하는 함수
		BOOL OnIoDisConnected(SOCKET _listenSocket)
		{
			// 초기화를 해줍니다.
			return Initialze(_listenSocket);
		}
	};

	class CTestIocp : public CIOCP {
	private:
		// CPacketSession 을 상속 받은 CTestSession입니다.
		// 여기서는 단순히 TestSession으로 이름을 붙였지만 나중에 게임 서버나 다른 서버에 적용할 때에는
		// 해당 목적에 맞는 클래스 이름을 정해주면 됩니다.
		CTestSession *listenSession; // 서버의 Listen을 담당할 개체입니다.
		CTestSession *testSessions[MAX_SESSION];
		// 접속할 개체들을 관리할 배열입니다. MAX_SESSION은 10으로 정의되어 있습니다.

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

			// 위의 순서를 익혀두는 것이 좋습니다.
			// 서버를 제작할 경우 Listen을 담당하는 개체를 처음 생성한 후 TCP일 경우
			// Bind->Listen을 해주는 순서입니다.

			// Listen의 한 개체를 IOCP에 등록해줍니다.
			// reinterpret_cast를 통해서 형 변환을 해줍니다.
			if (!RegisterSocketToIocp(listenSession->GetSocket(), reinterpret_cast<ULONG_PTR>(listenSession)))
				return FALSE;

			// 접속할 클라이언트를 관리하는 개체를 초기화합니다.
			for (USHORT i = 0; i < MAX_SESSION; i++)
			{
				// 접속할 클라이언트 개체를 생성
				testSessions[i] = new CTestSession();

				// 초기화합니다. 내부는 다음 코드에서 살펴보겠습니다.
				if (!testSessions[i]->Initialze(listenSession->GetSocket()))
				{
					// 만약 실패했을 경우 지금까지 생성한 개체를 삭제합니다.
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
			// Listen 개체를 종료합니다.
			listenSession->End();

			// 접속할 클라이언트를 관리하는 개체들을 종료, 삭제합니다.
			for (USHORT i = 0; i < MAX_SESSION; i++)
			{
				testSessions[i]->End();

				delete testSessions[i];
			}

			// listen 개체를 삭제합니다.
			delete listenSession;

			// 부모 클래스인 CIocp의 End를 호출합니다.
			// CIOCP의 End를 호출함으로써 등록한 모든 핸들이 리셋됩니다.
			return CIOCP::End();
		}

		// AcceptEx를 통해서 클라이언트가 접속했을때 호출
		// 가상 함수 파라미터로 접속에 사용된 개체가 넘어오게 됩니다.
		// 지금은 VOID*형으로 되어 있지만 형 변환을 하면 CTestSession으로 사용할 수 있습니다.
		virtual VOID OnIoConnected(VOID *_object)
		{
			_tprintf(_T("OnIoConnected 0x%x\n"), _object);

			// 가상 함수 파라미터로 접속한 클라이언트를 관리할 개체 포인터가 넘어옵니다.
			// 이것을 형 변환해서 사용합니다.
			// reinterpret_cast를 사용하는 이유는 좀 더 형 변환 성능을 향상시키기 위해서 입니다.
			CTestSession *Session = reinterpret_cast<CTestSession*>(_object);

			// 앞으로 사용할 개체와 소켓을 IOCP에 등록합니다.
			// 앞으로 접속한 개체의 모든 행동은 IOCP로 신호가 오게 됩니다.
			// IOCP로 등록하면서 같이 등록할 키는 개체의 포인터로 합니다.
			if (!RegisterSocketToIocp(Session->GetSocket(), reinterpret_cast<ULONG_PTR>(Session)))
				return;

			// 개체가 접속에 성공했을 때 처리할 함수를 호출합니다.
			if (!Session->OnIoConnected())
				return;
		}

		// 클라이언트 접속 종료 시 불러지는 가상 함수
		// 역시 접속 종료되는 개체의 파라미터가 넘어갑니다.
		virtual VOID OnIoDisconnected(VOID *_object)
		{
			_tprintf(_T("OnIoDisconnected 0x%x\n"), _object);

			// 접속을 종료하는 개체의 포인터가 넘어옵니다.
			CTestSession *Session = reinterpret_cast<CTestSession*>(_object);

			// 접속 종료 시 처리할 함수를 호출합니다.
			if (!Session->OnIoDisConnected(listenSession->GetSocket()))
				return;
		}

		// 접속 완료 IO가 발생
		// 전송이 완료된 개체의 파라미터가 넘어갑니다.
		virtual VOID OnIoWrote(VOID *_object, DWORD _dataLength)
		{
			_tprintf(_T("OnIoConnected 0x%x\n"), _object);

			// 개체로 사용하기 위해서 형 변환을 해줍니다.
			CTestSession *Session = reinterpret_cast<CTestSession*>(_object);

			// 전송 완료가 될 때까지 살려두었던 데이터를 삭제합니다.
			Session->WriteComplete();
		}

		// 클라이언트에서 데이터가 왔을 때 호출되어 처리하는 함수 입니다.
		// OnIoRead에서는 개체 파라미터 이외에도 받은 데이터의 길이가 파라미터로 넘어가게 됩니다.
		virtual VOID OnIoRead(VOID *_object, DWORD _dataLength)
		{
			// 받은 데이터의 길이를 저장하는 변수
			DWORD DataLength = 0;

			// 해당 데이터의 목적이 저장되는 프로토콜 변수
			DWORD Protocol = 0;

			// 하나의 패킷을 저장하는 버퍼 MAX_BUFFER_LENGTH는 4096으로 정의되어 있습니다.
			BYTE Packet[MAX_BUFFER_LENGTH] = { 0, };

			// 받은 패킷의 길이
			DWORD PacketLength = 0;

			// 데이터를 받은 개체 포인터를 형 변환하여 확인합니다.
			CTestSession *Session = reinterpret_cast<CTestSession*>(_object);

			// CNetworkSession에서 CPacketSession으로 데이터를 옮깁니다.
			// DataLength는 레퍼런스를 이용해서 받은 데이터 길이가 들어가게 됩니다.
			if (!Session->ReadPacketForIocp(DataLength))
				return;

			// 받은 패킷을 분석하여 프로토콜을 확인하여 처리합니다.
			// CPacketSession 안에 있는 버퍼에서 하나의 패킷을 빼내어 옵니다.
			// GetPacket 함수를 호출하면 Protocol, Packet, PacketLength가 넘어오게 됩니다.
			// 이 부분을 While로 돌리는 이유는 Nagle에 의해서 패킷이 모자라게 오거나
			// 2개 이상의 패킷이 뭉쳐서 왔을 때 연속적으로 채킷을 처리하기 위함입니다.
			while (Session->GetPacket(Protocol, Packet, PacketLength))
			{
				// 프로토콜처리부
			}
		}
	};
}

using namespace BONE_NETWORK;

int _tmain(int argc, _TCHAR* argv[])
{
	// Winsock DLL을 로드합니다
	WSADATA WsaData;
	WSAStartup(MAKEWORD(2, 2), &WsaData);

	CTestIocp Iocp;

	Iocp.Begin();

	getchar();

	Iocp.End();

	WSACleanup();

	return 0;
}