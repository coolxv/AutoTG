#pragma once
//string.h是C的头文件
#include <string>
#include <windows.h>

std::wstring StringToWString(std::string s);
std::string WStringToString(std::wstring ws);

std::string StTime2StrTime(SYSTEMTIME &tm1);
SYSTEMTIME StrTime2StTime(std::string time);
ULONGLONG StrTime2LongTime(std::string time);
SYSTEMTIME LongTime2StTime(ULONGLONG ulTime);
ULONGLONG GetNetTime(std::string srv);

