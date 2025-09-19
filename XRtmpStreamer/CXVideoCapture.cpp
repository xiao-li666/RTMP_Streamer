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
	fps = cam.get(CAP_PROP_FPS);//Ö¡ÂÊ
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
		std::unique_lock<std::mutex> lck(mux);
		for (uint i = 0; i < filters.size(); i++)
		{
			Mat dstFrame;
			filters[i]->Filter(&frame, &dstFrame);
			frame = dstFrame;
		}
		//mux.unlock();

		Push(XData((char*)frame.data, frame.cols * frame.rows * frame.elemSize(),GetCurrentTime()));
	}
}

bool CXVideoCapture::AddFilter(XFilter* filter)
{
	std::unique_lock<std::mutex> lck(mux); 
	if (!filter) return false;
	filters.push_back(filter);
	return true;
}
