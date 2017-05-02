#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
#include <tchar.h>

void ShowAttribute(DWORD attrib);
void ShowFileTime(FILETIME ft);

int _tmain(int argc, TCHAR* argv[]){
	
	TCHAR filename[] = _T("ReadScaleView.txt");
	
	HANDLE hFile = NULL;
	
	BY_HANDLE_FILE_INFORMATION fileinfo;
	
	hFile = CreateFile(filename, GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if(hFile == INVALID_HANDLE_VALUE){
		_tprintf(_T("CreateFile Error !! \n"));
		return -1;
	}
	GetFileInformationByHandle(hFile, &fileinfo);
	
	_tprintf(_T("FileSize : %d\n"), fileinfo.nFileSizeLow);
	
	_tprintf(_T("File Create Time : "));
	ShowFileTime(fileinfo.ftCreationTime);
	_tprintf(_T("File Access Time : "));
	ShowFileTime(fileinfo.ftLastAccessTime);
	_tprintf(_T("File LastWrite Time : "));
	ShowFileTime(fileinfo.ftLastWriteTime);
	
	CloseHandle(hFile);
	return 0;
}

void ShowAttribute(DWORD attrib){
	if(attrib & FILE_ATTRIBUTE_NORMAL){
		_tprintf(_T("Normal \n"));
	}else{
		if(attrib & FILE_ATTRIBUTE_HIDDEN){
			_tprintf(_T("Hidden \n"));
		}
		if(attrib & FILE_ATTRIBUTE_READONLY){
			_tprintf(_T("ReadOnly \n"));
		}
	}
	
	return ;
}
void ShowFileTime(FILETIME ft){
	SYSTEMTIME st, local;
	TCHAR fileinfo[100] = "\0";
	
	FileTimeToSystemTime(&ft, &st);
	SystemTimeToTzSpecificLocalTime(NULL, &st, &local);
	
	_stprintf(fileinfo, "%02d/%02d/%d %02d : %02d", local.wMonth, local.wDay, local.wYear, local.wHour, local.wMinute);
	
	_tprintf(_T("%s"), fileinfo);
	return ;
}
