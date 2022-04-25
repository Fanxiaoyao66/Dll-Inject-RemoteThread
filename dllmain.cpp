// dllmain.cpp : 定义 DLL 应用程序的入口点。
#include "pch.h"
#include "windows.h"
#include "tchar.h"
#include "urlmon.h"

#pragma comment(lib,"urlmon.lib")

#define DEF_URL (_T("https://www.csdn.net/tags/MtjaQgwsMTE1MDQtYmxvZwO0O0OO0O0O.html"))
#define DEF_FILE_NAME (_T("index.html"))

//HMODULE表示模块句柄。代表应用程序载入的模块，win32系统下通常是被载入模块的线性地址。
HMODULE g_hMod = NULL;

DWORD WINAPI ThreadProc(LPVOID lParam)
{
    TCHAR szPath[_MAX_PATH] = { 0, };

    if (!GetModuleFileName(g_hMod, szPath, MAX_PATH))
        return FALSE;
    
    TCHAR* p = _tcsrchr(szPath, '\\');
    if (!p)
        return FALSE;

    //后缀_s表示使用安全的字符串拷贝函数，防止缓冲区不够大而引起错误
    _tcscpy_s(p + 1, _MAX_PATH, DEF_FILE_NAME);

    URLDownloadToFile(NULL, DEF_URL, szPath, 0, NULL);

    return 0;
}

BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
                     )
{
    //Handle 是代表系统的内核对象，如文件句柄，线程句柄，进程句柄。
    HANDLE hTread = NULL;

    g_hMod = (HMODULE)hModule;

    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
        //MessageBoxA(NULL, "DLL Attached!\n", "Game Hacking", MB_OK | MB_TOPMOST);
        OutputDebugString(_T("Inject success！"));
        _tprintf(L"Inject success！");
        hTread = CreateThread(NULL, 0, ThreadProc, NULL, 0, NULL);
        CloseHandle(hTread);
        break;
    }
    return TRUE;
}

