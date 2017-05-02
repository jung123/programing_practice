#include <stdio.h>
#include <windows.h>
#include <tchar.h>
#include <process.h>
typedef void(*WORK) (void);

HANDLE semaphore;

unsigned int WINAPI ThreadProc(LPVOID lpparam) {
	WaitForSingleObject(semaphore, INFINITE);
	_tprintf(_T("TEST TEST!! \n"));
	ReleaseSemaphore(semaphore, 1, NULL);
	return 0;
}

int main() {
	HANDLE hThread;
	DWORD dwThreadId;
	WORK test;

	semaphore = CreateSemaphore(NULL, 5, 5, NULL);

	hThread = (HANDLE)_beginthreadex(NULL, 0, ThreadProc, NULL, 0, (unsigned *)&dwThreadId);
	if (hThread == NULL) {
		_tprintf(_T("Thread Creation Fault !! \n"));
		return -1;
	}

	if (semaphore == NULL) {
		_tprintf(_T("semaphore Creation Fail !! \n"));
	}
	WaitForSingleObject(hThread, INFINITE);
	CloseHandle(hThread);
	CloseHandle(semaphore);
	
	Sleep(50000);

	return 0;
}