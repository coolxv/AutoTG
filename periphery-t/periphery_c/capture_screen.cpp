#include "pch.h"
#include <vector>
#include <windows.h>
#include <opencv2/opencv.hpp>
#include "capture_screen.h"

using namespace std;
using namespace cv;

struct GetHwndsState
{
	DWORD processId;
	vector<HWND>& list;
	bool visibleOnly;
};

static void endCaptureWindow(CaptureState*& cs);

static void initialize()
{
	HINSTANCE hUser32 = LoadLibrary(L"User32.dll");
	if (hUser32)
	{
		typedef BOOL(WINAPI* LPSetProcessDPIAware)(void);
		LPSetProcessDPIAware pSetProcessDPIAware = (LPSetProcessDPIAware)GetProcAddress(hUser32, "SetProcessDPIAware");
		if (pSetProcessDPIAware)
		{
			pSetProcessDPIAware();
		}
		FreeLibrary(hUser32);
	}	


}

static void calcRect(RECT& rect, int x, int y)
{
	rect.left += x, rect.right += x;
	rect.top += y, rect.bottom += y;
}

static bool overlapRect(const RECT& a, const RECT& b)
{
	return (a.left >= b.left && a.left < b.right || a.right > b.left && a.right <= b.right)
		&& (a.top >= b.top && a.top < b.bottom || a.bottom > b.top && a.bottom <= b.bottom);
}

static void revealWindow(HWND window)
{
	if (!IsWindow(window))
		throw exception("Window does not exist.");

	if (IsIconic(window))
		SendMessage(window, WM_SYSCOMMAND, SC_RESTORE, 0);

	RECT cr;
	GetClientRect(window, &cr);

	POINT client_offset = { 0, 0 };
	if (!ClientToScreen(window, &client_offset))
		throw exception("Could not get client area offset.");

	calcRect(cr, client_offset.x, client_offset.y);

	// Find the HWND of the window after which to insert the current window
	// in z-order. This is the window in front of the topmost window occluding
	// the current window (if any).
	bool target_next = false;
	HWND target = 0;
	HWND next = window;
	while (next = GetWindow(next, GW_HWNDPREV))
	{
		RECT wr;
		GetWindowRect(next, &wr);

		if (overlapRect(wr, cr))
			target_next = true;
		else if (target_next)
			target = next, target_next = false;
	}

	if (target_next)
		SetForegroundWindow(window);
	else if (target)
		SetWindowPos(window, target, 0, 0, 0, 0, SWP_NOACTIVATE | SWP_NOMOVE | SWP_NOSIZE);
}

static CaptureState* beginCaptureWindow(HWND window)
{
	if (!IsWindow(window))
		throw exception("Window does not exist.");

	CaptureState* cs = new CaptureState();

	cs->window = window;

	try
	{
		cs->windowDc = GetWindowDC(nullptr);
		cs->memDc = CreateCompatibleDC(cs->windowDc);

		if (!cs->windowDc || !cs->memDc)
			throw exception("Could not get the required device contexts.");

		GetClientRect(window, &cs->cr);

		POINT client_offset = { 0, 0 };
		if (!ClientToScreen(window, &client_offset))
			throw exception("Could not get client area offset.");

		calcRect(cs->cr, client_offset.x, client_offset.y);

		cs->x = cs->cr.left;
		cs->y = cs->cr.top;
		cs->width = cs->cr.right - cs->cr.left;
		cs->height = cs->cr.bottom - cs->cr.top;

		if (cs->width == 0 || cs->height == 0)
			throw exception("Client area width or height is zero.");

		cs->hbmp = CreateCompatibleBitmap(cs->windowDc, cs->width, cs->height);

		if (!cs->hbmp)
			throw exception("Could not create bitmap.");

		SelectObject(cs->memDc, cs->hbmp);

		BITMAPINFOHEADER bi =
		{
			sizeof(BITMAPINFOHEADER),   // biSize
			cs->width,                  // biWidth
			-cs->height,                // biHeight
			1,                          // biPlanes
			32,                         // biBitCount
			BI_RGB,                     // biCompression
			0,                          // biSizeImage
			0,                          // biXPelsPerMeter
			0,                          // biYPelsPerMeter
			0,                          // biClrUser
			0                           // biClrImportant
		};
		cs->bi = bi;

		return cs;
	}
	catch (exception e)
	{
		endCaptureWindow(cs);
		throw e;
	}
}

