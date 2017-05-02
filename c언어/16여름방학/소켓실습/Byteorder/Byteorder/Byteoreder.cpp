#pragma comment(lib,"ws2_32")
#include <stdio.h>
#include <windows.h>
#include <tchar.h>

void error_quit(TCHAR *msg);

int _tmain(int argc, TCHAR* argv[])
{
	int result = 0;
	// 윈속 초기화
	WSADATA wsa;
	result = WSAStartup(MAKEWORD(2, 2), &wsa);
	if (result != 0) return 1;
	// 소켓 생성
	SOCKET tcp_sock = socket(AF_INET, SOCK_STREAM, 0);
	if (tcp_sock == INVALID_SOCKET) {
		error_quit(_T("SOCKET()"));
	}
	// 바이트오더 함수
	u_short x1 = 0x1234;
	u_long y1 = 0x12345678;
	u_short x2;
	u_long y2;
	// host -> network byte
	_tprintf(_T("\n host -> network!! \n"));
	_tprintf(_T("0x%x -> 0x%x\n"), x1, x2 = htons(x1));
	_tprintf(_T("0x%x -> 0x%x\n"), y1, y2 = htonl(y1));

	// 소켓닫기
	closesocket(tcp_sock);
	// 윈속 종료
	WSACleanup();
	return 0;
}

void error_quit(TCHAR *msg) {
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