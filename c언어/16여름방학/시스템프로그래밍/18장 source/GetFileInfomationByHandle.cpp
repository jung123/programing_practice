#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <windows.h>
#include <tchar.h>

void ShowAttributes(DWORD attrib);
void ShowFileTime(FILETIME ftTime);

int _tmain(int argc, TCHAR* argv[]){
	TCHAR filename[] = _T("asd.txt");
	HANDLE hFile = NULL;
	
	BY_HANDLE_FILE_INFORMATION fileinfo;
	
	hFile = CreateFile(filename, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if(hFile == INVALID_HANDLE_VALUE){
		_tprintf(_T("CreateFile Error !!! \n"));
		return -1;
	}
	
	GetFileInformationByHandle(hFile, &fileinfo);
	
	ShowAttributes(fileinfo.dwFileAttributes);
	
	_tprintf(_T("file Creation Time : "));
	ShowFileTime(fileinfo.ftCreationTime);
	_tprintf(_T("File Access Time : "));
	ShowFileTime(fileinfo.ftLastAccessTime);
	_tprintf(_T("File Write Time : "));
	ShowFileTime(fileinfo.ftLastWriteTime);
	
	CloseHandle(hFile);
	return 0;
}

void ShowAttributes(DWORD attrib){
	if(attrib & FILE_ATTRIBUTE_NORMAL){
		_tprintf(_T("Normal \n"));
	}else{
		if(attrib & FILE_ATTRIBUTE_HIDDEN){
			_tprintf(_T("Hidden\n"));
		}
		if(attrib & FILE_ATTRIBUTE_READONLY){
			_tprintf(_T("ReadOnly\n"));
		}
	}
	
}

void ShowFileTime(FILETIME ftTime){
	SYSTEMTIME stSystem, stspecialtime;
	TCHAR filetimeinfo[100] = "\0";
	
	FileTimeToSystemTime(&ftTime, &stSystem);
	SystemTimeToTzSpecificLocalTime(NULL, &stSystem, &stspecialtime);
	
	_stprintf(filetimeinfo, _T("%02d/%02d/%d %02d:%02d\n"),
					stspecialtime.wMonth, stspecialtime.wDay,stspecialtime.wYear,
					stspecialtime.wHour, stspecialtime.wMinute);
	_tprintf(_T("%s"),filetimeinfo);
}