static void endCaptureWindow(CaptureState*& cs)
{
	if (!cs)
		return;

	DeleteObject(cs->hbmp);
	DeleteObject(cs->memDc);
	ReleaseDC(nullptr, cs->windowDc);

	delete cs;
	cs = 0;
}

static void captureFrame(const CaptureState* cs, Mat& image)
{
	if (!cs)
		throw exception("Capture state is null.");


	if (!BitBlt(cs->memDc, 0, 0, cs->width, cs->height, cs->windowDc, cs->x, cs->y, SRCCOPY))
		throw exception("Copying between device contexts failed.");
	
	image.create(cs->height, cs->width, CV_8UC4);

	if (!image.data)
		throw exception("Could not allocate destination matrix.");

	//BITMAP bmp;
	//GetObject(cs->hbmp, sizeof(BITMAP), &bmp);
	//int nChannels = bmp.bmBitsPixel;


	int success = GetDIBits(cs->memDc, cs->hbmp, 0, (UINT)cs->height, image.data, (BITMAPINFO*)&cs->bi, DIB_RGB_COLORS);

	if (!success || success == ERROR_INVALID_PARAMETER)
		throw exception("Could not copy bitmap to matrix.");
}



static int32_t findPic(Mat &image, Mat &templ, double min, double max, int method, Point &result)
{
	Mat imageTmp;
	cvtColor(image, imageTmp, CV_BGRA2BGR);
	//模板本身也是有一定体积的,所以result的宽高只能是这样
	int width = imageTmp.cols - templ.cols + 1;
	int height = imageTmp.rows - templ.rows + 1;
	//将各像素的匹配值也输出为图像
	Mat match(width, height, CV_32FC1);
	matchTemplate(imageTmp, templ, match, method);
	//normalize(match, match, 0, 1, NORM_MINMAX, -1, Mat());


	cv::Point minLoc;
	cv::Point maxLoc;
	double minValue, maxValue;
	minMaxLoc(match, &minValue, &maxValue, &minLoc, &maxLoc, Mat());
	if (method == CV_TM_SQDIFF || method == CV_TM_SQDIFF_NORMED)
	{
		//Rect rect(minLoc.x, minLoc.y, templ.cols, templ.rows);
		//Mat roiImage = image(rect);
		if (minValue <= min)
		{
			rectangle(image, Rect(minLoc.x, minLoc.y, templ.cols, templ.rows), Scalar(0, 0, 255), 2);
			result.x = minLoc.x;
			result.y = minLoc.y;
			return 1;
		}
		else
		{
			rectangle(image, Rect(minLoc.x, minLoc.y, templ.cols, templ.rows), Scalar(0, 255, 0), 2);

		}
	}
	else
	{
		if (maxValue >= max)
		{
			rectangle(image, Rect(maxLoc.x, maxLoc.y, templ.cols, templ.rows), Scalar(0, 0, 255), 2);
			result.x = maxLoc.x;
			result.y = maxLoc.y;
			return 1;
		}
		else
		{
			rectangle(image, Rect(maxLoc.x, maxLoc.y, templ.cols, templ.rows), Scalar(0, 255, 0), 2);
		}
	}
	return 0;

}
static int32_t findPicBySize(Mat &image, Mat &templ, Rect& rect, double min, double max, int method, Point &result)
{
	Mat imageTmp;
	Mat roiImage = image(rect);
	cvtColor(roiImage, imageTmp, CV_BGRA2BGR);
	//imshow("1", imageTmp);
	//imshow("2", templ);

	//模板本身也是有一定体积的,所以result的宽高只能是这样
	int width = imageTmp.cols - templ.cols + 1;
	int height = imageTmp.rows - templ.rows + 1;
	//将各像素的匹配值也输出为图像
	Mat match(width, height, CV_32FC1);
	matchTemplate(imageTmp, templ, match, method);
	//normalize(match, match, 0, 1, NORM_MINMAX, -1, Mat());


	cv::Point minLoc;
	cv::Point maxLoc;
	double minValue, maxValue;
	minMaxLoc(match, &minValue, &maxValue, &minLoc, &maxLoc, Mat());
	if (method == CV_TM_SQDIFF || method == CV_TM_SQDIFF_NORMED)
	{
		if (minValue <= min)
		{
			rectangle(image, Rect(rect.x + minLoc.x, rect.y + minLoc.y, templ.cols, templ.rows), Scalar(0, 0, 255), 2);
			result.x = rect.x + minLoc.x;
			result.y = rect.y + minLoc.y;
			return 1;
		}
		else
		{
			rectangle(image, Rect(rect.x + minLoc.x, rect.y + minLoc.y, templ.cols, templ.rows), Scalar(0, 255, 0), 2);

		}
	}
	else
	{
		if (maxValue >= max)
		{
			rectangle(image, Rect(rect.x + maxLoc.x, rect.y + maxLoc.y, templ.cols, templ.rows), Scalar(0, 0, 255), 2);
			result.x = rect.x + maxLoc.x;
			result.y = rect.y + maxLoc.y;
			return 1;
		}
		else
		{
			rectangle(image, Rect(rect.x + maxLoc.x, rect.y + maxLoc.y, templ.cols, templ.rows), Scalar(0, 255, 0), 2);
		}
	}
	return 0;

}

