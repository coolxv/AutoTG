#pragma once
#include <fflua.h>
#include "capture_screen.h"

class CapIntf
{
public:
	CapIntf();
	~CapIntf();
public:
	int32_t captureStart();
	void captureEnd();
	std::map<std::string, int32_t> findColorByTv(uint32_t hsvMax, uint32_t hsvMin, uint32_t w, uint32_t h, uint32_t tmax, uint32_t tmin);
	std::map<std::string, int32_t> findColorByTvBySize(uint32_t x, uint32_t y, uint32_t w, uint32_t h, uint32_t hsvMax, uint32_t hsvMin, uint32_t ww, uint32_t hh, uint32_t tmax, uint32_t tmin);
	std::map<std::string, int32_t> findColor(uint32_t hsvMax, uint32_t hsvMin);
	std::map<std::string, int32_t> findColorBySize(uint32_t x, uint32_t y, uint32_t w, uint32_t h, uint32_t hsvMax, uint32_t hsvMin);
	std::map<std::string, int32_t> findPicByTv(std::string picName, double tv);
	std::map<std::string, int32_t> findPicByTvBySize(uint32_t x, uint32_t y, uint32_t w, uint32_t h, string picName, double tv);
	std::map<std::string, int32_t> findPic(std::string picName);
	std::map<std::string, int32_t> findPicBySize(uint32_t x, uint32_t y, uint32_t w, uint32_t h, string picName);
	std::map<std::string, double> pickColorByPosi(uint32_t x, uint32_t y);

	void setPara(int method, double max, double min, uint32_t w, uint32_t h, uint32_t wt, uint32_t ht);
	void setHwnd(int32_t hwnd);
	void setShow(bool show);
	bool isHwnd();

private:
	CaptureScreen *m_cap;
	cv::Mat m_img;
	bool m_qmode;
	bool m_show;

};

void reg_cap_intf_lapi(lua_State *ls);