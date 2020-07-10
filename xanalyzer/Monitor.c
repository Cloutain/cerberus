#include "Monitor.h"
#include "Analyze.h"

#define __XMONITOR_FILE_NAME__		_T("xmonitor_xdetour.dll")

__bool __INTERNAL_FUNC__ Monitor(__tchar *pResultFilePath, PMONITOR_CONFIGURE pConfigure) {
	STARTUPINFO si;
	PROCESS_INFORMATION pi;
	__bool bRet = FALSE;
	__tchar szXMonitorFilePath[MAX_PATH] = {0};

	// ��ȡxmonitor_xdetour.dll��·��
	GetLocalPath(NULL, szXMonitorFilePath);
	__logic_tcscat__(szXMonitorFilePath, __XMONITOR_FILE_NAME__);

	// �������ýṹ����һ��xdetour�������ļ�
	

	// ׼��ִ��
	ZeroMemory( &si, sizeof(si) );
	si.cb = sizeof(si);
	ZeroMemory( &pi, sizeof(pi) );

	if( !CreateProcess( NULL,
		pConfigure->szProgramName,
		NULL,
		NULL,
		FALSE,
		CREATE_SUSPENDED,
		NULL,
		NULL,
		&si,
		&pi )
		) 
	{
		int n = GetLastError();
		return FALSE;
	}

	// ע��
	{
		
		__tchar szTargetProcessName[0x20] = {0};
		_tcscpy(szTargetProcessName, _tcsrchr(pConfigure->szProgramName, _T('\\')) + 1);

		if (InsertRemoteThread(szTargetProcessName, szXMonitorFilePath) == FALSE) {
			bRet = FALSE;
			goto _continue;
		}
	}

_continue:
	ResumeThread( pi.hProcess );

	//WaitForSingleObject( pi.hProcess, INFINITE );
	CloseHandle( pi.hProcess );
	CloseHandle( pi.hThread );
	return bRet;
}
