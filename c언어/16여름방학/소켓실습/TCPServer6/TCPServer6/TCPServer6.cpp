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
	// 윈속 초기화
	WSADATA wsa;
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) return 1;
	// 소켓생성
	SOCKET sock = socket(AF_INET6, SOCK_STREAM, 0);
	if (sock == INVALID_SOCKET) error_quit(_T("socket()"));
	// bind	ipv6버전이다 다르게 해야한다 !!
	SOCKADDR_IN6 serveraddr;
	ZeroMemory(&serveraddr, sizeof(serveraddr));
	serveraddr.sin6_family = AF_INET6;
	serveraddr.sin6_port = htons(SERVERPORT);
	serveraddr.sin6_addr = in6addr_any;				// in6addr_any 의 뜻을 모르겠다...
	retval = bind(sock, (SOCKADDR *)&serveraddr, sizeof(serveraddr));
	if (retval == SOCKET_ERROR) error_quit(_T("bind()"));
	// listen
	retval = listen(sock, SOMAXCONN);			// backlog는 서버가 당장 처리하지 못하지만 연결을 대기중인 클라이언트의수 (= 대기 큐의 길이)
	if (retval == SOCKET_ERROR) error_quit(_T("listen()"));
	// 데이터 통신에 사용할 변수
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

		// 접속한 클라이언트 정보 출력 ==> 여기가 ipV6의 다른점이다..
		TCHAR ipaddr[50];
		DWORD ipaddrlen = sizeof(ipaddr);
		WSAAddressToString((SOCKADDR *)&client_sock, sizeof(client_addr), NULL, ipaddr, &ipaddrlen);
		_tprintf(_T("\n[TCP 서버] 클라이언트 접속 : %s\n"), ipaddr);

		// 클라이언트와 데이터 통신
		while (1) {
			//데이터 받기
			retval = recv(client_sock, buf, BUFSIZE, 0);
			if (retval == SOCKET_ERROR) {
				error_display(_T("recv()"));
				break;
			}
			else if (retval == 0) break;	// '0' 인 경우 성공적으로 종료된 경우를 의미한다.
			// 받은 데이터 출력
			buf[retval] = '\0';
			_tprintf(_T("[TCP/%s] %s\n"), ipaddr, buf);

			// 데이터 보내기
			retval = send(client_sock, buf, BUFSIZE, 0);
			if(retval == SOCKET_ERROR) {
				error_display(_T("send()"));
				break;
			}
		}
		closesocket(client_sock);
	}

	// 소켓 닫기
	closesocket(sock);
	// 윈속 종료
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