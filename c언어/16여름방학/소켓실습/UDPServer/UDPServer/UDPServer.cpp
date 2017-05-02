// UDPServer.cpp : �ܼ� ���� ���α׷��� ���� �������� �����մϴ�.
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
		buf[retval] = '\0'; // �׻� ������������ �������� '\0' ���� ������. BUFSIZE��ŭ �͵� buf�� 513byte �Ҵ�Ǿ� �ִ�. !!
		printf("[UDP ����/%s:%d] %s [������ ����]\n", inet_ntoa(clientaddr.sin_addr), ntohs(clientaddr.sin_port), buf);
		
		retval = sendto(udp_sock, buf, retval, 0, (SOCKADDR *)&clientaddr, sizeof(clientaddr));
		if (retval == SOCKET_ERROR)
		{
			error_display(_T("sendto()"));
			continue;
		}
		printf("[UDP����] %d������ ����[%s:%d]\n", retval,inet_ntoa(clientaddr.sin_addr), ntohs(clientaddr.sin_port));
		
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

