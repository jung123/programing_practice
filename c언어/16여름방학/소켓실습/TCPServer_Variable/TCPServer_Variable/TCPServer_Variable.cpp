#pragma comment(lib,"ws2_32")
#include <stdio.h>
#include <stdlib.h>
#include <tchar.h>
#include <locale.h>
#include <windows.h>

#define SERVERPORT 9000
#define BUFSIZE 512

void error_display(TCHAR *msg);
void error_quit(TCHAR *msg);
int _recv_ahead(SOCKET s, char *p);
int recvline(SOCKET s, char *buf, int maxline);

int _tmain(int argc, TCHAR* argv[])
{
	int retval;
	_tsetlocale(LC_ALL, _T("korean"));		// �����ڵ� ��������

											// ���� �ʱ�ȭ
	WSADATA wsa;
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) return 1;
	// ���� ����
	SOCKET sock = socket(AF_INET, SOCK_STREAM, 0);
	if (sock == INVALID_SOCKET) error_quit(_T("socket()"));

	// bind
	SOCKADDR_IN serveraddr;
	memset((void *)&serveraddr, 0, sizeof(serveraddr));
	serveraddr.sin_family = AF_INET;
	serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);
	serveraddr.sin_port = htons(SERVERPORT);
	retval = bind(sock, (SOCKADDR *)&serveraddr, sizeof(serveraddr));
	if (retval == SOCKET_ERROR) error_quit(_T("bind()"));

	// LISTEN()
	retval = listen(sock, SOMAXCONN);
	if (retval == SOCKET_ERROR) error_quit(_T("listen()"));

	// ������ ��ſ� ����� ����
	SOCKET client_sock;
	SOCKADDR_IN clientaddr;
	int addrlen;
	char buf[BUFSIZE + 1];

	while (1)
	{
		addrlen = sizeof(clientaddr);
		client_sock = accept(sock, (SOCKADDR *)&clientaddr, &addrlen);
		if (client_sock == INVALID_SOCKET) {
			error_display(_T("accept()"));
			break;
		}
		printf("[TCP ����] Ŭ���̾�Ʈ ���� IP:%s, PORT:%d\n", inet_ntoa(clientaddr.sin_addr), ntohs(clientaddr.sin_port));
		while (1)
		{
			// ������ �ޱ�
			retval = recvline(client_sock, buf, BUFSIZE + 1);
			if (retval == SOCKET_ERROR) {
				error_display(_T("recv()"));
				break;
			}
			else if (retval == 0) break;

			printf("[TCP/%s:%d] %s\n", inet_ntoa(clientaddr.sin_addr), ntohs(clientaddr.sin_port), buf);
		}
		closesocket(client_sock);
		printf("[TCP ����] Ŭ���̾�Ʈ ���� IP:%s, ��Ʈ ��ȣ:%d\n", inet_ntoa(clientaddr.sin_addr), ntohs(clientaddr.sin_port));
	}

	// ���� �ݱ�
	closesocket(sock);
	// ���� ����
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
	_tprintf(_T("[ERROR / %s] %s\n"), msg, (TCHAR)lpMsgBuf);		// (TCHAR ������ ĳ���� �Ǵ��� �پ���)
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
int _recv_ahead(SOCKET s, char *p)
{
	__declspec(thread) static int nbytes = 0;
	__declspec(thread) static char buf[1024];
	__declspec(thread) static char *ptr;

	if (nbytes == 0 || nbytes == SOCKET_ERROR) {
		nbytes = recv(s, buf, sizeof(buf), 0);
		if (nbytes == SOCKET_ERROR) return SOCKET_ERROR;
		else if (nbytes == 0) return 0;
		ptr = buf;
	}
	--nbytes;
	*p = *ptr++;

	return 1;
}
int recvline(SOCKET s, char *buf, int maxline) // ������ ���̸� ���� ���� ������ ��ٸ��°� �ƴ϶� �ִ���̸�ŭ ���� �� ������ 
{											   // �ϳ��ϳ� �˻縦 �ϸ� \n �� ���� �޴� ���� �����Ѵ�. �ֳ��ϸ� \n�� �Ŀ� �ٸ� ���ڰ� �� �� ����
	int n, nbytes;							  //  ������....
	char c, *ptr = buf;

	for (n = 1; n < maxline; n++)
	{
		nbytes = _recv_ahead(s, &c);		// �̰͵� ���� �� ���� �Լ��� ȣ���ؾ� �ϹǷ� ���ɿ� ������ ��ĥ���ִ�.
		if (nbytes == 1) {
			*ptr++ = c;
			if (c == '\n') break;
		}
		else if (nbytes == 0) {
			*ptr = 0;
			return n - 1;
		}
		else return SOCKET_ERROR;
	}
	*ptr = 0;
	return n;
}