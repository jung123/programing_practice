#pragma comment(lib, "ws2_32")
#include <stdio.h>
#include <windows.h>
#include <tchar.h>
#include <locale.h>

#define SERVERPORT 9000
#define BUFSIZE 512

void error_quit(TCHAR *msg);
void error_display(TCHAR *msg);

int _tmain(int argc, TCHAR* argv[]) {
	int retval;

	_tsetlocale(LC_ALL, _T("Korean"));

	// 윈속 초기화
	WSADATA wsa;
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) error_quit(_T("WSAStartUp"));
	// 소켓생성
	SOCKET listen_sock = socket(AF_INET, SOCK_STREAM, 0);
	if (listen_sock == INVALID_SOCKET) {_tsetlocale(LC_ALL, _T("Korean")); 
		error_quit(_T("socket() | LISTEN_SOCK"));
	}
	// bind()
	SOCKADDR_IN serveraddr;
	ZeroMemory(&serveraddr, sizeof(serveraddr));
	serveraddr.sin_family = AF_INET;
	serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);		// (u_long)0x00000000
	serveraddr.sin_port = htons(SERVERPORT);
	retval = bind(listen_sock, (SOCKADDR *)&serveraddr, sizeof(serveraddr)); // somaxconn = 5 로 정의되어 있다.
	if (retval == SOCKET_ERROR) error_quit(_T("bind()"));
	// listen()
	retval = listen(listen_sock, SOMAXCONN);
	if (retval == SOCKET_ERROR) error_quit(_T("listen()"));
	// 데이터 동신에 사용할 변수
	SOCKET client_sock;
	SOCKADDR_IN clientaddr;
	int addrlen;
	char buf[BUFSIZE + 1] = "\0";

	while (1)
	{
		// accept()
		addrlen = sizeof(clientaddr);
		client_sock = accept(listen_sock, (SOCKADDR *)&clientaddr, &addrlen);
		if (client_sock == INVALID_SOCKET)
		{
			error_display(_T("accept()"));
			break;
		}
		// 성공적으로 client와 접속을 맺음 -> client 정보 출력
		printf("\n[TCP 서버] 클라이언트 접속 : IP 주소=%s. 포트 번호 = %d\n",
			inet_ntoa(clientaddr.sin_addr), ntohs(clientaddr.sin_port));
		// 클라이언트와 데이터 통신
		while (1)
		{
			retval = recv(client_sock, buf, sizeof(buf), 0);
			if (retval == SOCKET_ERROR) {
				error_display(_T("recv()"));
				break;
			}
			else if (retval == 0) break;		// retval 에는 recv() 함수로 클라이언트에서 받은 데이터의 크기를 저장
			// 받은 데이터 출력
			buf[retval] = '\0'; // 버퍼의 마지막에 문자열 끝을 지정함
			_tprintf(_T("[TCP %s:%d] %s\n"), inet_ntoa(clientaddr.sin_addr), ntohs(clientaddr.sin_port), buf);
			// 데이터 보내기
			retval = send(client_sock, buf, strlen(buf), 0);
			if (retval == SOCKET_ERROR) {
				error_display(_T("send()"));
				break;
			}
		}
		closesocket(client_sock);
		_tprintf(_T("[TCP 서버] 클라이언트 종료 [%s : %d]\n"), inet_ntoa(clientaddr.sin_addr), ntohs(clientaddr.sin_port));
	}
	// listen_socket 종료
	closesocket(listen_sock);
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
	MessageBox(NULL, (LPCTSTR)lpMsgBuf, msg, MB_ICONERROR);
	LocalFree(lpMsgBuf);
}