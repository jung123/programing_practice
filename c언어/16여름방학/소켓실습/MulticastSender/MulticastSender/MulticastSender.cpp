// MulticastSender.cpp : 콘솔 응용 프로그램에 대한 진입점을 정의합니다.
//

#include "stdafx.h"

#define MULTICASTIP "235.7.8.9"
#define REMOTEPORT 9000
#define BUFSIZE 512

void error_quit(TCHAR *msg);
void error_display(TCHAR *msg);

int main()
{
	int retval;
	_tsetlocale(LC_ALL, _T("korean"));

	WSADATA wsa;
	if ( WSAStartup(MAKEWORD(2,2),&wsa) != 0 ) return 1;

	SOCKET sock = socket(AF_INET, SOCK_DGRAM, 0);
	if (sock == INVALID_SOCKET) error_quit(_T("socket()"));
	// 멀티캐스트 ttl 설정
	int ttl = 2;
	retval = setsockopt(sock, IPPROTO_IP, IP_MULTICAST_TTL, (char *)&ttl, sizeof(ttl));
	if (retval == SOCKET_ERROR) error_quit(_T("setsockopt()"));

	SOCKADDR_IN remoteaddr;
	memset((void *)&remoteaddr, 0, sizeof(remoteaddr));
	remoteaddr.sin_family = AF_INET;
	remoteaddr.sin_addr.s_addr = inet_addr(MULTICASTIP);
	remoteaddr.sin_port = htons(REMOTEPORT);

	char buf[BUFSIZE+1];
	int len;

	while (1)
	{
		printf("\n[보낼 데이터] ");
		if (fgets(buf, BUFSIZE, stdin) == NULL) break;

		len = strlen(buf);
		if (buf[len - 1] == '\n') buf[len - 1] = '\0';
		if (len == 0) break;

		retval = sendto(sock, buf, strlen(buf), 0, (SOCKADDR *)&remoteaddr, sizeof(remoteaddr));
		if (retval == SOCKET_ERROR)
		{
			error_display(_T("sendto()"));
			continue;
		}
		printf("%d 바이트를 보냈습니다.\n", retval);
	}
	closesocket(sock);
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
		(LPTSTR)lpMsgBuf, 0, NULL
	);
	MessageBox(NULL, (LPCTSTR)lpMsgBuf, msg, 0);
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
		(LPTSTR)lpMsgBuf, 0, NULL
	);
	printf("[ERROR:%s] %s\n",(char *)msg, lpMsgBuf);
	LocalFree(lpMsgBuf);
}

