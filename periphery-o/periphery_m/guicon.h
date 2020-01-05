#ifndef __GUICON_H__
#define __GUICON_H__
#include <windows.h>

void RedirectIOToConsole(bool open);
void UnRedirectIOToConsole();
void cls(HANDLE hConsole);

#endif