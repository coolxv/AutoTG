#pragma once

#include <string>

#define DllImport   __declspec( dllimport )
#define DllExport   __declspec( dllexport )



extern "C" DllExport DWORD WINAPI gp_thr(LPVOID lpParameter);
extern "C" DllExport DWORD WINAPI gp_thr_q(LPVOID lpParameter);


