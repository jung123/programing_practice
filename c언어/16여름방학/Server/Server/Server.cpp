#pragma comment(lib, "ws2_32")
#include <stdio.h>
#include <windows.h>
#include <tchar.h>

void err_quit(TCHAR *msg);		// ��Ʈ��ũ ���� ������ �����ڵ尡 �ƴ� ��ü������ �˷��ֱ� ���� �Լ��̴� !!

int main() {
	WSADATA wsa;
	//					���� �ʱ�ȭ
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) return 1;	// �����ʱ�ȭ �Լ� ȣ��
	MessageBox(NULL, _T("���� �ʱ�ȭ ����"), _T("�˶�"), MB_OK);
	// ���� ����
	SOCKET tcp_sock = socket(AF_INET, SOCK_STREAM, 0);
	if (tcp_sock == INVALID_SOCKET) err_quit(_T("socket()"));
	MessageBox(NULL, _T("���� ���� ����"), _T("�˶�"), MB_OK);
	// ���� �ݱ�
	closesocket(tcp_sock);
	// ���� ����
	WSACleanup();

	return 0;
}

void err_quit(TCHAR *msg) {
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