#pragma comment(lib, "ws2_32")
#include <stdio.h>
#include <windows.h>
#include <tchar.h>
#include <stdlib.h>
#include <locale.h>

#define SERVERPORT 9000
#define BUFSIZE 50

void error_display(TCHAR *msg);
void error_quit(TCHAR *msg);
int recvn(SOCKET s, char *buf, int len, int flags);

int _tmain(int argc, TCHAR* argv[])
{
	int retval;
	_tsetlocale(LC_ALL, _T("korean"));		// �����ڵ� ��������

	// ���� �ʱ�ȭ
	WSADATA wsa;
	if (WSAStartup(MAKEWORD(2,2), &wsa) != 0) return 1;
	// ���� ����
	SOCKET sock = socket(AF_INET, SOCK_STREAM, 0);
	if (sock == INVALID_SOCKET) error_quit(_T("socket()"));

	// bind
	SOCKADDR_IN serveraddr;
	memset((void *)&serveraddr, 0, sizeof(serveraddr));
	serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);
	serveraddr.sin_family = AF_INET;
	serveraddr.sin_port = htons(SERVERPORT);
	retval = bind(sock, (SOCKADDR *)&serveraddr, sizeof(serveraddr));
	if (retval == SOCKET_ERROR) error_quit(_T("bind()"));
	
	// listen
	retval = listen(sock, SOMAXCONN);
	if (retval == SOCKET_ERROR) error_quit(_T("listen()"));

	// ������ ó���� ����� ����
	SOCKET client_sock;
	SOCKADDR_IN clientaddr;
	memset((void *)&clientaddr, 0, sizeof(clientaddr));
	int addrlen;
	char buf[BUFSIZE + 1];

	while (1)
	{
		addrlen = sizeof(clientaddr);
		// accept
		client_sock = accept(sock, (SOCKADDR *)&clientaddr, &addrlen);
		if (client_sock == INVALID_SOCKET)
		{
			error_display(_T("accept()"));
			break;
		}
		// ������ Ŭ���̾�Ʈ ���� ���
		printf("[TCP] ����� ���� IP: %s, PORT: %d\n", inet_ntoa(clientaddr.sin_addr), ntohs(clientaddr.sin_port));

		// Ŭ���̾�Ʈ�� ������ ���
		while(1)
		{
			retval = recvn(client_sock, buf, BUFSIZE, 0);
			if (retval == SOCKET_ERROR)
			{
				error_display(_T("recvn()"));
				break;
			}
			else if (retval == 0) break;
			// ���� ������ ���
			buf[retval] = '\0';
			printf("[TCP/%s:%d] %s\n", inet_ntoa(clientaddr.sin_addr), ntohs(clientaddr.sin_port), buf);
		}
		closesocket(client_sock);
		printf("[TCP ����] Ŭ���̾�Ʈ ���� : IP �ּ�=%s, ��Ʈ��ȣ=%d\n", inet_ntoa(clientaddr.sin_addr), ntohs(clientaddr.sin_port));
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
int recvn(SOCKET s, char *buf, int len, int flags)		// �׻� ������ ������ �����͸� ���� �� return �ϴ� �Լ��̴� !!
{
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
	return (len - left);
}