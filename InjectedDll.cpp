#include "windows.h"
#include "tchar.h"

//LP����ָ�롢C�����ɸı䡢T��������Ƿ���UNICODE����ֱ�defineΪchar��wchar_t��STR�����ַ�����
BOOL InjectDll(DWORD dwPID, LPCTSTR szDllPath)
{
	HANDLE hProcess = NULL, hThread = NULL;
	HMODULE hMod = NULL;
	LPVOID pRemoteBuf = NULL;
	DWORD dwBufSize = (DWORD)(_tcslen(szDllPath) + 1) * sizeof(TCHAR);
	//LPTHREAD_START_ROUTINE:ָ��һ���������ú���֪ͨ����ĳ���߳��ѿ�ʼִ��,��ʾԶ���߳����̵߳���ʼ��ַ
	LPTHREAD_START_ROUTINE pThreadProc;

	//dwPID��ȡ���̾��
	if (!(hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, dwPID)))
	{
		_tprintf(_T("Open Process(%d) failed!\n"), dwPID);
		GetLastError();
		return FALSE;
	}

	//�����ڴ�
	//MEM_COMMIT��Ϊ�ض���ҳ����������ڴ��л���̵�ҳ���ļ��е�����洢
	//PAGE_READWRITE: ����ɱ�Ӧ�ó����д
	pRemoteBuf = VirtualAllocEx(hProcess, NULL, dwBufSize, MEM_COMMIT, PAGE_READWRITE);

	//д��dll�ļ����ڷ�����ڴ�
	WriteProcessMemory(hProcess, pRemoteBuf, (LPVOID)szDllPath, dwBufSize, NULL);
	_tprintf(L"%s\n", szDllPath);

	//��ȡLoadLibraryW API��ַ
	//��ЩAPI��������A��W,A��ANSI�汾,W����UNICODE�汾
	hMod = GetModuleHandle(_T("kernel32.dll"));
	//Զ���̵߳���ʼ��ַ
	pThreadProc = (LPTHREAD_START_ROUTINE)GetProcAddress(hMod, "LoadLibraryW");
	_tprintf(L"%x\n", pThreadProc);

	//��notepad�������߳�
	hThread = CreateRemoteThread(hProcess, NULL, 0, pThreadProc,					//�̵߳���ʼ��ַ����
		pRemoteBuf,					//ָ��Ҫ���ݸ��̺߳����ı�����ָ��
		0, NULL);
	_tprintf(L"%x\n", (unsigned long long)hThread);
	//�ȴ�ֱ��ָ���Ķ��󷢳��źŻ�ʱ���
	WaitForSingleObject(hThread, INFINITE);
	CloseHandle(hThread);
	CloseHandle(hProcess);

	return TRUE;
}

int _tmain(int argc, TCHAR* argv[])
{
	if (argc != 3)
	{
		_tprintf(L"%s pid dll_path\n", argv[0]);
		return 1;
	}

	//inject dll
	//atol ���ַ���ת���ɳ���������
	if (InjectDll((DWORD)_tstol(argv[1]), argv[2]))
		_tprintf(L"Inject \"%s\" success!\n", argv[2]);
	else
		_tprintf(L"Inject \"%s\" failed!\n", argv[2]);

	return 0;
}