#pragma comment(lib, "ws2_32")
#include <stdio.h>
#include <process.h>
#include <windows.h>
#include <stdlib.h>
#include <locale.h>
#include <tchar.h>
#include "resource.h"

#define SERVERPORT 9000
#define SERVERIP "127.0.0.1"
#define BUFSIZE 512


// Dialog 프로시져
BOOL CALLBACK DlgProc(HWND dHwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
void DisplayText(TCHAR *fmt, ...);
void error_quit(TCHAR *msg);
void error_display(TCHAR *msg);
int recvn(SOCKET s, char *buf, int len, int flags);
unsigned int WINAPI clientMain(LPVOID arg);

SOCKET sock;
TCHAR buf[BUFSIZE + 1];
HWND hSendButton;
HWND hEdit1, hEdit2;
HANDLE hWriteEvent, hReadEvent;


int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, char *nCmdLine, int nCmdShow)
{
	// 이벤트 생성
	hWriteEvent = CreateEvent(NULL, FALSE, FALSE, NULL);	// 쓰레드가 버퍼에 있는 데이터를 전송한 후 사용자가 메시지를 입력하게 하는 순서
	hReadEvent = CreateEvent(NULL, FALSE, TRUE,NULL);					// 사용자가 메시지를 입력한 후 쓰레드가 버퍼의 내용을 읽어 전송하는 순서
	if ((hWriteEvent == NULL) && (hReadEvent == NULL)) return 1;	


	// 쓰레드 생성
	HANDLE hThread = (HANDLE)_beginthreadex(NULL, 0, clientMain, NULL, 0, NULL);
	if (hThread == INVALID_HANDLE_VALUE) return 1;
	// dialog 생성
	DialogBox(hInstance, MAKEINTRESOURCE(IDD_DIALOG1), NULL, DlgProc);
	CloseHandle(hEdit1);
	CloseHandle(hEdit2);
	closesocket(sock);
	WSACleanup();
	return 0;
}

BOOL CALLBACK DlgProc(HWND dHwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{ 
	switch (uMsg) {
	case WM_INITDIALOG:
		hEdit1 = GetDlgItem(dHwnd, IDC_EDIT1);
		hEdit2 = GetDlgItem(dHwnd, IDC_EDIT2);
		hSendButton = GetDlgItem(dHwnd, IDOK);
		SendMessage(hEdit1, EM_SETLIMITTEXT, BUFSIZE, 0);
		return TRUE;
	case WM_COMMAND:
		switch (LOWORD(wParam)) {
		case IDOK:
			EnableWindow(hSendButton, FALSE);
			WaitForSingleObject(hReadEvent, INFINITE);			// read 이벤트 signaled 기다리기
			GetDlgItemText(dHwnd, IDC_EDIT1, buf, BUFSIZE + 1);
			SetEvent(hWriteEvent);								// wrtie 이벤트 set !!
			SetFocus(hEdit1);
			SendMessage(hEdit1, EM_SETSEL, 0, -1);
			return TRUE;
		case IDCANCEL:
			EndDialog(dHwnd, IDCANCEL);
			return TRUE;
		}
		return FALSE;
	}
	return FALSE;
}
void error_quit(TCHAR *msg)
{
	LPVOID lpMsgBuf = NULL;
	FormatMessage(
		FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
		NULL, WSAGetLastError(), MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (TCHAR *)lpMsgBuf, 0, NULL);
	MessageBox(NULL, (TCHAR *)lpMsgBuf, msg, MB_ICONERROR);
	LocalFree(lpMsgBuf);
	exit(1);
}
// 에러 디스플레이
void error_display(TCHAR *msg)
{
	LPVOID lpMsgBuf = NULL;
	FormatMessage(
		FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
		NULL, WSAGetLastError(), MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (TCHAR *)lpMsgBuf, 0, NULL);
	DisplayText(_T("[%s]%s"), msg, lpMsgBuf);
	LocalFree(lpMsgBuf);
}

int recvn(SOCKET s, char *buf, int len, int flags)
{
	int received;
	TCHAR *ptr = (TCHAR *)buf;
	int left = len;

	while (left > 0)
	{
		received = recv(s, (char *)buf, left, flags);
		if (received == SOCKET_ERROR) return SOCKET_ERROR;
		else if (received == 0) break;
		left -= received;
		ptr += received;
	}
	return (len - left);
}
// 네트워크 통신 쓰레드이다 !
unsigned int WINAPI clientMain(LPVOID arg)
{
	int retval;
	WSADATA wsa;
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) return 1;

	sock = socket(AF_INET, SOCK_STREAM, 0);
	if (sock == INVALID_SOCKET) error_quit(_T("socket()"));

	SOCKADDR_IN serveraddr;
	memset((void *)&serveraddr, 0, sizeof(serveraddr));
	serveraddr.sin_family = AF_INET;
	serveraddr.sin_addr.s_addr = inet_addr(SERVERIP);
	serveraddr.sin_port = htons(SERVERPORT);
	
	retval = connect(sock, (SOCKADDR *)&serveraddr, sizeof(serveraddr));
	if (retval == SOCKET_ERROR) error_quit(_T("connect()"));

	while(1)
	{
		WaitForSingleObject(hWriteEvent, INFINITE);
		if (strlen((char *)buf) == 0) {
			EnableWindow(hSendButton, TRUE);
			SetEvent(hReadEvent);
			continue;
		}

		retval = send(sock, (char *)buf, strlen((char *)buf), 0);
		if (retval == SOCKET_ERROR) {
			error_display(_T("send()"));
			continue;
		}
		DisplayText(_T("[TCP 클라이언트] %d바이트를 보냈습니다..\r\n"), retval);

		retval = recvn(sock, (char *)buf, retval, 0);
		if (retval == SOCKET_ERROR) {
			error_display(_T("recv()"));
			break;
		}
		else if (retval == 0) break;

		buf[retval] = '\0';
		DisplayText(_T("[TCP 클라이언트] %d바이트를 받았습니다.\r\n"), retval);
		DisplayText(_T("[받은 데이터] %s\r\n"), buf);
		
		EnableWindow(hSendButton, TRUE);
		SetEvent(hReadEvent);
	}
	return 0;
}
void DisplayText(TCHAR *fmt, ...) {
	va_list arg;
	va_start(arg, fmt);

	TCHAR cbuf[BUFSIZE + 256];
	_vstprintf_s(cbuf, sizeof(cbuf), fmt, arg);

	int nLength = GetWindowTextLength(hEdit2);
	SendMessage(hEdit2, EM_SETSEL, nLength, nLength);
	SendMessage(hEdit2, EM_REPLACESEL, FALSE, (LPARAM)cbuf);

	va_end(arg);
}