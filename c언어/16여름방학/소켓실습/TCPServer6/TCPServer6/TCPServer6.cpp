#pragma comment(lib, (ws2_32))
#include <stdio.h>
//#include <windows.h>
#include <tchar.h>
#include <locale.h>
#include <ws2tcpip.h>
#include <winsock2.h>

#define SERVERPORT 9000
#define BUFSIZE 512

void error_quit(TCHAR *msg);
void error_display(TCHAR *msg);

int main()
{
	_tsetlocale(LC_ALL, _T("korean"));

	int retval;
	// ���� �ʱ�ȭ
	WSADATA wsa;
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) return 1;
	// ���ϻ���
	SOCKET sock = socket(AF_INET6, SOCK_STREAM, 0);
	if (sock == INVALID_SOCKET) error_quit(_T("socket()"));
	// bind	ipv6�����̴� �ٸ��� �ؾ��Ѵ� !!
	SOCKADDR_IN6 serveraddr;
	ZeroMemory(&serveraddr, sizeof(serveraddr));
	serveraddr.sin6_family = AF_INET6;
	serveraddr.sin6_port = htons(SERVERPORT);
	serveraddr.sin6_addr = in6addr_any;				// in6addr_any �� ���� �𸣰ڴ�...
	retval = bind(sock, (SOCKADDR *)&serveraddr, sizeof(serveraddr));
	if (retval == SOCKET_ERROR) error_quit(_T("bind()"));
	// listen
	retval = listen(sock, SOMAXCONN);			// backlog�� ������ ���� ó������ �������� ������ ������� Ŭ���̾�Ʈ�Ǽ� (= ��� ť�� ����)
	if (retval == SOCKET_ERROR) error_quit(_T("listen()"));
	// ������ ��ſ� ����� ����
	SOCKET client_sock;
	SOCKADDR_IN6 client_addr;
	int addrlen;
	char buf[BUFSIZE];

	while (1)
	{
		// accept()
		addrlen = sizeof(client_addr);
		client_sock = accept(sock, (SOCKADDR *)&client_sock, &addrlen);
		if (client_sock == SOCKET_ERROR) {
			error_display(_T("accept()"));
			break;
		}

		// ������ Ŭ���̾�Ʈ ���� ��� ==> ���Ⱑ ipV6�� �ٸ����̴�..
		TCHAR ipaddr[50];
		DWORD ipaddrlen = sizeof(ipaddr);
		WSAAddressToString((SOCKADDR *)&client_sock, sizeof(client_addr), NULL, ipaddr, &ipaddrlen);
		_tprintf(_T("\n[TCP ����] Ŭ���̾�Ʈ ���� : %s\n"), ipaddr);

		// Ŭ���̾�Ʈ�� ������ ���
		while (1) {
			//������ �ޱ�
			retval = recv(client_sock, buf, BUFSIZE, 0);
			if (retval == SOCKET_ERROR) {
				error_display(_T("recv()"));
				break;
			}
			else if (retval == 0) break;	// '0' �� ��� ���������� ����� ��츦 �ǹ��Ѵ�.
			// ���� ������ ���
			buf[retval] = '\0';
			_tprintf(_T("[TCP/%s] %s\n"), ipaddr, buf);

			// ������ ������
			retval = send(client_sock, buf, BUFSIZE, 0);
			if(retval == SOCKET_ERROR) {
				error_display(_T("send()"));
				break;
			}
		}
		closesocket(client_sock);
	}

	// ���� �ݱ�
	closesocket(sock);
	// ���� ����
	WSACleanup();
	return 0;
}

void error_quit(TCHAR *msg)
{
	LPVOID lpMsgBuf;
	FormatMessage(
		FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
		NULL,
		WSAGetLastError(),
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		(LPTSTR)&lpMsgBuf, 0, NULL);
	MessageBox(NULL, (LPCTSTR)lpMsgBuf, msg, MB_ICONERROR);
	LocalFree(lpMsgBuf);
	exit(1);
}
void error_display(TCHAR *msg)
{
	LPVOID lpMsgBuf;
	FormatMessage(
		FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
		NULL,
		WSAGetLastError(),
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		(LPTSTR)&lpMsgBuf, 0, NULL);
	printf("[%s] %s\n", msg, (char *)lpMsgBuf);
	LocalFree(lpMsgBuf);
}