#include "XDataThread.h"

XDataThread::XDataThread()
{
}

XDataThread::~XDataThread()
{
}

void XDataThread::Push(XData d)
{
	std::unique_lock<std::mutex> lock(mux);
	while (datas.size() >= MAX_LIST)
	{
		//cv.wait(lock);
		datas.front().Drop();//¶ªÆú×îÔçµÄÒ»Ö¡
		datas.pop_front();
	}
	datas.push_back(d);
}

XData XDataThread::Pop()
{
	std::lock_guard<std::mutex> lock(mux);
	if (datas.empty())
	{
		return XData();
	}
	XData d = datas.front();
	datas.pop_front();
	return d;
}

void XDataThread::Start()
{
	isExit = false;
	QThread::start();
}

void XDataThread::Stop()
{
	isExit = true;
	wait();
}

void XDataThread::Clear()
{
	std::unique_lock<std::mutex> lock(mux);
	while (!datas.empty())
	{
		datas.front().Drop();
		datas.pop_front();
	}
}
