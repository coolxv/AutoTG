#include "stdafx.h"
#include "set_autorun.h"
#include "guicon.h"
//////////////////////////////////////////////////////////////////////////
static bool autoRun = false;
static bool bopen = false;
static bool bredirct = false;
//响应开机自启动设置
int VirtualMouseAutorun( HWND hwndChk , bool  runSate)
{
	if (!runSate)
	{
		int iChk = (int)SendMessage(hwndChk, BM_GETCHECK, 0, 0);
		if (iChk == 0)
		{
			setVMAutorun();
			SendMessage(hwndChk, BM_SETCHECK, 1, 0);
			if (!bredirct)
			{
				RedirectIOToConsole(bopen);
				bredirct = true;
				bopen = true;
			}

		}
		else
		{
			delVMAutorun();
			SendMessage(hwndChk, BM_SETCHECK, 0, 0);
			if (bredirct)
			{
				UnRedirectIOToConsole();
				bredirct = false;
			}
		}
		return 1;
	}
	else
	{
		return 0;
	}
}

//////////////////////////////////////////////////////////////////////////

//设置开机自启动
int setVMAutorun()
{
	autoRun = true;
	return 1;
}

//////////////////////////////////////////////////////////////////////////

//取消开机自启动
int delVMAutorun()
{
	autoRun = false;
	return 1;

}

//////////////////////////////////////////////////////////////////////////

//检查是否已经开机自动运行
int checkAutorun()
{
	if (autoRun)
	{
		return 1;
	}
	else
	{
		return 0;
	}
}
