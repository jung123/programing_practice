// TCPServer_FixedVariable.cpp : 콘솔 응용 프로그램에 대한 진입점을 정의합니다.
//

#include "stdafx.h"

#define SERVERPORT 9000
#define BUFSIZE 512

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
int recvn(SOCKET s, char *buf, int len, int flags) {
	int received;
	char *ptr = buf;
	int left = len;

	while (left > 0)
	{
		received = recv(s, ptr, left, flags);
		if (received == SOCKET_ERROR) return SOCKET_ERROR;
		else if (received == 0) break;
		left -= received;
		ptr += received;
	}
	return (len - left);	// 데이터를 받은 크기를 return한다.
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
	serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);
	serveraddr.sin_port = htons(SERVERPORT);
	serveraddr.sin_family = AF_INET;
	retval = bind(sock, (SOCKADDR *)&serveraddr, sizeof(serveraddr));
	if (retval == SOCKET_ERROR) error_quit(_T("bind()"));

	retval = listen(sock, SOMAXCONN);
	if (retval == SOCKET_ERROR) error_quit(_T("listen()"));

	SOCKET client_sock;
	SOCKADDR_IN clientaddr;
	memset((void *)&clientaddr, 0, sizeof(clientaddr));
	int addrlen;
	char buf[BUFSIZE+1];
	int len;

	while (1)
	{
		addrlen = sizeof(clientaddr);
		client_sock = accept(sock, (SOCKADDR *)&clientaddr, &addrlen);
		if (client_sock == INVALID_SOCKET) error_quit(_T("accept()"));

		printf("[TCP 서버] 클라이언트 접속 IP:%s, PORT:%d\n", inet_ntoa(clientaddr.sin_addr), ntohs(clientaddr.sin_port));

		while (1)
		{
			// 고정길이 데이터 받는 부분 (4byte)
			retval = recvn(client_sock, (char *)&len, sizeof(int), 0);
			if (retval == SOCKET_ERROR) {
				error_display(_T("recv()"));
				break;
			}
			else if (retval == 0) break;
			// 가변길이 데이터 받는 부분
			retval = recvn(client_sock, buf, len, 0);
			if (retval == SOCKET_ERROR) {
				error_display(_T("recv()"));
				break;
			}
			else if (retval == 0) break;

			printf("[TCP/ %s:%d] %s(%d)\n", inet_ntoa(clientaddr.sin_addr), ntohs(clientaddr.sin_port), buf, len);

		}
		closesocket(client_sock);
	}
	closesocket(sock);
	WSACleanup();

    return 0;
}

