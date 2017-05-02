#pragma comment(lib, "ws2_32")
#include <stdio.h>
#include <windows.h>
#include <tchar.h>

void error_quit(TCHAR *msg);

int _tmain(int argc, TCHAR* argv[]) {
	// ���� �ʱ�ȭ
	WSADATA wsa;
	if(WSAStartup(MAKEWORD(2, 2), &wsa) != 0) return 1;
	// ���� ����
	SOCKET tcp_sock = socket(AF_INET, SOCK_STREAM, 0);
	if (tcp_sock == INVALID_SOCKET) error_quit(_T("socket"));
	//  ipv4 ��ȯ ���� !!
	char *ipv4test = "147.46.114.70";
	printf("IPv4 �ּ�(��ȯ ��) = %s, %x\n", ipv4test, ipv4test);
	//inet_addr test
	printf("IPv4 �ּ�(��ȯ ��) = 0x%x\n", inet_addr(ipv4test));
	// inet_ntoa test	==> inet_ntoa �� ���ڷ� � ������ ���� �ϴ��� ���� �ϴ� ���� �߿��ϴ� ! 32bit���� AND ��Ʈ��ũ ����Ʈ ���Ĺ�� !!
	IN_ADDR ipv4num;
	ipv4num.s_addr = inet_addr(ipv4test);	// host ����Ʈ ���Ĺ���� ip�ּҸ� 32bit ���� �� ����� ������� ������ �Ѵ�.
	printf("IPv4 �ּ�(��ȯ ��) = %s\n", inet_ntoa(ipv4num));	// �� ����� ����� ���ڷ� ����� ���� ���ڷ� ����Ѵ� !!

	/*
		IPv6 �� ��ȯ�ϴ� �� !!
	*/

	// ���ϴݱ�
	closesocket(tcp_sock);
	// ���� ����
	WSACleanup();
	return 0;
}

void error_quit(TCHAR *msg) {
	LPVOID message;
	FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
		NULL,
		WSAGetLastError(),
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		(LPTSTR)&message, 0, NULL);
	MessageBox(NULL, (LPCTSTR)message, msg, MB_ICONERROR);
	LocalFree(message);
	exit(1);
}