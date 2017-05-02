#include <stdio.h>
#include <tchar.h>
#include <windows.h>

int _tmain(int arc, TCHAR* argv[]){
	TCHAR filename[] = _T("RealScaleViewer.txt");
	
	DWORD fileSize = 0, dwReturn = 0;
	HANDLE hFile = NULL;
	
	
	hFile = CreateFile(filename, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if(hFile == INVALID_HANDLE_VALUE){
		_tprintf(_T("Create File Error !! \n"));
		return -1;
	}
	dwReturn = GetFileSize(hFile, &fileSize);
	if(dwReturn == INVALID_FILE_SIZE){
		_tprintf(_T("GetFileSize Error !!! \n"));
		return -1;
	}
	
	_tprintf(_T("%s's Filesize : %d \n'"), filename, fileSize);
	
	CloseHandle(hFile);
	return 0;
}
