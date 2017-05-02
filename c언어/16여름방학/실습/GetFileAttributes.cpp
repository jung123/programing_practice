#include <stdio.h>
#include <windows.h>
#include <tchar.h>


void ShowAttributes(DWORD attrib){
	if(attrib & FILE_ATTRIBUTE_NORMAL){
		_tprintf(_T("Normal.\n"));
	}else{
		if(attrib & FILE_ATTRIBUTE_READONLY){
		_tprintf(_T("Read Only\n"));
		}
		if(attrib & FILE_ATTRIBUTE_HIDDEN){
			_tprintf(_T("Hidden \n"));
		}	
	}
	return ;
}

int _tmain(int argc, TCHAR* argv[]){
	TCHAR filename[] = _T("RealScaleView.txt");
	DWORD fileSize = 0, dwResult = 0;
	DWORD fileAttrib;
	HANDLE hFile = NULL;
	
	
	fileAttrib = GetFileAttributes(filename);
	ShowAttributes(fileAttrib);
	_tprintf(_T("\n\n"));
	fileAttrib |= (FILE_ATTRIBUTE_READONLY | FILE_ATTRIBUTE_HIDDEN);
	SetFileAttributes(filename, fileAttrib);
	ShowAttributes(fileAttrib);
	
	
	CloseHandle(hFile);
	return 0;
}
