#include "pch.h"
#include <windows.h>
#include <Tlhelp32.h>
#include "win_intf.h"
#include "utils.h"

using namespace ff;
using namespace std;



WinIntf::WinIntf()
{

}
WinIntf::~WinIntf()
{

}

int32_t WinIntf::runExe(string exeFullPathName)
{

	wstring wsexeFullPathName = StringToWString(exeFullPathName);

	HINSTANCE hd =	ShellExecute(NULL, L"open", wsexeFullPathName.c_str(), NULL, NULL, SW_SHOWNORMAL);
	if ((int32_t)hd > 32)
	{
		return 1;
	}
	return 0;
}
int32_t WinIntf::findWindow(string className, string titleName)
{
	wstring wclassName = StringToWString(className);
	wstring wtitleName = StringToWString(titleName);
	HWND hq = FindWindow(nullptr, wtitleName.size() == 0 ? nullptr : wtitleName.c_str());
	if (hq != nullptr)
	{
		if (GetWindowLong(hq, GWL_EXSTYLE) & WS_EX_TOPMOST)
		{
			SetForegroundWindow(hq);
			SetFocus(hq);
			//SetWindowPos(hq,HWND_TOP, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_SHOWWINDOW | SWP_NOACTIVATE);
		}	
	}
	return (int32_t)hq;
}
int32_t WinIntf::moveWindow(int32_t hwnd, int32_t x, int32_t y)
{
	RECT rect;
	bool ret = GetWindowRect((HWND)hwnd, &rect);
	if (ret)
	{
		int w = rect.right - rect.left, h = rect.bottom - rect.top;
		return MoveWindow((HWND)hwnd, x, y, w, h, false);
	}
	return 0;

}
map<string, int32_t> WinIntf::getWindowRect(int32_t hwnd)
{
	RECT rect;
	map<string, int32_t> result;
	result["ret"] = 0;
	result["l"] = -1;
	result["t"] = -1;
	result["r"] = -1;
	result["b"] = -1;

	bool ret =GetWindowRect((HWND)hwnd, &rect);
	if(ret)
	{
		result["ret"] = 1;
		result["l"] = rect.left;
		result["t"] = rect.top;
		result["r"] = rect.right;
		result["b"] = rect.bottom;
	}

	return result;
}

map<string, int32_t> WinIntf::getWindowSize(int32_t hwnd)
{
	RECT rect;
	map<string, int32_t> result;
	result["ret"] = 0;
	result["x"] = -1;
	result["y"] = -1;
	result["width"] = -1;
	result["height"] = -1;


	bool ret = GetWindowRect((HWND)hwnd, &rect);
	if (ret)
	{
		result["ret"] = 1;
		result["x"] = rect.left;
		result["y"] = rect.top;
		result["width"] = rect.right - rect.left;
		result["height"] = rect.bottom -rect.top;

	}

	return result;
}

map<string, int32_t> WinIntf::getClientRect(int32_t hwnd)
{
	RECT rect;
	POINT client_offset = { 0, 0 };
	map<string, int32_t> result;
	result["ret"] = 0;
	result["l"] = -1;
	result["t"] = -1;
	result["r"] = -1;
	result["b"] = -1;

	bool ret1 = ClientToScreen((HWND)hwnd, &client_offset);
	bool ret2 = GetClientRect((HWND)hwnd, &rect);
	if (ret1 && ret2)
	{
		result["ret"] = 1;
		result["l"] = rect.left + client_offset.x;
		result["t"] = rect.top + client_offset.y;
		result["r"] = rect.right + client_offset.x;
		result["b"] = rect.bottom + client_offset.y;
	}

	return result;
}

map<string, int32_t> WinIntf::getClientSize(int32_t hwnd)
{
	RECT rect;
	POINT client_offset = { 0, 0 };
	map<string, int32_t> result;
	result["ret"] = 0;
	result["x"] = -1;
	result["y"] = -1;
	result["width"] = -1;
	result["height"] = -1;

	bool ret1 = ClientToScreen((HWND)hwnd, &client_offset);
	bool ret2 = GetClientRect((HWND)hwnd, &rect);
	if (ret1 && ret2)
	{
		result["ret"] = 1;
		result["x"] = client_offset.x;
		result["y"] = client_offset.y;
		result["width"] = rect.right - rect.left;
		result["height"] = rect.bottom - rect.top;
	}

	return result;
}
map<string, int32_t> WinIntf::clientToScreen(int32_t hwnd)
{
	POINT client_offset = { 0, 0 };
	map<string, int32_t> result;
	result["ret"] = 0;
	result["x"] = -1;
	result["y"] = -1;
	bool ret = ClientToScreen((HWND)hwnd, &client_offset);
	if (ret)
	{
		result["ret"] = 1;
		result["x"] = client_offset.x;
		result["y"] = client_offset.y;
	}

	return result;
}


string  WinIntf::getProcessName()
{
	wstring loStrRet = L"";
	//得到该进程的进程id
	DWORD ldwProID = GetCurrentProcessId();
	if (0 == ldwProID)
		return "";
	HANDLE handle = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	if (handle == (HANDLE)-1)
	{
		return "";
	}
	PROCESSENTRY32 procinfo;
	procinfo.dwSize = sizeof(PROCESSENTRY32);
	BOOL more = ::Process32First(handle, &procinfo);
	while (more)
	{
		if (procinfo.th32ProcessID == ldwProID)
		{
			loStrRet = procinfo.szExeFile;
			CloseHandle(handle);
			return WStringToString(loStrRet);
		}
		more = Process32Next(handle, &procinfo);
	}
	CloseHandle(handle);
	return WStringToString(loStrRet);
}


void reg_win_intf_lapi(lua_State *ls)
{
	//! 注册基类函数, ctor() 为构造函数的类型  
	fflua_register_t<WinIntf, ctor()>(ls, "WinIntf")
		.def(&WinIntf::runExe, "runExe")
		.def(&WinIntf::findWindow, "findWindow")
		.def(&WinIntf::moveWindow, "moveWindow")
		.def(&WinIntf::getWindowRect, "getWindowRect")
		.def(&WinIntf::getWindowSize, "getWindowSize")
		.def(&WinIntf::getClientRect, "getClientRect")
		.def(&WinIntf::getClientSize, "getClientSize")
		.def(&WinIntf::clientToScreen, "clientToScreen")
		.def(&WinIntf::getProcessName, "getProcessName");

}