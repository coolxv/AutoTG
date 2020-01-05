#pragma once
#include <string>
#include <map>
#include <fflua.h>


class WinIntf
{
public:
	WinIntf();
	~WinIntf();
public:
	int32_t runExe(std::string exeFullPathName);
	int32_t findWindow(std::string className, std::string titleName);
	int32_t moveWindow(int32_t hwnd, int32_t x, int32_t y);
	std::map<std::string, int32_t> getWindowRect(int32_t hwnd);
	std::map<std::string, int32_t> getWindowSize(int32_t hwnd);
	std::map<std::string, int32_t> getClientRect(int32_t hwnd);
	std::map<std::string, int32_t> getClientSize(int32_t hwnd);
	std::map<std::string, int32_t> clientToScreen(int32_t hwnd);
	std::string  getProcessName();

};

void reg_win_intf_lapi(lua_State *ls);
