#pragma comment(lib, "ws2_32")
#include <stdio.h>
#include <windows.h>
#include <tchar.h>
#include <locale.h>

#define SERVERPORT 9000
#define SERVERIP "127.0.0.1"
#define BUFSIZE 512

void error_quit(TCHAR *msg);
void error_display(TCHAR *msg);
int recvn(SOCKET s, char *buf, int len, int flags);

int _tmain(int argc, TCHAR* rgv[]) {
	int retval;		// ������� �����ϴµ� �����ϰ� ���δ�
	_tsetlocale(LC_ALL, _T("Korean"));
	// ���� �ʱ�ȭ
	WSADATA wsa;
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) return 1;
	// ���� ����
	SOCKET sock = socket(AF_INET, SOCK_STREAM, 0);
	if (sock == INVALID_SOCKET) error_quit(_T("socket()"));
	
	// connect()
	SOCKADDR_IN serveraddr;
	ZeroMemory(&serveraddr, sizeof(serveraddr));
	serveraddr.sin_family = AF_INET;
	serveraddr.sin_port = htons(SERVERPORT);
	serveraddr.sin_addr.s_addr = inet_addr(SERVERIP);	// inet_addr�� ���ڿ� ������ ip�ּҸ� ��Ʈ��ũ ����Ʈ ���������� 32bit ���ڷ� �ٲپ��ִ� �Լ�
	retval = connect(sock, (SOCKADDR *)&serveraddr, sizeof(serveraddr));
	if (retval == SOCKET_ERROR) error_quit(_T("connect"));

	// ������ ��ſ� ����� ����
	char buf[BUFSIZE + 1];
	int len;

	// ������ ������ ����
	while (1)
	{
		_tprintf(_T("\n [���� ������] "));
		if (fgets(buf, sizeof(buf), stdin) == NULL) break;

		// '\n' ����
		len = strlen(buf);
		if (buf[len - 1] == '\n') buf[len - 1] == '\0';
		if (strlen(buf) == 0) break;

		// ������ ������
		retval = send(sock, buf, strlen(buf), 0); // sned �Լ��� ���� �������� ũ�⸦ return �Ѵ�. 
		if (retval == SOCKET_ERROR) {	
			error_display(_T("send()"));
			break;
		}
		_tprintf(_T(" [TCP Ŭ���̾�Ʈ] %d����Ʈ�� ���½��ϴ�.\n"), retval);

		// ��Ƽ�� �ޱ�
		retval = recvn(sock, buf, retval, 0);
		if (retval == SOCKET_ERROR) {
			error_display(_T("recv()"));
			break;
		}
		else if (retval == 0) break;	// �����͸� ���� �ʾ��� ���� ����ε��ϴ�.
		
		// ���� ������ ���
	//	buf[retval] = '\0';			// �����͸� ���� �� �������� �������� ���ڿ��� ���� �־���
		_tprintf(_T("[TCP Ŭ���̾�Ʈ] %d����Ʈ�� �޾ҽ��ϴ�.\n"), retval);
		printf("[���� ������] %s", buf);
	}

	// ���ϴݱ�
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
	MessageBox(NULL, (LPCTSTR)lpMsgBuf, msg, MB_ICONERROR);
	LocalFree(lpMsgBuf);
}
int recvn(SOCKET s, char *buf, int len, int flags)	// len���� ���� �������� ���� �޾Ƽ� ������
{
	int received;
	char *ptr = buf;
	int left = len;

	while (left > 0) {
		received = recv(s, buf, left, flags);
		if (received == SOCKET_ERROR) return SOCKET_ERROR;
		else if (received == 0) break;
		left -= received;
		ptr += received;
	}
	return (len - left);
}