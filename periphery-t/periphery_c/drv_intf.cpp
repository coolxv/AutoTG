
#include "pch.h"
#include "drv_intf.h"

using namespace ff;
using namespace std;


// 定义接口函数类型
typedef bool(WINAPI *_SetSpeed)(unsigned char Keyboard, unsigned char Mouse, unsigned char Control);
typedef bool(WINAPI *_Restart)();
typedef bool(WINAPI *_OpenDevice)();
typedef bool(WINAPI *_OpenDeviceEx)(int VenderID, int ProductID);
typedef bool(WINAPI *_CloseDevice)();
typedef bool(WINAPI *_SetDeviceID)(int VenderID, int ProductID);

typedef int (WINAPI *_KeyDown)(const char *Key);
typedef int (WINAPI *_KeyUp)(const char *Key);
typedef int (WINAPI *_KeyPress)(const char *Key, int count);
typedef int (WINAPI *_CombinationKeyDown)(const char *Key1, const char *Key2, const char *Key3, const char *Key4, const char *Key5, const char *Key6 );
typedef int (WINAPI *_CombinationKeyUp)(const char *Key1, const char *Key2, const char *Key3, const char *Key4, const char *Key5, const char *Key6);
typedef int (WINAPI *_CombinationKeyPress)(const char *Key1, const char *Key2, const char *Key3, const char *Key4, const char *Key5, const char *Key6, int count);
typedef int (WINAPI *_KeyUpAll)();
typedef int (WINAPI *_SimulationPressKey)(const char *Key);

typedef int (WINAPI *_LeftDown)();
typedef int (WINAPI *_LeftUp)();
typedef int (WINAPI *_LeftClick)(int count);
typedef int (WINAPI *_LeftDoubleClick)(int count);

typedef int (WINAPI *_RightDown)();
typedef int (WINAPI *_RightUp)();
typedef int (WINAPI *_RightClick)(int count);
typedef int (WINAPI *_RightDoubleClick)(int count);

typedef int (WINAPI *_MiddleDown)();
typedef int (WINAPI *_MiddleUp)();
typedef int (WINAPI *_MiddleClick)(int count);
typedef int (WINAPI *_MiddleDoubleClick)(int count);

typedef int (WINAPI *_WheelsMove)(int Y);
typedef int (WINAPI *_MouseUpAll)();
typedef int (WINAPI *_MoveTo)(int X, int Y);
typedef int (WINAPI *_MoveToR)(int X, int Y);

typedef int (WINAPI *_GetCapsLock)();
typedef int (WINAPI *_GetNumLock)();




DrvIntf::DrvIntf()
	:m_open(false)
{
	// 加载动态库 kmd.dll
	m_hdllInst = LoadLibrary(L"km.dll");
	
}
DrvIntf::~DrvIntf()
{
	if (m_hdllInst)
	{
		FreeLibrary(m_hdllInst);
	}
}

// 设置通讯速度（仅支持新款硬件）
int32_t DrvIntf::setSpeed(unsigned char Keyboard, unsigned char Mouse, unsigned char Control)
{

	if (m_hdllInst)
	{
		_SetSpeed SetSpeed = (_SetSpeed)GetProcAddress(m_hdllInst, "SetSpeed");
		if (SetSpeed(Keyboard, Mouse, Control))
		{
			return 1;
		}
		else
		{
			return 0;
		}
	}

	return -1;
}

// 重启设备
int32_t DrvIntf::restart()
{

	if (m_hdllInst)
	{
		_Restart Restart = (_Restart)GetProcAddress(m_hdllInst, "Restart");
		if (Restart())
		{
			return 1;
		}
		else
		{
			return 0;
		}
	}

	return -1;
}


// 打开设备（指定设备ID）
int32_t DrvIntf::openDevice(int32_t venderID, int32_t productID)
{

	if (m_hdllInst)
	{
		_OpenDeviceEx OpenDeviceEx = (_OpenDeviceEx)GetProcAddress(m_hdllInst, "OpenDeviceEx");
		if (OpenDeviceEx(venderID, productID))
		{
			m_open = true;
			return 1;
		}
		else
		{
			return 0;
		}
	}

	return -1;
}
// 关闭设备
int32_t DrvIntf::closeDevice()
{

	if (m_hdllInst)
	{
		_CloseDevice CloseDevice = (_CloseDevice)GetProcAddress(m_hdllInst, "CloseDevice");
		if (m_open && CloseDevice())
		{
			m_open = false;
			return 1;
		}
		else
		{
			return 0;
		}
	}

	return -1;
}
// 设置自定义设备ID（厂商ID+产品ID）
int32_t DrvIntf::setDeviceID(int32_t venderID, int32_t productID)
{

	if (m_hdllInst)
	{
		_SetDeviceID SetDeviceID = (_SetDeviceID)GetProcAddress(m_hdllInst, "SetDeviceID");
		if (SetDeviceID(venderID, productID))
		{
			return 1;
		}
		else
		{
			return 0;
		}
	}

	return -1;
}

