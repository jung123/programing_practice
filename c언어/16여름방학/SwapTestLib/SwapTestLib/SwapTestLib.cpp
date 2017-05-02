#include <stdio.h>
#include <tchar.h>
#include "SwapDll.h"
#pragma comment(lib, "SwapDll.lib")

int _tmain(int argc, int* argv[]) {
	int a = 10;
	int b = 20;
	_tprintf(_T("Before Swap() a = %d b = %d \n"), a, b);

	swap(&a, &b);
	_tprintf(_T("After Swap() a = %d b = %d \n"), a, b);
	return 0;
}