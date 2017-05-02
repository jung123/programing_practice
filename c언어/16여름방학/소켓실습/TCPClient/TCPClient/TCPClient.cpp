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
	int retval;		// 결과값을 저장하는데 유용하게 쓰인다
	_tsetlocale(LC_ALL, _T("Korean"));
	// 윈속 초기화
	WSADATA wsa;
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) return 1;
	// 소켓 생성
	SOCKET sock = socket(AF_INET, SOCK_STREAM, 0);
	if (sock == INVALID_SOCKET) error_quit(_T("socket()"));
	
	// connect()
	SOCKADDR_IN serveraddr;
	ZeroMemory(&serveraddr, sizeof(serveraddr));
	serveraddr.sin_family = AF_INET;
	serveraddr.sin_port = htons(SERVERPORT);
	serveraddr.sin_addr.s_addr = inet_addr(SERVERIP);	// inet_addr은 문자열 형태의 ip주소를 네트워크 바이트 정렬형식의 32bit 숫자로 바꾸어주는 함수
	retval = connect(sock, (SOCKADDR *)&serveraddr, sizeof(serveraddr));
	if (retval == SOCKET_ERROR) error_quit(_T("connect"));

	// 데이터 통신에 사용할 변수
	char buf[BUFSIZE + 1];
	int len;

	// 서버와 데이터 동신
	while (1)
	{
		_tprintf(_T("\n [보낼 데이터] "));
		if (fgets(buf, sizeof(buf), stdin) == NULL) break;

		// '\n' 제거
		len = strlen(buf);
		if (buf[len - 1] == '\n') buf[len - 1] == '\0';
		if (strlen(buf) == 0) break;

		// 데이터 보내기
		retval = send(sock, buf, strlen(buf), 0); // sned 함수는 보낸 데이터의 크기를 return 한다. 
		if (retval == SOCKET_ERROR) {	
			error_display(_T("send()"));
			break;
		}
		_tprintf(_T(" [TCP 클라이언트] %d바이트를 보냈습니다.\n"), retval);

		// 데티어 받기
		retval = recvn(sock, buf, retval, 0);
		if (retval == SOCKET_ERROR) {
			error_display(_T("recv()"));
			break;
		}
		else if (retval == 0) break;	// 데이터를 받지 않았을 때의 경우인듯하다.
		
		// 받은 데이터 출력
	//	buf[retval] = '\0';			// 데이터를 받은 후 데이터의 마지막을 문자열의 끝을 넣어줌
		_tprintf(_T("[TCP 클라이언트] %d바이트를 받았습니다.\n"), retval);
		printf("[받은 데이터] %s", buf);
	}

	// 소켓닫기
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
	MessageBox(NULL, (LPCTSTR)lpMsgBuf, msg, MB_ICONERROR);
	LocalFree(lpMsgBuf);
}
int recvn(SOCKET s, char *buf, int len, int flags)	// len에는 보낼 데이터의 양을 받아서 저장함
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