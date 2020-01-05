#include "stdafx.h"

#include "set_autorun.h"

//////////////////////////////////////////////////////////////////////////

//响应开机自启动设置
int VirtualMouseAutorun( HWND hwndChk )
{
	int iChk = (int)SendMessage( hwndChk, BM_GETCHECK, 0, 0 );
	if( iChk == 0 )
	{
		setVMAutorun();
		SendMessage( hwndChk, BM_SETCHECK, 1, 0 );
	}
	else
	{
		delVMAutorun();
		SendMessage( hwndChk, BM_SETCHECK, 0, 0 );
	}
	return 0;
}

//////////////////////////////////////////////////////////////////////////

//设置开机自启动
int setVMAutorun()
{
	HKEY hKey;     
	long lRet = RegOpenKeyEx( HKEY_LOCAL_MACHINE, TEXT("Software\\Microsoft\\Windows\\CurrentVersion\\Run"), 0, KEY_WRITE, &hKey );
	if(lRet == ERROR_SUCCESS) 
	{
		WCHAR pFilePath[MAX_PATH] = {0};
		DWORD dwRet = GetModuleFileName(NULL, pFilePath, MAX_PATH);
		lRet = RegSetValueEx( hKey, TEXT("gpatm"), 0, REG_SZ, (BYTE *)pFilePath, dwRet*2 );
		RegCloseKey(hKey);
	}
	if( lRet == ERROR_SUCCESS )
		return 1;
	return 0;
}

//////////////////////////////////////////////////////////////////////////

//取消开机自启动
int delVMAutorun()
{
    HKEY hKey;     
    long lRet = RegOpenKeyEx( HKEY_LOCAL_MACHINE, TEXT("Software\\Microsoft\\Windows\\CurrentVersion\\Run"), 0, KEY_WRITE, &hKey );
    if( lRet == ERROR_SUCCESS )
    {
        lRet = RegDeleteValue( hKey, TEXT("gpatm") );
        RegCloseKey(hKey);
    }
	if( lRet == ERROR_SUCCESS )
		return 1;
	return 0;
}

//////////////////////////////////////////////////////////////////////////

//检查是否已经开机自动运行
int checkAutorun()
{
    HKEY hKey;
    DWORD dwtype, sizeBuff;
    long lRet;
	WCHAR reBuff[MAX_PATH]	 = {0};
	WCHAR pFilePath[MAX_PATH] = {0};
	
    lRet= RegOpenKeyEx( HKEY_LOCAL_MACHINE, TEXT("Software\\Microsoft\\Windows\\CurrentVersion\\Run"), 0, KEY_READ, &hKey );
    if( lRet == ERROR_SUCCESS ) 
    {
        sizeBuff = sizeof(reBuff);

        //读取键值
        if( RegQueryValueEx( hKey, TEXT("gpatm"), 0, &dwtype, (BYTE*)reBuff, &sizeBuff ) == ERROR_SUCCESS )
		{
			GetModuleFileName( NULL, pFilePath, MAX_PATH );
			if(wcscmp( pFilePath, reBuff ) == 0 )
			{
				RegCloseKey(hKey);
				return 1;
			}
		}
        RegCloseKey(hKey);
    }
	return 0;
}
