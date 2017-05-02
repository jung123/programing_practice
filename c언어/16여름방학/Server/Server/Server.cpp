#pragma comment(lib, "ws2_32")
#include <stdio.h>
#include <windows.h>
#include <tchar.h>

void err_quit(TCHAR *msg);		// 네트워크 상의 오류를 오류코드가 아닌 구체적으로 알려주기 위한 함수이다 !!

int main() {
	WSADATA wsa;
	//					윈속 초기화
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) return 1;	// 윈속초기화 함수 호출
	MessageBox(NULL, _T("윈속 초기화 성공"), _T("알람"), MB_OK);
	// 소켓 생성
	SOCKET tcp_sock = socket(AF_INET, SOCK_STREAM, 0);
	if (tcp_sock == INVALID_SOCKET) err_quit(_T("socket()"));
	MessageBox(NULL, _T("소켓 생성 성공"), _T("알람"), MB_OK);
	// 소켓 닫기
	closesocket(tcp_sock);
	// 윈속 종료
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