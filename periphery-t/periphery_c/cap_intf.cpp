#include "pch.h"
#include "cap_intf.h"
using namespace std;
using namespace ff;
using namespace cv;




void show(bool show, string name, Mat &image)
{
	if (show)
	{
		int   cxScreen = GetSystemMetrics(SM_CXSCREEN);  // wide
		int   cyScreen = GetSystemMetrics(SM_CYSCREEN);  // high
		//namedWindow(name, CV_WINDOW_NORMAL);
		imshow(name, image);
		//Rect rect = getWindowImageRect(name);
		//resizeWindow(name, rect.width / 2, rect.height / 2);
		moveWindow(name, cxScreen / 2, cyScreen / 2);
	}
}









CapIntf::CapIntf()
	:m_cap(nullptr), m_qmode(false), m_show(false)
{

}
CapIntf::~CapIntf()
{
	if (nullptr != m_cap)
	{
		delete m_cap;
	}
}


int32_t CapIntf::captureStart()
{
	int32_t ret = 0;
	if (nullptr != m_cap)
	{
		try
		{
			if (!m_img.empty())
			{
				m_img.release();
			}
			m_cap->captureFrame(m_img);
			show(m_show, "cap", m_img);
			ret = 1;
			m_qmode = true;

		}
		catch (exception e)
		{
			m_qmode = false;
			ret = 0;
		}
	}
	else
	{
		m_qmode = false;
		ret = 0;
	}
	return ret;
}
void CapIntf::captureEnd()
{
	if (nullptr != m_cap && true == m_qmode)
	{
		if (!m_img.empty())
		{
			m_img.release();
		}
	}
	m_qmode = false;
	return;
}

map<string, int32_t> CapIntf::findColorByTv(uint32_t hsvMax, uint32_t hsvMin, uint32_t w, uint32_t h, uint32_t tmax, uint32_t tmin)
{
	map<string, int32_t> ret;
	ret["x"] = -1;
	ret["y"] = -1;
	ret["ret"] = 0;
	if (nullptr != m_cap)
	{
		try
		{
			int h_min = (hsvMin & 0xff0000) >> 16;
			int s_min = (hsvMin & 0xff00) >> 8;
			int v_min = hsvMin & 0xff;
			int h_max = (hsvMax & 0xff0000) >> 16;
			int s_max = (hsvMax & 0xff00) >> 8;
			int v_max = hsvMax & 0xff;
			Mat image;
			Point result;
			if (m_qmode && !m_img.empty())
			{
				image = m_img;
			}
			else
			{
				m_cap->captureFrame(image);
			}
			Scalar hsv_min(h_min, s_min, v_min);
			Scalar hsv_max(h_max, s_max, v_max);
			int32_t r = m_cap->findColorByTv(image, hsv_max, hsv_min, w, h, tmax, tmin, result);
			if (r)
			{
				ret["x"] = result.x;
				ret["y"] = result.y;
				ret["ret"] = 1;
			}
			show(m_show, "color", image);

		}
		catch (exception e)
		{
			ret["ret"] = -1;
		}
	}
	else
	{
		ret["ret"] = -2;
	}
	return ret;
}
map<string, int32_t> CapIntf::findColorByTvBySize(uint32_t x, uint32_t y, uint32_t w, uint32_t h, uint32_t hsvMax, uint32_t hsvMin, uint32_t ww, uint32_t hh, uint32_t tmax, uint32_t tmin)
{
	map<string, int32_t> ret;
	ret["x"] = -1;
	ret["y"] = -1;
	ret["ret"] = 0;
	if (nullptr != m_cap)
	{
		try
		{
			int h_min = (hsvMin & 0xff0000) >> 16;
			int s_min = (hsvMin & 0xff00) >> 8;
			int v_min = hsvMin & 0xff;
			int h_max = (hsvMax & 0xff0000) >> 16;
			int s_max = (hsvMax & 0xff00) >> 8;
			int v_max = hsvMax & 0xff;
			Mat image;
			Point result;
			if (m_qmode && !m_img.empty())
			{
				image = m_img;
			}
			else
			{
				m_cap->captureFrame(image);
			}

			Rect rect(x, y, w, h);
			Scalar hsv_min(h_min, s_min, v_min);
			Scalar hsv_max(h_max, s_max, v_max);
			int32_t r = m_cap->findColorByTvBySize(image, rect, hsv_max, hsv_min, ww, hh, tmax, tmin, result);
			if (r)
			{
				ret["x"] = result.x;
				ret["y"] = result.y;
				ret["ret"] = 1;
			}
			show(m_show, "color", image);

		}
		catch (exception e)
		{
			ret["ret"] = -1;
		}
	}
	else
	{
		ret["ret"] = -2;
	}
	return ret;
}

