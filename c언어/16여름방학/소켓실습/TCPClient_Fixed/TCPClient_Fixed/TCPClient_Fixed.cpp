#pragma comment(lib,"ws2_32")
#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
#include <locale.h>
#include <tchar.h>

#define SERVERIP "127.0.0.1"
#define SERVERPORT 9000
#define BUFSIZE 50

void error_display(TCHAR *msg);
void error_quit(TCHAR *msg);

int _tmain(int argc, TCHAR* argv[])
{
	int retval = 0;

	WSADATA wsa;
	if (WSAStartup(MAKEWORD(2,2),&wsa) != 0) return 1;

	SOCKET sock = socket(AF_INET, SOCK_STREAM, 0);
	if (sock == INVALID_SOCKET) error_quit(_T("socket()"));

	SOCKADDR_IN serveraddr;
	memset((void *)&serveraddr, 0, sizeof(serveraddr));
	serveraddr.sin_addr.s_addr = inet_addr(SERVERIP);
	serveraddr.sin_port = htons(SERVERPORT);
	serveraddr.sin_family = AF_INET;
	retval = connect(sock, (SOCKADDR *)&serveraddr, sizeof(serveraddr));
	if (retval == SOCKET_ERROR) error_quit(_T("connect()"));

	//
	char buf[BUFSIZE + 1] = "\0";
	char *testdata[] = {
		"안녕하세요.",
		"반가워요",
		"오늘따라 할 이야기가 많아요",
		"저도 그렇네요"
	};


	for (int i = 0; i < 4; i++) {
		memset(buf, '#', sizeof(buf));
		strncpy_s(buf, sizeof(buf), testdata[i], strlen(testdata[i]));

		retval = send(sock, buf, BUFSIZE, 0);
		if (retval == SOCKET_ERROR) {
			error_display(_T("send()"));
			break;
		}
		printf("[TCP 클라이언트] %d바이트를 보냈습니다. !\n", retval);
	}


	closesocket(sock);
	WSACleanup();

	return 0;
}
void error_display(TCHAR *msg)
{
	LPVOID lpMsgBuf = NULL;
	FormatMessage(
		FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
		NULL,
		WSAGetLastError(),
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		(LPTSTR)lpMsgBuf, 0, NULL
	);
	_tprintf(_T("[ERROR / %s] %s\n"), msg, (TCHAR)lpMsgBuf);		// (TCHAR 형으로 캐스팅 되는지 바야함)
	LocalFree(lpMsgBuf);
}
void error_quit(TCHAR *msg)
{
	LPVOID lpMsgBuf = NULL;
	FormatMessage(
		FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
		NULL,
		WSAGetLastError(),
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		(LPTSTR)lpMsgBuf, 0, NULL
	);
	MessageBox(NULL, (LPCTSTR)lpMsgBuf, msg, MB_ICONERROR);
	LocalFree(lpMsgBuf);
	exit(1);
}