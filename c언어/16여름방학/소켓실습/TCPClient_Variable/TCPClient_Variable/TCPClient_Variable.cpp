// TCPClient_Variable.cpp : 콘솔 응용 프로그램에 대한 진입점을 정의합니다.
//

#include "stdafx.h"

#define SERVERIP "127.0.0.1"
#define SERVERPORT 9000
#define BUFSIZE 50

void error_display(TCHAR *msg);
void error_quit(TCHAR *msg);

int _tmain(int argc, TCHAR* argv[])
{
	int retval = 0;

	WSADATA wsa;
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) return 1;

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
	char buf[BUFSIZE];
	char *testdata[] = {
		"qweqweqweqwe",
		"안녕하세요.",
		"반가워요",
		"오늘따라 할 이야기가 많아요",
		"저도 그렇네요"
	};
	int len;


	for (int i = 0; i < 5; i++) {
		len = strlen(testdata[i]);
		
		strncpy_s(buf, sizeof(buf), testdata[i], len++);	// 자동으로 문자열 끝을 의미하는 '\0'을 마지막에 붙혀주는 듯
		buf[len++] = '\n';							// 데이터의 경계를 의미한다. !!!

		retval = send(sock, buf, len, 0);
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