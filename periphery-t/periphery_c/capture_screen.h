#pragma once

#include <windows.h>
#include <opencv2/opencv.hpp>



struct CaptureState
{
	HWND window;
	RECT cr;
	HDC windowDc;
	HDC memDc;
	HBITMAP hbmp;
	BITMAPINFOHEADER bi;
	int width, height;
	int x, y;
};

class CaptureScreen
{
public:

	CaptureScreen(HWND window);
	~CaptureScreen();

	void captureFrame(cv::Mat &image);
	int32_t findPicByTv(cv::Mat &image, cv::Mat &templ, double tv, cv::Point &result);
	int32_t findPic(cv::Mat &image, cv::Mat &templ, cv::Point &result);
	int32_t findPicByTvBySize(cv::Mat &image, cv::Mat &templ, cv::Rect& rect, double tv, cv::Point &result);
	int32_t findPicBySize(cv::Mat &image, cv::Mat &templ, cv::Rect& rect, cv::Point &result);
	int32_t findColorByTv(cv::Mat &image, cv::Scalar hsvMax, cv::Scalar hsvMin, uint32_t w, uint32_t h, uint32_t tmax, uint32_t tmin, cv::Point &result);
	int32_t findColor(cv::Mat &image, cv::Scalar hsvMax, cv::Scalar hsvMin, cv::Point &result);
	int32_t findColorByTvBySize(cv::Mat &image, cv::Rect& rect, cv::Scalar hsvMax, cv::Scalar hsvMin, uint32_t w, uint32_t h, uint32_t tmax, uint32_t tmin, cv::Point &result);
	int32_t findColorBySize(cv::Mat &image, cv::Rect& rect, cv::Scalar hsvMax, cv::Scalar hsvMin, cv::Point &result);
	int32_t pickColorByPosi(cv::Mat &image, uint32_t x, uint32_t y, cv::Vec3b &colorBGR, cv::Vec3b &colorHSV);

	void setColorWidth(uint32_t w);
	void setColorHeight(uint32_t h);
	void setColorThred(uint32_t tmax, uint32_t tmin);
	void setPicMin(double min);
	void setPicMax(double max);
	void setMethod(int method);

private:
	void initialize();
private:
	const HWND m_window;
	CaptureState* m_state;
	uint32_t m_w;
	uint32_t m_h;
	uint32_t m_tmax;
	uint32_t m_tmin;
	double m_pmin;
	double m_pmax;
	int m_method;
	bool m_opWin;

};


