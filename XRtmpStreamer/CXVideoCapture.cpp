#include "CXVideoCapture.h"

CXVideoCapture::CXVideoCapture()
{
}

CXVideoCapture::~CXVideoCapture()
{
}

bool CXVideoCapture::Init(int camIdx)
{
	Stop();
	cam.open(camIdx);
	if (!cam.isOpened())
	{
		cout << "!cam.isOpened()" << endl;
		return false;
	}
	cout << "cam.isOpened()" << endl;

	width = cam.get(CAP_PROP_FRAME_WIDTH);
	height = cam.get(CAP_PROP_FRAME_HEIGHT);
	fps = cam.get(CAP_PROP_FPS);//帧率
	return true;
}

bool CXVideoCapture::Init(const char* url)
{
	return false;
}

void CXVideoCapture::Stop()
{
	XDataThread::Stop();
	if (cam.isOpened())
	{
		cam.release();
	}
}

void CXVideoCapture::run()
{
	Mat frame;
	while (!isExit)
	{
		if (!cam.read(frame))
		{
			msleep(1);
			continue;
		}
		if (frame.empty())
		{
			continue;
		}	
		long long t1 = GetCurrentTime();
		std::unique_lock<std::mutex> lck(mux);
		for (uint i = 0; i < filters.size(); i++)
		{
			Mat dstFrame;
			filters[i]->Filter(&frame, &dstFrame);
			frame = dstFrame;
		}
		//计算美颜处理延时
		long long t2 = GetCurrentTime() - t1;
		//mux.unlock();

		Push(XData((char*)frame.data, frame.cols * frame.rows * frame.elemSize(),GetCurrentTime() + t2));
	}
}

bool CXVideoCapture::AddFilter(XFilter* filter)
{
	std::unique_lock<std::mutex> lck(mux); 
	if (!filter) return false;
	filters.push_back(filter);
	return true;
}
