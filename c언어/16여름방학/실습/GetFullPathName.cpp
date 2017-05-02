/*
	Access, LastWrite, Create정보
	Attribute정보
	WriteFile, ReadFile
	시간정보			==> 핸들을 통해서 얻 
	파일의 절대경로얻기 
*/

#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
#include <tchar.h>

#define STRING_NUM 100

void ShowTime(FILETIME ft);
void ShowAttribute(DWORD attrib);
void ShowFullPath(TCHAR *filename);

int main(){
	TCHAR filename[STRING_NUM] = _T("RealScaleViewer.txt");
	HANDLE hFile = NULL;
	BY_HANDLE_FILE_INFORMATION fileinfo;
	
	hFile = CreateFile(filename, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if(hFile == INVALID_HANDLE_VALUE){
		_tprintf(_T("CreateFile Error !! \n"));
		return -1;
	}
	
	GetFileInformationByHandle(hFile, &fileinfo);
	
	
	_tprintf(_T("CreateTime : "));
	ShowTime(fileinfo.ftCreationTime);
	_tprintf(_T("AccessTime : "));
	ShowTime(fileinfo.ftLastAccessTime);
	_tprintf(_T("LastWriteTime : "));
	ShowTime(fileinfo.ftLastWriteTime);
	//_tprintf(_T("File Attribute : "));
	//ShowAttribute(fileinfo.dwFileAttributes);
	ShowFullPath(filename);
	_tprintf(_T("FileSize : %u"),fileinfo.nFileSizeLow);
	CloseHandle(hFile);
	return 0;
}

void ShowFullPath(TCHAR *filename){
	TCHAR path[STRING_NUM], *filepart= NULL;
	
	GetFullPathName(filename, STRING_NUM, path, &filepart);
	_tprintf("FullPath : %s\n", path);
}
void ShowTime(FILETIME ft){
	SYSTEMTIME st, local;
	TCHAR fileinfo[STRING_NUM] = "\0";
	 
	FileTimeToSystemTime(&ft, &st);
	SystemTimeToTzSpecificLocalTime(NULL, &st, &local);
	
	_stprintf(fileinfo, "%02d/%02d/%d %02d시%02d분\n",
				local.wMonth, local.wDay, local.wYear, local.wHour, local.wMinute);
	///////// print
	
	_tprintf(_T("%s"), fileinfo);
	
	
	return;
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