static int32_t findColor(Mat &image, Scalar hsvMax, Scalar hsvMin, uint32_t w, uint32_t h, uint32_t tmax, uint32_t tmin, Point &result)
{
	int32_t ret = 0;
	Mat tempImage;
	cvtColor(image, tempImage, CV_BGR2HSV);
	//imshow("4", tempImage);

	Mat dstImage = Mat::zeros(tempImage.size(), CV_8U);
	inRange(tempImage, hsvMin, hsvMax, dstImage);
	//imshow("1", dstImage);
	Mat element = getStructuringElement(MORPH_RECT, Size(w- tmin, h- tmin));
	morphologyEx(dstImage, dstImage, MORPH_CLOSE, element);
	//imshow("2", dstImage);
	morphologyEx(dstImage, dstImage, MORPH_OPEN, element);
	//imshow("3", dstImage);
	vector<vector<Point>>contours;
	findContours(dstImage, contours, RETR_EXTERNAL, CHAIN_APPROX_NONE);
	vector<Rect>rect(contours.size());
	result.x = 0;
	result.y = 0;
	int size = 0;
	for (int i = 0; i < (int)contours.size(); i++)
	{
		rect[i] = boundingRect(contours[i]);
		int x = rect[i].x;
		int y = rect[i].y;
		int width = rect[i].width;
		int height = rect[i].height;
		int sizetmp = width * height;

		if ((width >= (int)((int)w-(int)tmin)) && (width <= (int)((int)w+(int)tmax)) && (height >= (int)((int)h-(int)tmin)) && (height <= (int)((int)h+(int)tmax)))
		{
			rectangle(image, Point(x, y), Point(x + width, y + height), Scalar(0, 0, 255), 2);
			result.x = (size > sizetmp )? result.x : x;
			result.y = (size > sizetmp) ? result.y : y;
			ret = 1;
		}
		else
		{
			rectangle(image, Point(x, y), Point(x + width, y + height), Scalar(0, 255, 0), 2);
		}
	}
	return ret;
}
static int32_t findColorBySize(Mat &image,Rect& rect, Scalar hsvMax, Scalar hsvMin, uint32_t w, uint32_t h, uint32_t tmax, uint32_t tmin, Point &result)
{
	int32_t ret = 0;
	Mat tempImage = image(rect);
	//imshow("1", tempImage);
	cvtColor(tempImage, tempImage, CV_BGR2HSV);
	//imshow("2", tempImage);
	Mat dstImage = Mat::zeros(tempImage.size(), CV_8U);
	inRange(tempImage, hsvMin, hsvMax, dstImage);
	//imshow("3", dstImage);
	Mat element = getStructuringElement(MORPH_RECT, Size(w- tmin, h- tmin));
	morphologyEx(dstImage, dstImage, MORPH_DILATE, element);
	//imshow("4", dstImage);
	morphologyEx(dstImage, dstImage, MORPH_ERODE, element);
	//imshow("5", dstImage);
	vector<vector<Point>>contours;
	findContours(dstImage, contours, RETR_EXTERNAL, CHAIN_APPROX_NONE);
	vector<Rect>rect_tmp(contours.size());
	result.x = 0;
	result.y = 0;
	int size = 0;
	for (int i = 0; i < (int)contours.size(); i++)
	{
		rect_tmp[i] = boundingRect(contours[i]);
		int x = rect_tmp[i].x;
		int y = rect_tmp[i].y;
		int width = rect_tmp[i].width;
		int height = rect_tmp[i].height;
		int sizetmp = width * height;
		if ((width >= (int)((int)w- tmin)) && (width <= (int)((int)w+ tmax)) && (height >= (int)((int)h- tmin)) && (height <= (int)((int)h+ tmax)))
		{
			rectangle(image, Rect(rect.x + x, rect.y + y, width, height), Scalar(0, 0, 255), 2);
			result.x = (size > sizetmp) ? result.x : x + rect.x;
			result.y = (size > sizetmp) ? result.y : y + rect.y;
			ret = 1;
		}
		else
		{
			rectangle(image, Rect(rect.x + x, rect.y + y, width, height), Scalar(0, 255, 0), 2);
		}
	}
	return ret;
}


