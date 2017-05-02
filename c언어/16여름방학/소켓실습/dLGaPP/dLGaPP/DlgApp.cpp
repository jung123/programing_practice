#include "resource.h"
#include <stdio.h>
#include <locale.h>
#include <windows.h>
#include <tchar.h>

#define BUFSIZE 25

BOOL CALLBACK DlgProc(HWND, UINT, WPARAM, LPARAM);

void DisplayText(char *fmt, ...);

HWND hEdit1, hEdit2;

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	_tsetlocale(LC_ALL, _T("korean"));
	DialogBox(hInstance, MAKEINTRESOURCE(IDD_DIALOG1), NULL, DlgProc);
	return 0;
}

BOOL CALLBACK DlgProc(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	static char buf[BUFSIZE + 1];

	switch (uMsg) {
	case WM_INITDIALOG:
		hEdit1 = GetDlgItem(hDlg, IDC_EDIT1);
		hEdit2 = GetDlgItem(hDlg, IDC_EDIT2);
		SendMessage(hEdit1, EM_SETLIMITTEXT, BUFSIZE, 0);
		return TRUE;
	case WM_COMMAND:
		switch (LOWORD(wParam)) {
		case IDOK:
			GetDlgItemText(hDlg, IDC_EDIT1, (LPWSTR)buf, BUFSIZE + 1);
			DisplayText("%s\r\n", buf);
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

void DisplayText(char *fmt, ...)
{
	va_list arg;
	va_start(arg, fmt);

	char cbuf[BUFSIZE + 256];
	vsprintf_s(cbuf,sizeof(cbuf), fmt, arg);

	int nlength = GetWindowTextLength(hEdit2);
	SendMessage(hEdit2, EM_SETSEL, nlength, nlength);
	SendMessage(hEdit2, EM_REPLACESEL, FALSE, (LPARAM)cbuf);
	va_end(arg);
}