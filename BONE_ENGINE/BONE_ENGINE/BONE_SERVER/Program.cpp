#include "PreHeader.h"

int _tmain(int argc, _TCHAR* argv[])
{
	CoInitialize(NULL);

	WSADATA WsaData;
	WSAStartup(MAKEWORD(2, 2), &WsaData);

	CServerIOCP ServerIocp;

	if (ServerIocp.Begin())
	{
		getchar();
		ServerIocp.End();
	}

	WSACleanup();
	CoUninitialize();

	return 0;
}