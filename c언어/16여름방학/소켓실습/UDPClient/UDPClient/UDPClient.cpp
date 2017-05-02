// UDPClient.cpp : 콘솔 응용 프로그램에 대한 진입점을 정의합니다.
//

#include "stdafx.h"

#define SERVERPORT 9000
#define SERVERIP "127.0.0.1"
#define BUFSIZE 512

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

int main()
{
	_tsetlocale(LC_ALL, _T("korean"));
	int retval;
	WSADATA wsa;
	if ( WSAStartup(MAKEWORD(2,2),&wsa) != 0 ) return 1;

	SOCKET sock = socket(AF_INET, SOCK_DGRAM, 0);
	if (sock == INVALID_SOCKET) return 1;

	SOCKADDR_IN serveraddr;
	memset((void *)&serveraddr, 0, sizeof(serveraddr));
	serveraddr.sin_family = AF_INET;
	serveraddr.sin_addr.s_addr = inet_addr(SERVERIP);
	serveraddr.sin_port = htons(SERVERPORT);
	

	char buf[BUFSIZE + 1];
	SOCKADDR_IN serveraddr1;
	int addrlen;
	char *retch = NULL;
	while (1)
	{
		retch = fgets(buf, BUFSIZE, stdin);
		if (retch == NULL)
		{
			printf("fgets() Error !! \n");
			break;
		}
		int temp = strlen(buf);
		if (buf[temp-1] == '\n') buf[temp] = '\0';
		else if (temp == 0) break;
		retval = sendto(sock, buf,temp, 0, (SOCKADDR *)&serveraddr, sizeof(serveraddr));
		if (retval == SOCKET_ERROR) {
			error_display(_T("serndto()"));
			continue;
		}

		printf("*** %d데이터를 전송했습니다.!\n", retval);

		addrlen = sizeof(serveraddr1);
		retval = recvfrom(sock, buf, BUFSIZE, 0, (SOCKADDR *)&serveraddr1, &addrlen);
		if (retval == SOCKET_ERROR) {
			error_display(_T("recvfrom()"));
			continue;
		}
		buf[retval] = '\0';
		printf("[UDP/%s:%d] %s[데이터받음]\n", inet_ntoa(serveraddr1.sin_addr), ntohs(serveraddr1.sin_port), buf);
	}
	closesocket(sock);
	WSACleanup();

    return 0;
}

