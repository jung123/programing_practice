// TCPClient_FixedVariable.cpp : 콘솔 응용 프로그램에 대한 진입점을 정의합니다.
//

#include "stdafx.h"
#pragma comment(lib,"ws2_32")
//#include <WinSock2.h>
#include <stdio.h>
#include <tchar.h>
#include <windows.h>
#include <locale.h>
#include <stdlib.h>

#define SERVERPORT 9000
#define SERVERIP "127.0.0.1"
#define BUFSIZE 50

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

int main()
{
	_tsetlocale(LC_ALL, _T("korean"));
	int retval;

	WSADATA wsa;
	if (WSAStartup(MAKEWORD(2,2),&wsa) != 0) return 1;

	SOCKET sock = socket(AF_INET, SOCK_STREAM, 0);
	if (sock == INVALID_SOCKET) error_quit(_T("socket()"));

	SOCKADDR_IN serveraddr;
	memset((void *)&serveraddr, 0, sizeof(serveraddr));
	serveraddr.sin_family = AF_INET;
	serveraddr.sin_addr.s_addr = inet_addr(SERVERIP);
	serveraddr.sin_port = htons(SERVERPORT);
	retval = connect(sock, (SOCKADDR *)&serveraddr, sizeof(serveraddr));
	if (retval == SOCKET_ERROR) error_quit(_T("connect()"));

	char buf[BUFSIZE];
	char *testdata[] = {
		"안녕하세여.",
		"반가워요",
		"오늘따라 할 이야기가 많네요.",
		"저도요^^"
	};
	int len;

	for (int i = 0; i < 4; i++)
	{
		len = strlen(testdata[i]);
		strncpy_s(buf, sizeof(buf), testdata[i], len++);

		retval = send(sock, (char *)&len, sizeof(len), 0);
		if (retval == SOCKET_ERROR) {
			error_display(_T("send()"));
			break;
		}
		
		retval = send(sock, buf, len, 0);
		if (retval == SOCKET_ERROR) {
			error_display(_T("send()"));
			break;
		}
		printf("[TCP] %d+%d데이터를 보냈습니다.\n", sizeof(len), len);

	}
	closesocket(sock);
	WSACleanup();
    return 0;
}