static int32_t pickColorByPosi(Mat &image, uint32_t x, uint32_t y, Vec3b &colorBGR, Vec3b &colorHSV)
{
	Mat hsvimage;
	cvtColor(image, hsvimage, CV_BGR2HSV);
	Vec3b colorTmpx;
	Vec3b colorTmpy;

	unsigned char b = 0;
	unsigned char g = 0;
	unsigned char r = 0;

	for (int i = -1; i <= 1; i++)
	{
		colorTmpx = image.at<Vec3b>(y, x+i);//读取原图像(y, x)的BGR颜色值，如果是灰度图像，将Vec3b改为uchar
		colorTmpy = image.at<Vec3b>(y+i, x);//读取原图像(y, x)的BGR颜色值，如果是灰度图像，将Vec3b改为uchar
		b += colorTmpx(0) + colorTmpy(0);
		g += colorTmpx(1) + colorTmpy(1);
		r += colorTmpx(2) + colorTmpy(2);
	}

	colorBGR(0) = b/6;
	colorBGR(1) = g/6;
	colorBGR(2) = r/6;

	unsigned char h = 0;
	unsigned char s = 0;
	unsigned char v = 0;
	for (int i = -1; i <= 1; i++)
	{
		colorTmpx = hsvimage.at<Vec3b>(y, x + i);//读取原图像(y, x)的BGR颜色值，如果是灰度图像，将Vec3b改为uchar
		colorTmpy = hsvimage.at<Vec3b>(y + i, x);//读取原图像(y, x)的BGR颜色值，如果是灰度图像，将Vec3b改为uchar
		h += colorTmpx(0) + colorTmpy(0);
		s += colorTmpx(1) + colorTmpy(1);
		v += colorTmpx(2) + colorTmpy(2);
	}
	colorHSV(0) = h / 4;
	colorHSV(1) = s / 4;
	colorHSV(2) = v / 4;
	//circle(image, Point(x, y), 4, cv::Scalar(0, 0, 255));//在图像中画出特征点，2是圆的半径
	//imshow("cc", image);
	return 1;

}

/* class CaptureScreen */
CaptureScreen::CaptureScreen(HWND window)
	:m_window(window), m_state(nullptr), m_w(30), m_h(30), m_tmax(10), m_tmin(5), m_pmin(0.1), m_pmax(0.9), m_method(CV_TM_CCOEFF_NORMED),m_opWin(true)
{ 

}