// 键按下
int32_t DrvIntf::keyDown(string  key)
{
	if (m_hdllInst)
	{
		_KeyDown KeyDown = (_KeyDown)GetProcAddress(m_hdllInst, "KeyDown");
		if (KeyDown(key.c_str()))
		{
			return 1;
		}
		else
		{
			return 0;
		}
	}

	return -1;
}
// 键弹起
int32_t DrvIntf::keyUp(string key)
{
	if (m_hdllInst)
	{
		_KeyUp KeyUp = (_KeyUp)GetProcAddress(m_hdllInst, "KeyUp");
		if (KeyUp(key.c_str()))
		{
			return 1;
		}
		else
		{
			return 0;
		}
	}

	return -1;
}
// 一次按键
int32_t DrvIntf::keyPress(string key, int32_t count)
{
	if (m_hdllInst)
	{
		_KeyPress KeyPress = (_KeyPress)GetProcAddress(m_hdllInst, "KeyPress");
		if (KeyPress(key.c_str(), count))
		{
			return 1;
		}
		else
		{
			return 0;
		}
	}

	return -1;
}
// 组合键按下
int32_t DrvIntf::combinationKeyDown(string key1, string key2, string key3, string key4, string key5, string key6)
{
	if (m_hdllInst)
	{
		_CombinationKeyDown CombinationKeyDown = (_CombinationKeyDown)GetProcAddress(m_hdllInst, "CombinationKeyDown");
		if (CombinationKeyDown(key1.c_str(), key2.c_str(), key3.c_str(), key4.c_str(), key5.c_str(), key6.c_str()))
		{
			return 1;
		}
		else
		{
			return 0;
		}
	}

	return -1;
}
// 组合键弹起
int32_t DrvIntf::combinationKeyUp(string key1, string key2, string key3, string key4, string key5, string key6)
{
	if (m_hdllInst)
	{
		_CombinationKeyUp CombinationKeyUp = (_CombinationKeyUp)GetProcAddress(m_hdllInst, "CombinationKeyUp");
		if (CombinationKeyUp(key1.c_str(), key2.c_str(), key3.c_str(), key4.c_str(), key5.c_str(), key6.c_str()))
		{
			return 1;
		}
		else
		{
			return 0;
		}
	}

	return -1;
}
// 组合按键
int32_t DrvIntf::combinationKeyPress(string key1, string key2, string key3, string key4, string key5, string key6, int32_t count)
{
	if (m_hdllInst)
	{
		_CombinationKeyPress CombinationKeyPress = (_CombinationKeyPress)GetProcAddress(m_hdllInst, "CombinationKeyPress");
		if (CombinationKeyPress(key1.c_str(), key2.c_str(), key3.c_str(), key4.c_str(), key5.c_str(), key6.c_str(), count))
		{
			return 1;
		}
		else
		{
			return 0;
		}
	}

	return -1;
}
// 释放所有按键
int32_t DrvIntf::keyUpAll()
{
	if (m_hdllInst)
	{
		_KeyUpAll KeyUpAll = (_KeyUpAll)GetProcAddress(m_hdllInst, "KeyUpAll");
		if (KeyUpAll())
		{
			return 1;
		}
		else
		{
			return 0;
		}
	}

	return -1;
}
// 模拟按键输入
int32_t DrvIntf::simulationPressKey(string keys)
{
	if (m_hdllInst)
	{
		_SimulationPressKey SimulationPressKey = (_SimulationPressKey)GetProcAddress(m_hdllInst, "SimulationPressKey");
		if (SimulationPressKey(keys.c_str()))
		{
			return 1;
		}
		else
		{
			return 0;
		}
	}

	return -1;
}

