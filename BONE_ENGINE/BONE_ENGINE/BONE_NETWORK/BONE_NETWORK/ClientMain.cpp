#include"Common.h"
#pragma comment(lib,"ws2_32.lib")
using namespace BONE_NETWORK;

int Count = 0;

// 소켓 초기화
void Init_socket(SOCKET* sock, int sockType, bool isBind, PSOCKADDR_IN localaddr, int port)
{
	int retval;
	// 윈속 초기화
	WSADATA wsa;
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
	{
		::ExitProcess(-1);
	}

	// socket()
	(*sock) = socket(AF_INET, sockType, 0);
	if (*sock == INVALID_SOCKET) Error_Quit(TEXT("socket()"));


	if (isBind)
	{
		// bind()
		//SOCKADDR_IN            localaddr;
		ZeroMemory(&(*localaddr), sizeof(*localaddr));
		(localaddr)->sin_family = AF_INET;
		(localaddr)->sin_port = htons(port);
		(localaddr)->sin_addr.s_addr = htonl(INADDR_ANY);
		retval = bind(​*sock, (SOCKADDR*​)&(*localaddr),
			sizeof(*localaddr));
	}
}


void set_sockaddrin(PSOCKADDR_IN sockaddr, int port, BOOL isServer, u_long saddr, char *serverIp)
{
	ZeroMemory(&(*sockaddr), sizeof(*sockaddr));
	(*sockaddr).sin_family = AF_INET;
	(*sockaddr).sin_port = htons(port);
	//-------------------------------------------------------
	if (isServer)
		(*sockaddr).sin_addr.s_addr = htonl(saddr);
	else
		(*sockaddr).sin_addr.s_addr = inet_addr(serverIp);
	//-------------------------------------------------------
}

// TCP 소켓 송신 함수.. 
void tcp_send(SOCKET descSock, char* sendbuff, int size)
{
	int        nBytes = size,
		nLeft,
		idx;

	int ret;


	//sendbuff에 size만큼 데이터를 채운다.

	nLeft = nBytes;
	idx = 0;

	while (nLeft > 0)
	{
		ret = send(descSock, &sendbuff[idx], nLeft, 0);
		if (ret == SOCKET_ERROR)
		{
			// error Handling
		}
		nLeft -= ret;
		idx += ret;
	}

}


// TCP 소켓 수신 함수..
int tcp_recv(SOCKET s, char *buf, int len, int flags)
{
	int received;        char *ptr = buf;            int left = len;
	while (left > 0) {
		received = recv(s, ptr, left, flags);
		if (received == SOCKET_ERROR)         return SOCKET_ERROR;
		else if (received == 0)                 return -1;
		left -= received;
		ptr += received;
	}
	return (len - left);
}

int main()
{
	int i;
	char buf[BUFSIZE];

	// 소켓 초기화
	SOCKET mySocket[TOTALCOUNT];

	for (i = 0; i<TOTALCOUNT; i++)
	{
		Init_socket(&mySocket[i], SOCK_STREAM, FALSE, NULL, NULL);
	}

	// 접속할 서버 세팅
	SOCKADDR_IN serverAddr;
	set_sockaddrin(&serverAddr, 9000, FALSE, NULL, "129.254.192.66");

	// 접속
	for (i = 0; i<TOTALCOUNT; i++) {
		if (connect(mySocket[i], (SOCKADDR*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR)
			Error_Quit(TEXT("접속실패"));
		printf("연결됨 %d개 \r", ++Count);
		Sleep(1);
	}

	while (1) {
		// 메세지 전송 :
		for (i = 0; i<TOTALCOUNT; i++)
		{
			sprintf(buf, "SEND : %d", i);
			tcp_send(mySocket[i], buf, BUFSIZE);
			printf("SEND : %d \r", i);
		}

		printf("\n 메세지를 다 날렸습니다. 보낸 메세지를 받습니다. ");

		printf("\n");


		// 메세지 받는다
		for (i = 0; i<TOTALCOUNT; i++)
		{
			tcp_recv(mySocket[i], buf, BUFSIZE, 0);
			printf("받은 메세지 RECV : %s \r", buf);
			Sleep(3);
		}

	}


	printf("\n");
	system("pause");

	// 닫기 
	printf("\n엔터를 누르시면 소켓을 닫습니다.");

	for (i = 0; i<TOTALCOUNT; i++)
	{
		closesocket(mySocket[i]);
		printf("닫음 %d개 \r", --Count);
	}


	return 0;
}