CaptureScreen::~CaptureScreen()
{
	if (m_state)
		endCaptureWindow(m_state);
}

void CaptureScreen::initialize()
{
	if (m_state)
		return;
	::initialize();
	if (m_opWin)
	{
		revealWindow(m_window);
	}
	m_state = beginCaptureWindow(m_window);
}


void CaptureScreen::captureFrame(Mat& image)
{
	initialize();
	::captureFrame(m_state, image);
}

int32_t CaptureScreen::findPicByTv(Mat &image, Mat &templ, double tv, Point &result)
{
	if (m_method == CV_TM_SQDIFF || m_method == CV_TM_SQDIFF_NORMED)
	{

		return ::findPic(image, templ, tv, m_pmax, m_method, result);
	}
	else
	{
		return ::findPic(image, templ, m_pmin, tv, m_method, result);

	}
}
int32_t CaptureScreen::findPic(Mat &image, Mat &templ, Point &result)
{

	return ::findPic(image, templ, m_pmin, m_pmax, m_method, result);

}
int32_t CaptureScreen::findPicByTvBySize(Mat &image, Mat &templ, Rect& rect, double tv, Point &result)
{
	if (m_method == CV_TM_SQDIFF || m_method == CV_TM_SQDIFF_NORMED)
	{

		return ::findPicBySize(image, templ, rect, tv, m_pmax, m_method, result);
	}
	else
	{
		return ::findPicBySize(image, templ, rect, m_pmin, tv, m_method, result);

	}
}
int32_t CaptureScreen::findPicBySize(Mat &image, Mat &templ, Rect& rect, Point &result)
{

	return ::findPicBySize(image, templ, rect, m_pmin, m_pmax, m_method, result);

}
int32_t CaptureScreen::findColorByTv(Mat &image, Scalar hsvMax, Scalar hsvMin, uint32_t w, uint32_t h, uint32_t tmax, uint32_t tmin, Point &result)
{
	return ::findColor(image, hsvMax, hsvMin, w, h, tmax, tmin, result);
}
int32_t CaptureScreen::findColor(Mat &image, Scalar hsvMax, Scalar hsvMin, Point &result)
{
	return ::findColor(image, hsvMax, hsvMin, m_w, m_h, m_tmax, m_tmin, result);
}
int32_t CaptureScreen::findColorByTvBySize(Mat &image, Rect& rect, Scalar hsvMax, Scalar hsvMin, uint32_t w, uint32_t h, uint32_t tmax, uint32_t tmin, Point &result)
{
	return ::findColorBySize(image, rect, hsvMax, hsvMin, w, h, tmax, tmin, result);
}
int32_t CaptureScreen::findColorBySize(Mat &image, Rect& rect, Scalar hsvMax, Scalar hsvMin, Point &result)
{
	return ::findColorBySize(image, rect, hsvMax, hsvMin, m_w, m_h, m_tmax, m_tmin, result);
}

int32_t CaptureScreen::pickColorByPosi(Mat &image, uint32_t x, uint32_t y, Vec3b &colorBGR, Vec3b &colorHSV)
{
	return ::pickColorByPosi(image,x, y, colorBGR, colorHSV);
}

void CaptureScreen::setColorWidth(uint32_t w)
{
	m_w = w;
}
void CaptureScreen::setColorHeight(uint32_t h)
{
	m_h = h;
}
void CaptureScreen::setColorThred(uint32_t tmax, uint32_t tmin)
{
	m_tmax = tmax;
	m_tmin = tmin;
}
void CaptureScreen::setPicMin(double min)
{
	m_pmin = min;
}
void CaptureScreen::setPicMax(double max)
{
	m_pmax = max;
}
void CaptureScreen::setMethod(int method)
{
	if (method == CV_TM_SQDIFF ||
		method == CV_TM_SQDIFF_NORMED ||
		method == CV_TM_CCORR ||
		method == CV_TM_CCORR_NORMED ||
		method == CV_TM_CCOEFF ||
		method == CV_TM_CCOEFF_NORMED)
	{
		m_method = method;
	}
}