map<string, int32_t> CapIntf::findColor(uint32_t hsvMax, uint32_t hsvMin)
{
	map<string, int32_t> ret;
	ret["x"] = -1;
	ret["y"] = -1;
	ret["ret"] = 0;
	if (nullptr != m_cap)
	{
		try
		{
			int h_min = (hsvMin & 0xff0000) >> 16;
			int s_min = (hsvMin & 0xff00) >> 8;
			int v_min = hsvMin & 0xff;
			int h_max = (hsvMax & 0xff0000) >> 16;
			int s_max = (hsvMax & 0xff00) >> 8;
			int v_max = hsvMax & 0xff;
			Mat image;
			Point result;
			if (m_qmode && !m_img.empty())
			{
				image = m_img;
			}
			else
			{
				m_cap->captureFrame(image);
			}
			Scalar hsv_min(h_min, s_min, v_min);
			Scalar hsv_max(h_max, s_max, v_max);
			int32_t r = m_cap->findColor(image, hsv_max, hsv_min, result);
			if (r)
			{
				ret["x"] = result.x;
				ret["y"] = result.y;
				ret["ret"] = 1;
			}
			show(m_show, "color", image);
		}
		catch (exception e)
		{
			ret["ret"] = -1;
		}
	}
	else
	{
		ret["ret"] = -2;
	}
	return ret;
}
map<string, int32_t> CapIntf::findColorBySize(uint32_t x, uint32_t y, uint32_t w, uint32_t h, uint32_t hsvMax, uint32_t hsvMin)
{
	map<string, int32_t> ret;
	ret["x"] = -1;
	ret["y"] = -1;
	ret["ret"] = 0;
	if (nullptr != m_cap)
	{
		try
		{
			int h_min = (hsvMin & 0xff0000) >> 16;
			int s_min = (hsvMin & 0xff00) >> 8;
			int v_min = hsvMin & 0xff;
			int h_max = (hsvMax & 0xff0000) >> 16;
			int s_max = (hsvMax & 0xff00) >> 8;
			int v_max = hsvMax & 0xff;
			Mat image;
			Point result;
			if (m_qmode && !m_img.empty())
			{
				image = m_img;
			}
			else
			{
				m_cap->captureFrame(image);
			}
			Rect rect(x, y, w, h);
			Scalar hsv_min(h_min, s_min, v_min);
			Scalar hsv_max(h_max, s_max, v_max);
			int32_t r = m_cap->findColorBySize(image, rect, hsv_max, hsv_min, result);
			if (r)
			{
				ret["x"] = result.x;
				ret["y"] = result.y;
				ret["ret"] = 1;
			}
			show(m_show, "color", image);
		}
		catch (exception e)
		{
			ret["ret"] = -1;
		}
	}
	else
	{
		ret["ret"] = -2;
	}
	return ret;
}
map<string, int32_t> CapIntf::findPicByTv(string picName, double tv)
{
	map<string, int32_t> ret;
	ret["x"] = -1;
	ret["y"] = -1;
	ret["ret"] = 0;
	if (nullptr != m_cap)
	{
		try
		{
			Mat templ = imread(picName, IMREAD_COLOR);
			if (templ.data)
			{
				Mat image;
				Point result;
				if (m_qmode && !m_img.empty())
				{
					image = m_img;
				}
				else
				{
					m_cap->captureFrame(image);
				}
				int32_t r = m_cap->findPicByTv(image, templ, tv, result);
				if (r)
				{
					ret["x"] = result.x;
					ret["y"] = result.y;
					ret["ret"] = 1;
				}
				show(m_show, "pic", image);
			}
		}
		catch (exception e)
		{
			ret["ret"] = -1;
		}
	}
	else
	{
		ret["ret"] = -2;
	}
	return ret;
}
map<string, int32_t> CapIntf::findPicByTvBySize(uint32_t x, uint32_t y, uint32_t w, uint32_t h, string picName, double tv)
{
	map<string, int32_t> ret;
	ret["x"] = -1;
	ret["y"] = -1;
	ret["ret"] = 0;
	if (nullptr != m_cap)
	{
		try
		{
			Mat templ = imread(picName, IMREAD_COLOR);
			if (templ.data)
			{
				Mat image;
				Point result;
				if (m_qmode && !m_img.empty())
				{
					image = m_img;
				}
				else
				{
					m_cap->captureFrame(image);
				}
				Rect rect(x, y, w, h);
				int32_t r = m_cap->findPicByTvBySize(image, templ, rect, tv, result);
				if (r)
				{
					ret["x"] = result.x;
					ret["y"] = result.y;
					ret["ret"] = 1;
				}
				show(m_show, "pic", image);
			}
		}
		catch (exception e)
		{
			ret["ret"] = -1;
		}
	}
	else
	{
		ret["ret"] = -2;
	}
	return ret;
}

