#include <stdio.h>
#include <windows.h>
#include <tchar.h>

int _tmain(int argc, TCHAR* argv[]){
	SYSTEM_INFO si;
	DWORD allocGranularity;
	DWORD pageSize;
	
	GetSystemInfo(&si);
	pageSize = si.dwPageSize;
	allocGranularity = si.dwAllocationGranularity;
	
	_tprintf(_T("Page Size : %u Kbyte \n"),pageSize/1024);
	_tprintf(_T("Allocation granularity : %u Kbyte \n"),allocGran+ularity/1024);	
	return 0;
}