// 获取大写灯状态
int32_t DrvIntf::getCapsLock()
{
	if (m_hdllInst)
	{
		_GetCapsLock GetCapsLock = (_GetCapsLock)GetProcAddress(m_hdllInst, "GetCapsLock");
		int32_t ret = GetCapsLock();
		return ret;
	}

	return -1;
}
// 获取NumLock灯状态
int32_t DrvIntf::getNumLock()
{
	if (m_hdllInst)
	{
		_GetNumLock GetNumLock = (_GetNumLock)GetProcAddress(m_hdllInst, "GetNumLock");
		int32_t ret = GetNumLock();
		return ret;
	}

	return -1;
}
// 鼠标左键按下
int32_t DrvIntf::leftDown()
{
	if (m_hdllInst)
	{
		_LeftDown LeftDown = (_LeftDown)GetProcAddress(m_hdllInst, "LeftDown");
		if (LeftDown())
		{
			return 1;
		}
		else
		{
			return 0;
		}
	}

	return -1;
}
// 鼠标左键弹起
int32_t DrvIntf::leftUp()
{
	if (m_hdllInst)
	{
		_LeftUp LeftUp = (_LeftUp)GetProcAddress(m_hdllInst, "LeftUp");
		if (LeftUp())
		{
			return 1;
		}
		else
		{
			return 0;
		}
	}

	return -1;
}
// 鼠标左键单击
int32_t DrvIntf::leftClick(int32_t count)
{
	if (m_hdllInst)
	{
		_LeftClick LeftClick = (_LeftClick)GetProcAddress(m_hdllInst, "LeftClick");
		if (LeftClick(count))
		{
			return 1;
		}
		else
		{
			return 0;
		}
	}

	return -1;
}
// 鼠标左键双击
int32_t DrvIntf::leftDoubleClick(int32_t count)
{
	if (m_hdllInst)
	{
		_LeftDoubleClick LeftDoubleClick = (_LeftDoubleClick)GetProcAddress(m_hdllInst, "LeftDoubleClick");
		if (LeftDoubleClick(count))
		{
			return 1;
		}
		else
		{
			return 0;
		}
	}

	return -1;
}
// 鼠标右键按下
int32_t DrvIntf::rightDown()
{
	if (m_hdllInst)
	{
		_RightDown RightDown = (_RightDown)GetProcAddress(m_hdllInst, "RightDown");
		if (RightDown())
		{
			return 1;
		}
		else
		{
			return 0;
		}
	}

	return -1;
}
// 鼠标右键弹起
int32_t DrvIntf::rightUp()
{
	if (m_hdllInst)
	{
		_RightUp RightUp = (_RightUp)GetProcAddress(m_hdllInst, "RightUp");
		if (RightUp())
		{
			return 1;
		}
		else
		{
			return 0;
		}
	}

	return -1;
}
// 鼠标右键单击
int32_t DrvIntf::rightClick(int32_t count)
{
	if (m_hdllInst)
	{
		_RightClick RightClick = (_RightClick)GetProcAddress(m_hdllInst, "RightClick");
		if (RightClick(count))
		{
			return 1;
		}
		else
		{
			return 0;
		}
	}

	return -1;
}
// 鼠标右键双击
int32_t DrvIntf::rightDoubleClick(int32_t count)
{
	if (m_hdllInst)
	{
		_RightDoubleClick RightDoubleClick = (_RightDoubleClick)GetProcAddress(m_hdllInst, "RightDoubleClick");
		if (RightDoubleClick(count))
		{
			return 1;
		}
		else
		{
			return 0;
		}
	}

	return -1;
}
// 鼠标中键按下
int32_t DrvIntf::middleDown()
{
	if (m_hdllInst)
	{
		_MiddleDown MiddleDown = (_MiddleDown)GetProcAddress(m_hdllInst, "MiddleDown");
		if (MiddleDown())
		{
			return 1;
		}
		else
		{
			return 0;
		}
	}

	return -1;
}
// 鼠标中键弹起
int32_t DrvIntf::middleUp()
{
	if (m_hdllInst)
	{
		_MiddleUp MiddleUp = (_MiddleUp)GetProcAddress(m_hdllInst, "MiddleUp");
		if (MiddleUp())
		{
			return 1;
		}
		else
		{
			return 0;
		}
	}

	return -1;
}
// 鼠标中键单击
int32_t DrvIntf::middleClick(int32_t count)
{
	if (m_hdllInst)
	{
		_MiddleClick MiddleClick = (_MiddleClick)GetProcAddress(m_hdllInst, "MiddleClick");
		if (MiddleClick(count))
		{
			return 1;
		}
		else
		{
			return 0;
		}
	}

	return -1;
}
// 鼠标中键双击
int32_t DrvIntf::middleDoubleClick(int32_t count)
{
	if (m_hdllInst)
	{
		_MiddleDoubleClick MiddleDoubleClick = (_MiddleDoubleClick)GetProcAddress(m_hdllInst, "MiddleDoubleClick");
		if (MiddleDoubleClick(count))
		{
			return 1;
		}
		else
		{
			return 0;
		}
	}

	return -1;
}
// 鼠标滚轮滚动
int32_t DrvIntf::wheelsMove(int32_t y)
{
	if (m_hdllInst)
	{
		_WheelsMove WheelsMove = (_WheelsMove)GetProcAddress(m_hdllInst, "WheelsMove");
		if (WheelsMove(y))
		{
			return 1;
		}
		else
		{
			return 0;
		}
	}

	return -1;
}
// 释放所有鼠标按键
int32_t DrvIntf::mouseUpAll()
{
	if (m_hdllInst)
	{
		_MouseUpAll MouseUpAll = (_MouseUpAll)GetProcAddress(m_hdllInst, "MouseUpAll");
		if (MouseUpAll())
		{
			return 1;
		}
		else
		{
			return 0;
		}
	}

	return -1;
}
// 模拟鼠标移动
int32_t DrvIntf::moveTo(int32_t x, int32_t y)
{
	if (m_hdllInst)
	{
		_MoveTo MoveTo = (_MoveTo)GetProcAddress(m_hdllInst, "MoveTo");
redo:
		int32_t ret = MoveTo(x, y);
		POINT pt;
		BOOL bReturn = GetCursorPos(&pt); //获取鼠标指针位置到pt
		if (bReturn != FALSE && (pt.x != x || pt.y != y)) //如果函数执行成功
		{
			goto redo;
		}

		if (ret)
		{
			return 1;
		}
		else
		{
			return 0;
		}
	}

	return -1;
}
// 相对移动鼠标(X，Y不能大于255)
int32_t DrvIntf::moveToR(int32_t x, int32_t y)
{
	if (m_hdllInst)
	{
		_MoveToR MoveToR = (_MoveToR)GetProcAddress(m_hdllInst, "MoveToR");
		if (MoveToR(x, y))
		{
			return 1;
		}
		else
		{
			return 0;
		}
	}

	return -1;
}
// 复位移动鼠标
int32_t DrvIntf::resetMouse()
{
	if (m_hdllInst)
	{
		_MoveTo MoveTo = (_MoveTo)GetProcAddress(m_hdllInst, "MoveTo");
		if (MoveTo(0, 0))
		{
			return 1;
		}
		else
		{
			return 0;
		}
	}

	return -1;
}

