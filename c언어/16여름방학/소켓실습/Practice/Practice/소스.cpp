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
	DialogBox(hInstance, MAKEINTRESOURCE(IDD_DIALOG1), NULL, DlgProc);	// dialog ����ϴ� �Լ�
	return 0;
}
BOOL CALLBACK DlgProc(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam) {
	static TCHAR buff[BUFSIZE + 1];
	
	switch (uMsg) {
	case WM_INITDIALOG:
		hEdit1 = GetDlgItem(hDlg, IDC_EDIT1);
		hEdit2 = GetDlgItem(hDlg, IDC_EDIT2);
		SendMessage(hEdit1, EM_SETLIMITTEXT, BUFSIZE, 0); // ù ��° ��Ʈ���� �ִ� ���ڿ� ���̸� �����Ѵ�. ! => BUFSIZE��ŭ
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

void DisplayText(char *fmt, ...)	// �������ڿ� ���ؼ� ���ݴ� ������ �ʿ䰡 �ִ�. �׸���  SendMessage()�� ���ؼ� ��Ʈ�ѿ� ����� �����ٴ� ����
{									// �˾Ұ�, �پ��� �޽����� �˸� ���� ���ϴ�.
	va_list ag;
	va_start(ag, fmt);
	char buf[BUFSIZE + 256];
	vsprintf_s(buf, sizeof(buf), fmt, ag);
	int nLength = GetWindowTextLength(hEdit2);		// 2��° ��Ʈ�ѿ� ����ϱ� ���� 2��° ��Ʈ���� �ؽ�Ʈ ũ�⸦ �޾ƿ��� �Լ��ε� �ϴ�.
	SendMessage(hEdit2, EM_SETSEL, nLength, nLength);	// 2��° ��Ʈ�ѿ� em_setsel �̶�� �޽����� ������ ��ǹ������� ��
	SendMessage(hEdit2, EM_REPLACESEL, FALSE, (LPARAM)buf);		// replace ����� ���´�.
	va_end(ag);
}