#pragma once
#include <iostream>
#include <opencv2/opencv.hpp>
#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include "XDataThread.h"
#include <vector>
#include "XFilter.h"
using namespace std;
using namespace cv;
class XVideoCapture :
    public XDataThread
{
public:
	~XVideoCapture();

	static XVideoCapture* Get(unsigned char index = 0);//工厂模式

	virtual bool Init(int camIdx = 0) = 0;
	virtual bool Init(const char *url) = 0;
	virtual void Stop() = 0;
	virtual void run() = 0;
	//添加滤镜
	virtual bool AddFilter(XFilter* filter) = 0;
	

	int width = 1280;
	int height = 720;
	int fps = 25;

protected:
	XVideoCapture();
	vector<XFilter*> filters;
};