map<string, int32_t> CapIntf::findPic(string picName)
{
	map<string, int32_t> ret;
	ret["x"] = -1;
	ret["y"] = -1;
	ret["ret"] = 0;
	if (nullptr != m_cap)
	{
		try
		{
			Mat templ = imread(picName, IMREAD_COLOR);
			if (templ.data)
			{
				Mat image;
				Point result;
				if (m_qmode && !m_img.empty())
				{
					image = m_img;
				}
				else
				{
					m_cap->captureFrame(image);
				}
				int32_t r = m_cap->findPic(image, templ, result);
				if (r)
				{
					ret["x"] = result.x;
					ret["y"] = result.y;
					ret["ret"] = 1;
				}
				show(m_show, "pic", image);
			}
		}
		catch (exception e)
		{
			ret["ret"] = -1;
		}
	}
	else
	{
		ret["ret"] = -2;
	}
	return ret;
}

map<string, int32_t> CapIntf::findPicBySize(uint32_t x, uint32_t y, uint32_t w, uint32_t h, string picName)
{
	map<string, int32_t> ret;
	ret["x"] = -1;
	ret["y"] = -1;
	ret["ret"] = 0;
	if (nullptr != m_cap)
	{
		try
		{
			Mat templ = imread(picName, IMREAD_COLOR);
			if (templ.data)
			{
				Mat image;
				Point result;
				if (m_qmode && !m_img.empty())
				{
					image = m_img;
				}
				else
				{
					m_cap->captureFrame(image);
				}
				Rect rect(x, y, w, h);
				int32_t r = m_cap->findPicBySize(image, templ, rect, result);
				if (r)
				{
					ret["x"] = result.x;
					ret["y"] = result.y;
					ret["ret"] = 1;
				}
				show(m_show, "pic", image);
			}
		}
		catch (exception e)
		{
			ret["ret"] = -1;
		}
	}
	else
	{
		ret["ret"] = -2;
	}
	return ret;
}
map<string, double> CapIntf::pickColorByPosi(uint32_t x, uint32_t y)
{
	map<string, double> ret;
	ret["r"] = -1;
	ret["g"] = -1;
	ret["b"] = -1;
	ret["ret"] = 0;
	if (nullptr != m_cap)
	{
		try
		{
			Mat image;
			Vec3b resultBGR;
			Vec3b resultHSV;
			if (m_qmode && !m_img.empty())
			{
				image = m_img;
			}
			else
			{
				m_cap->captureFrame(image);
			}
			int32_t r = m_cap->pickColorByPosi(image, x, y, resultBGR, resultHSV);
			if (r)
			{
				ret["b"] = resultBGR(0);
				ret["g"] = resultBGR(1);
				ret["r"] = resultBGR(2);
				ret["h"] = resultHSV(0);
				ret["s"] = resultHSV(1);
				ret["v"] = resultHSV(2);
				ret["ret"] = 1;
			}
		}
		catch (exception e)
		{
			ret["ret"] = -1;
		}
	}
	else
	{
		ret["ret"] = -2;
	}
	return ret;
}

void CapIntf::setPara(int method, double pmax, double pmin, uint32_t w, uint32_t h, uint32_t wt, uint32_t ht)
{
	if (nullptr != m_cap)
	{
		m_cap->setMethod(method);
		m_cap->setPicMax(pmax);
		m_cap->setPicMin(pmin);
		m_cap->setColorWidth(w);
		m_cap->setColorHeight(h);
		m_cap->setColorThred(wt, ht);
	}
}


void CapIntf::setHwnd(int32_t hwnd)
{
	if (nullptr == m_cap)
	{
		m_cap = new CaptureScreen((HWND)hwnd);
	}
	else
	{
		delete m_cap;
		m_cap = new CaptureScreen((HWND)hwnd);
	}
}
void CapIntf::setShow(bool show)
{
	m_show = show;
}

bool CapIntf::isHwnd()
{
	if (nullptr == m_cap)
	{
		return false;
	}
	else
	{
		return true;
	}
}void reg_cap_intf_lapi(lua_State *ls)
{
	//! 注册基类函数, ctor() 为构造函数的类型  
	fflua_register_t<CapIntf, ctor()>(ls, "CapIntf")
		.def(&CapIntf::captureStart, "captureStart")
		.def(&CapIntf::captureEnd, "captureEnd")
		.def(&CapIntf::findColorByTv, "findColorByTv")
		.def(&CapIntf::findColor, "findColor")
		.def(&CapIntf::findPicByTv, "findPicByTv")
		.def(&CapIntf::findPic, "findPic")
		.def(&CapIntf::findColorByTvBySize, "findColorByTvBySize")
		.def(&CapIntf::findColorBySize, "findColorBySize")
		.def(&CapIntf::findPicByTvBySize, "findPicByTvBySize")
		.def(&CapIntf::findPicBySize, "findPicBySize")
		.def(&CapIntf::pickColorByPosi, "pickColorByPosi")
		.def(&CapIntf::setPara, "setPara")
		.def(&CapIntf::setHwnd, "setHwnd")
		.def(&CapIntf::setShow, "setShow")
		.def(&CapIntf::isHwnd, "isHwnd");

}