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


// Dialog ���ν���
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
	// �̺�Ʈ ����
	hWriteEvent = CreateEvent(NULL, FALSE, FALSE, NULL);	// �����尡 ���ۿ� �ִ� �����͸� ������ �� ����ڰ� �޽����� �Է��ϰ� �ϴ� ����
	hReadEvent = CreateEvent(NULL, FALSE, TRUE,NULL);					// ����ڰ� �޽����� �Է��� �� �����尡 ������ ������ �о� �����ϴ� ����
	if ((hWriteEvent == NULL) && (hReadEvent == NULL)) return 1;	


	// ������ ����
	HANDLE hThread = (HANDLE)_beginthreadex(NULL, 0, clientMain, NULL, 0, NULL);
	if (hThread == INVALID_HANDLE_VALUE) return 1;
	// dialog ����
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
			WaitForSingleObject(hReadEvent, INFINITE);			// read �̺�Ʈ signaled ��ٸ���
			GetDlgItemText(dHwnd, IDC_EDIT1, buf, BUFSIZE + 1);
			SetEvent(hWriteEvent);								// wrtie �̺�Ʈ set !!
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
// ���� ���÷���
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
// ��Ʈ��ũ ��� �������̴� !
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
		DisplayText(_T("[TCP Ŭ���̾�Ʈ] %d����Ʈ�� ���½��ϴ�..\r\n"), retval);

		retval = recvn(sock, (char *)buf, retval, 0);
		if (retval == SOCKET_ERROR) {
			error_display(_T("recv()"));
			break;
		}
		else if (retval == 0) break;

		buf[retval] = '\0';
		DisplayText(_T("[TCP Ŭ���̾�Ʈ] %d����Ʈ�� �޾ҽ��ϴ�.\r\n"), retval);
		DisplayText(_T("[���� ������] %s\r\n"), buf);
		
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