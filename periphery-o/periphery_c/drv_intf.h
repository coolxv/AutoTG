#pragma once
#include <iostream>
#include <string>
#include <windows.h>
#include <fflua.h>


class DrvIntf
{
public:
	DrvIntf();
	~DrvIntf();
public:
	// 设置通讯速度（仅支持新款硬件）
	int32_t setSpeed(unsigned char Keyboard, unsigned char Mouse, unsigned char Control);
	// 重启设备
	int32_t restart();
	// 打开设备（指定设备ID）
	int32_t openDevice(int32_t venderID, int32_t productID);
	// 关闭设备
	int32_t closeDevice();
	// 设置自定义设备ID（厂商ID+产品ID）
	int32_t setDeviceID(int32_t venderID, int32_t productID);


	// 键按下
	int32_t keyDown(std::string key);
	// 键弹起
	int32_t keyUp(std::string  key);
	// 一次按键
	int32_t keyPress(std::string  key, int32_t count);
	// 组合键按下
	int32_t combinationKeyDown(std::string  key1, std::string  key2, std::string  key3, std::string  key4, std::string  key5, std::string  key6);
	// 组合键弹起
	int32_t combinationKeyUp(std::string  key1, std::string  key2, std::string  key3, std::string  key4, std::string  key5, std::string  key6);
	// 组合按键
	int32_t combinationKeyPress(std::string  key1, std::string  key2, std::string  key3, std::string  key4, std::string  key5, std::string  key6, int32_t count);
	// 释放所有按键
	int32_t keyUpAll();
	// 模拟按键输入
	int32_t simulationPressKey(std::string keys);
	// 获取大写灯状态
	int32_t getCapsLock();
	// 获取NumLock灯状态
	int32_t getNumLock();


	// 鼠标左键按下
	int32_t leftDown();
	// 鼠标左键弹起
	int32_t leftUp();
	// 鼠标左键单击
	int32_t leftClick(int32_t count);
	// 鼠标左键双击
	int32_t leftDoubleClick(int32_t count);
	// 鼠标右键按下
	int32_t rightDown();
	// 鼠标右键弹起
	int32_t rightUp();
	// 鼠标右键单击
	int32_t rightClick(int32_t count);
	// 鼠标右键双击
	int32_t rightDoubleClick(int32_t count);
	// 鼠标中键按下
	int32_t middleDown();
	// 鼠标中键弹起
	int32_t middleUp();
	// 鼠标中键单击
	int32_t middleClick(int32_t count);
	// 鼠标中键双击
	int32_t middleDoubleClick(int32_t count);
	// 鼠标滚轮滚动
	int32_t wheelsMove(int32_t y);
	// 释放所有鼠标按键
	int32_t mouseUpAll();
	// 模拟鼠标移动
	int32_t moveTo(int32_t x, int32_t y);
	// 相对移动鼠标(X，Y不能大于255)
	int32_t moveToR(int32_t x, int32_t y);
	// 复位移动鼠标
	int32_t resetMouse();
private:
	HINSTANCE m_hdllInst;
	bool m_open;
};

void reg_drv_intf_lapi(lua_State *ls);
