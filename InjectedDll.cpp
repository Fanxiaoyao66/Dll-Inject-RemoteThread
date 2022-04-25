#include "windows.h"
#include "tchar.h"

//LP代表指针、C代表不可改变、T代表根据是否定义UNICODE宏而分别define为char或wchar_t、STR代表字符串。
BOOL InjectDll(DWORD dwPID, LPCTSTR szDllPath)
{
	HANDLE hProcess = NULL, hThread = NULL;
	HMODULE hMod = NULL;
	LPVOID pRemoteBuf = NULL;
	DWORD dwBufSize = (DWORD)(_tcslen(szDllPath) + 1) * sizeof(TCHAR);
	//LPTHREAD_START_ROUTINE:指向一个函数，该函数通知宿主某个线程已开始执行,表示远程线程中线程的起始地址
	LPTHREAD_START_ROUTINE pThreadProc;

	//dwPID获取进程句柄
	if (!(hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, dwPID)))
	{
		_tprintf(_T("Open Process(%d) failed!\n"), dwPID);
		GetLastError();
		return FALSE;
	}

	//分配内存
	//MEM_COMMIT：为特定的页面区域分配内存中或磁盘的页面文件中的物理存储
	//PAGE_READWRITE: 区域可被应用程序读写
	pRemoteBuf = VirtualAllocEx(hProcess, NULL, dwBufSize, MEM_COMMIT, PAGE_READWRITE);

	//写入dll文件名于分配的内存
	WriteProcessMemory(hProcess, pRemoteBuf, (LPVOID)szDllPath, dwBufSize, NULL);
	_tprintf(L"%s\n", szDllPath);

	//获取LoadLibraryW API地址
	//有些API后面会带有A或W,A是ANSI版本,W则是UNICODE版本
	hMod = GetModuleHandle(_T("kernel32.dll"));
	//远程线程的起始地址
	pThreadProc = (LPTHREAD_START_ROUTINE)GetProcAddress(hMod, "LoadLibraryW");
	_tprintf(L"%x\n", pThreadProc);

	//在notepad中运行线程
	hThread = CreateRemoteThread(hProcess, NULL, 0, pThreadProc,					//线程的起始地址函数
		pRemoteBuf,					//指向要传递给线程函数的变量的指针
		0, NULL);
	_tprintf(L"%x\n", (unsigned long long)hThread);
	//等待直到指定的对象发出信号或超时间隔
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
	//atol 把字符串转换成长整型数用
	if (InjectDll((DWORD)_tstol(argv[1]), argv[2]))
		_tprintf(L"Inject \"%s\" success!\n", argv[2]);
	else
		_tprintf(L"Inject \"%s\" failed!\n", argv[2]);

	return 0;
}