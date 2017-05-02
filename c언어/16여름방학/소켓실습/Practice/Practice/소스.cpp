#include <stdio.h>
#include <windows.h>
#include <tchar.h>
#include <locale.h>
#include "resource.h"

#define BUFSIZE 25

BOOL CALLBACK DlgProc(HWND, UINT, WPARAM, LPARAM);

void DisplayText(char *fmt, ...);

HWND hEdit1, hEdit2;

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR nCmdLine, int nCmdShow)
{
	DialogBox(hInstance, MAKEINTRESOURCE(IDD_DIALOG1), NULL, DlgProc);	// dialog 출력하는 함수
	return 0;
}
BOOL CALLBACK DlgProc(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam) {
	static TCHAR buff[BUFSIZE + 1];
	
	switch (uMsg) {
	case WM_INITDIALOG:
		hEdit1 = GetDlgItem(hDlg, IDC_EDIT1);
		hEdit2 = GetDlgItem(hDlg, IDC_EDIT2);
		SendMessage(hEdit1, EM_SETLIMITTEXT, BUFSIZE, 0); // 첫 번째 컨트롤의 최대 문자열 길이를 지정한다. ! => BUFSIZE만큼
		return TRUE;
	case WM_COMMAND:
		switch (LOWORD(wParam)) {
		case IDOK:
			GetDlgItemText(hDlg, IDC_EDIT1, buff, BUFSIZE + 1);
			DisplayText("%s\r\n", buff);
			SendMessage(hEdit1, EM_SETSEL, 0, -1);
			return TRUE;
		case IDCANCEL:
			EndDialog(hDlg, IDCANCEL);
			return TRUE;
		}
		return FALSE;
	}
	return FALSE;
}

void DisplayText(char *fmt, ...)	// 가변인자에 대해서 조금더 공부할 필요가 있다. 그리고  SendMessage()를 통해서 컨트롤에 명령을 보낸다는 것을
{									// 알았고, 다양한 메시지를 알면 좋을 듯하다.
	va_list ag;
	va_start(ag, fmt);
	char buf[BUFSIZE + 256];
	vsprintf_s(buf, sizeof(buf), fmt, ag);
	int nLength = GetWindowTextLength(hEdit2);		// 2번째 컨트롤에 출력하기 위해 2번째 컨트롤의 텍스트 크기를 받아오는 함수인듯 하다.
	SendMessage(hEdit2, EM_SETSEL, nLength, nLength);	// 2번째 컨트롤에 em_setsel 이라는 메시지를 보냇다 어떤의미인지는 모름
	SendMessage(hEdit2, EM_REPLACESEL, FALSE, (LPARAM)buf);		// replace 명령을 보냈다.
	va_end(ag);
}