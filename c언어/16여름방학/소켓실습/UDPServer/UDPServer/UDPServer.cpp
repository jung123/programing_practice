// UDPServer.cpp : 콘솔 응용 프로그램에 대한 진입점을 정의합니다.
//

#include "stdafx.h"

#define SERVERPORT 9000
#define BUFSIZE 512

void error_quit(TCHAR *msg);
void error_display(TCHAR *msg);


int main()
{
	int retval;
	WSADATA wsa;
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) return 1;

	SOCKET udp_sock = socket(AF_INET, SOCK_DGRAM, 0);
	if (udp_sock == INVALID_SOCKET) error_quit(_T("socket()"));

	SOCKADDR_IN serveraddr;
	memset((void *)&serveraddr, 0, sizeof(serveraddr));
	serveraddr.sin_family = AF_INET;
	serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);
	serveraddr.sin_port = htons(SERVERPORT);
	retval = bind(udp_sock, (SOCKADDR *)&serveraddr, sizeof(serveraddr));
	if (retval == SOCKET_ERROR) error_quit(_T("bind()"));

	int addrlen;
	SOCKADDR_IN clientaddr;
	char buf[BUFSIZE+1];

	while (1)
	{
		addrlen = sizeof(clientaddr);
		retval = recvfrom(udp_sock, buf, BUFSIZE, 0, (SOCKADDR *)&clientaddr, &addrlen);
		if (retval == SOCKET_ERROR)
		{
			error_display(_T("recvfrom()"));
			continue;
		}
		buf[retval] = '\0'; // 항상 받은데이터의 마지막은 '\0' 으로 끝난다. BUFSIZE만큼 와도 buf는 513byte 할당되어 있다. !!
		printf("[UDP 서버/%s:%d] %s [데이터 받음]\n", inet_ntoa(clientaddr.sin_addr), ntohs(clientaddr.sin_port), buf);
		
		retval = sendto(udp_sock, buf, retval, 0, (SOCKADDR *)&clientaddr, sizeof(clientaddr));
		if (retval == SOCKET_ERROR)
		{
			error_display(_T("sendto()"));
			continue;
		}
		printf("[UDP서버] %d데이터 보냄[%s:%d]\n", retval,inet_ntoa(clientaddr.sin_addr), ntohs(clientaddr.sin_port));
		
	}
	closesocket(udp_sock);
	WSACleanup();
    return 0;
}

void error_quit(TCHAR *msg)
{
	LPVOID lpMsgBuf = NULL;
	FormatMessage(
		FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
		NULL,
		WSAGetLastError(),
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		(LPTSTR)lpMsgBuf, 0, NULL);
	MessageBox(NULL, (LPCTSTR)lpMsgBuf, msg, MB_ICONERROR);
	LocalFree(lpMsgBuf);
	exit(1);
}
void error_display(TCHAR *msg)
{
	LPVOID lpMsgBuf = NULL;
	FormatMessage(
		FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
		NULL,
		WSAGetLastError(),
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		(LPTSTR)lpMsgBuf, 0, NULL);
	MessageBox(NULL, (LPCTSTR)lpMsgBuf, msg, MB_ICONERROR);
	LocalFree(lpMsgBuf);
}

