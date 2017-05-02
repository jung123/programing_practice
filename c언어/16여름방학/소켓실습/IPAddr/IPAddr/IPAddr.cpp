#pragma comment(lib, "ws2_32")
#include <stdio.h>
#include <windows.h>
#include <tchar.h>

void error_quit(TCHAR *msg);

int _tmain(int argc, TCHAR* argv[]) {
	// 윈속 초기화
	WSADATA wsa;
	if(WSAStartup(MAKEWORD(2, 2), &wsa) != 0) return 1;
	// 소켓 생성
	SOCKET tcp_sock = socket(AF_INET, SOCK_STREAM, 0);
	if (tcp_sock == INVALID_SOCKET) error_quit(_T("socket"));
	//  ipv4 변환 연습 !!
	char *ipv4test = "147.46.114.70";
	printf("IPv4 주소(변환 전) = %s, %x\n", ipv4test, ipv4test);
	//inet_addr test
	printf("IPv4 주소(변환 후) = 0x%x\n", inet_addr(ipv4test));
	// inet_ntoa test	==> inet_ntoa 의 인자로 어떤 형식이 들어가야 하는지 이해 하는 것이 중요하다 ! 32bit숫자 AND 네트워크 바이트 정렬방식 !!
	IN_ADDR ipv4num;
	ipv4num.s_addr = inet_addr(ipv4test);	// host 바이트 정렬방식의 ip주소를 32bit 숫자 빅 엔디안 방식으로 저장을 한다.
	printf("IPv4 주소(변환 후) = %s\n", inet_ntoa(ipv4num));	// 빅 엔디안 방식의 숫자로 저장된 것을 문자로 출력한다 !!

	/*
		IPv6 의 변환하는 것 !!
	*/

	// 소켓닫기
	closesocket(tcp_sock);
	// 윈속 종료
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