void reg_drv_intf_lapi(lua_State *ls)
{
	//! 注册基类函数, ctor() 为构造函数的类型  
	fflua_register_t<DrvIntf, ctor()>(ls, "DrvIntf")
		.def(&DrvIntf::setSpeed, "setSpeed")
		.def(&DrvIntf::restart, "restart")
		.def(&DrvIntf::openDevice, "openDevice")
		.def(&DrvIntf::closeDevice, "closeDevice")
		.def(&DrvIntf::setDeviceID, "setDeviceID")
		.def(&DrvIntf::keyDown, "keyDown")
		.def(&DrvIntf::keyUp, "keyUp")
		.def(&DrvIntf::keyPress, "keyPress")
		.def(&DrvIntf::combinationKeyDown, "combinationKeyDown")
		.def(&DrvIntf::combinationKeyUp, "combinationKeyUp")
		.def(&DrvIntf::combinationKeyPress, "combinationKeyPress")
		.def(&DrvIntf::keyUpAll, "keyUpAll")
		.def(&DrvIntf::simulationPressKey, "simulationPressKey")
		.def(&DrvIntf::getCapsLock, "getCapsLock")
		.def(&DrvIntf::getNumLock, "getNumLock")
		.def(&DrvIntf::leftDown, "leftDown")
		.def(&DrvIntf::leftUp, "leftUp")
		.def(&DrvIntf::leftClick, "leftClick")
		.def(&DrvIntf::leftDoubleClick, "leftDoubleClick")
		.def(&DrvIntf::rightDown, "rightDown")
		.def(&DrvIntf::rightUp, "rightUp")
		.def(&DrvIntf::rightClick, "rightClick")
		.def(&DrvIntf::rightDoubleClick, "rightDoubleClick")
		.def(&DrvIntf::middleDown, "middleDown")
		.def(&DrvIntf::middleUp, "middleUp")
		.def(&DrvIntf::middleClick, "middleClick")
		.def(&DrvIntf::middleDoubleClick, "middleDoubleClick")
		.def(&DrvIntf::wheelsMove, "wheelsMove")
		.def(&DrvIntf::mouseUpAll, "mouseUpAll")
		.def(&DrvIntf::moveTo, "moveTo")
		.def(&DrvIntf::moveToR, "moveToR")
		.def(&DrvIntf::resetMouse, "resetMouse");

}